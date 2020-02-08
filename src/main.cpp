
#include <board.h>
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_manager.h"
#include <timer.h>
#include <SerialDebug.h>
#include <SPIFFS.h>

#include <SimpleCLI.h>
#include "parameters.h"

#include <ArduinoOTA.h>

// Create CLI Object
SimpleCLI cli;

// Commands
Command cmd_portal;
Command cmd_format;

auto timer = timer_create_default();
uintptr_t task;

volatile uint32_t cpt = 0;

uint16_t port;

void cmd_portalCallback(cmd *c)
{
  Command cmd(c);
  Serial.println("Portal request");
  wifiManager.startConfigPortal();
}

void cmd_FormatCallback(cmd *c)
{
  printlnA(F("Format SPIFF..."));
  SPIFFS.format();
}

bool toggle_led(void *)
{
  cpt++;
  led0_toggle();
  return true; // repeat? true
}

void setup()
{
  Serial.begin(115200);
  board_init();
  delay(500);
  printlnA(F("AutoPool Starting..."));
  setup_wifimanager();
  wifimanager_autoconnect();
  parameters_read_json();
  printlnA(F("Ip Address : "));
  printlnA(WiFi.localIP());

  ArduinoOTA
      .onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
          type = "sketch";
        else // U_SPIFFS
          type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
      })
      .onEnd([]() {
        Serial.println("\nEnd");
      })
      .onProgress([](unsigned int progress, unsigned int total) {
        Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
      })
      .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR)
          Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR)
          Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR)
          Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR)
          Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR)
          Serial.println("End Failed");
      });

  ArduinoOTA.begin();

  cmd_portal = cli.addSingleArgCmd("portal", cmd_portalCallback);
  cmd_format = cli.addSingleArgCmd("format", cmd_FormatCallback);


  //save the custom parameters to FS
  if (is_should_save_config())
  {
    printlnA(F("Need to write Json config file..."));
    parameters_write_json();
  }
  task = timer.every(500, toggle_led);
}

volatile unsigned long tmp = 0;
volatile unsigned long target = 0x5000;
unsigned long duration;

void loop()
{
  ArduinoOTA.handle();
  timer.tick(); // tick the timer
  if (cpt > 10)
  {
    timer.cancel(task);
  }

  if (Serial.available())
  {
    // Read out string from the serial monitor
    String input = Serial.readStringUntil('\n');
    // Parse the user input into the CLI
    cli.parse(input);
  }
}