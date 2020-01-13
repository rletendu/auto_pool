

#include <WiFi.h> 
#include <WebServer.h>
#include "wifi_manager.h"
#include <timer.h>
#include <SPIFFS.h>

#include <SimpleCLI.h>
#include "parameters.h"

// Create CLI Object
SimpleCLI cli;

// Commands
Command cmd_portal;


#define LED_BUILTIN 2


auto timer = timer_create_default(); // create a timer with default settings
uintptr_t task;

volatile uint32_t cpt=0;
bool shouldSaveConfig = false;
uint16_t port;

void cmd_portalCallback(cmd* c) {
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
  Serial.begin(9600);
  delay(500);
  setup_wifimanager();
  cmd_portal = cli.addSingleArgCmd("portal", cmd_portalCallback);
 
  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT
  digitalWrite(LED_BUILTIN, true); 
  
  //save the custom parameters to FS
  if (shouldSaveConfig) {
    parameters_write_json();
  }
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