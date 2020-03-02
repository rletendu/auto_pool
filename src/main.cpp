#include <config.h>
#include <board.h>
#include <ota.h>
#include "display.h"
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_manager.h"
#include <timer.h>
#include <SerialDebug.h>
#include <SPIFFS.h>

#include <SimpleCLI.h>
#include "parameters.h"
#include "mqtt.h"

#include <ArduinoOTA.h>
#include "time.h"
#include <Nextion.h>

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
  printlnA(F("Portal request"));
  wifimanager_start_portal();
  printlnA(F("Rebooting..."));
  ESP.restart();
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

void printLocalTime()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    Serial.println("Failed to obtain time");
    return;
  }
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  //parameters_format();
  board_init();
  delay(500);

  printlnA(F("AutoPool Starting..."));
  setup_wifimanager();
  wifimanager_autoconnect();
  parameters_read_json();
  printlnA(F("Ip Address : "));
  printlnA(WiFi.localIP());
  ota_init();
  mqtt_init();
  configTime(GMTOFFSET, DAYLIGHTOFFSET, NTPSERVER);
  printLocalTime();

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
  ota_loop();
  display_loop();
  mqtt_loop();
  timer.tick(); // tick the timer
  if (cpt > 10)
  {
    timer.cancel(task);
  }

  if (Serial.available())
  {
    String input = Serial.readStringUntil('\n');
    cli.parse(input);
  }
}