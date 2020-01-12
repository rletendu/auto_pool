
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <WiFi.h> 
#include <WebServer.h>
#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32
#include <timer.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson

#include <SimpleCLI.h>

// Create CLI Object
SimpleCLI cli;

// Commands
Command cmd_portal;
WiFiManager wifiManager;

#define LED_BUILTIN 2

// #define mqtt_server       "xxx.cloudmqtt.com"
// #define mqtt_port         "12345"
// #define mqtt_user         "mqtt_user"
// #define mqtt_pass         "mqtt_pass"
// #define humidity_topic    "sensor/humidity"

char mqtt_server[50] = "xxx.cloudmqtt.com";
char mqtt_port[50]  =       "12345";
char mqtt_user[50]   =      "mqtt_user";
char mqtt_pass[50]   =      "mqtt_pass";
char humidity_topic[50] =   "sensor/humidity";

auto timer = timer_create_default(); // create a timer with default settings
uintptr_t task;

volatile uint32_t cpt=0;
bool shouldSaveConfig = false;
uint16_t port;

void cowsayCallback(cmd* c) {
    Command cmd(c); // Create wrapper object

    Serial.println("portal request");
    wifiManager.startConfigPortal();
}


bool toggle_led(void *) {

  cpt++;
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED
  return true; // repeat? true
}

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

void setup() {

  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);
  WiFiManagerParameter custom_text("</p>MQTT Server");


  Serial.begin(9600);
  delay(500);

  cmd_portal = cli.addSingleArgCmd("portal", cowsayCallback);
  //clean FS for testing 
  //SPIFFS.format();

  //read configuration from FS json
  Serial.println("mounting FS...");
  if (SPIFFS.begin()) {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json")) {
      //file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile) {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject& json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success()) {
          Serial.println("\nparsed json");
          strcpy(mqtt_server, json["mqtt_server"]);
          strcpy(mqtt_port, json["mqtt_port"]);
          strcpy(mqtt_user, json["mqtt_user"]);
          strcpy(mqtt_pass, json["mqtt_pass"]);
          port = json["mqtt_port"];
          Serial.print("Port : ");
          Serial.println(port);

        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
  //end read



  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT
  digitalWrite(LED_BUILTIN, true); 
  
  //wifiManager.resetSettings();

  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

  wifiManager.autoConnect("AutoConnectAP");
  //wifiManager.startConfigPortal();

  Serial.println("connected...yeey :)");


  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());




//save the custom parameters to FS
if (shouldSaveConfig) {
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_server"] = mqtt_server;
  json["mqtt_port"] = mqtt_port;
  json["mqtt_user"] = mqtt_user;
  json["mqtt_pass"] = mqtt_pass;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}


  // call the toggle_led function every 500 millis (half second)
  //timer.every(1000, toggle_led);
  task = timer.every(500, toggle_led);

  
}

volatile unsigned long tmp=0;
volatile unsigned long target=0x5000;
unsigned long duration;

void loop() {

  timer.tick(); // tick the timer
  if (cpt>10) {
    timer.cancel(task);
  }

  if (Serial.available()) {
      // Read out string from the serial monitor
      String input = Serial.readStringUntil('\n');

      // Parse the user input into the CLI
      cli.parse(input);
  }
}