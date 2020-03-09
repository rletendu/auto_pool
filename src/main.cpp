#include <config.h>
#include <board.h>
#include <ota.h>
#include "display.h"
#include <WiFi.h>
#include <WebServer.h>
#include "wifi_manager.h"
#include "soft_timer.h"
#include <SerialDebug.h>
#include "orp_control.h"
#include "ph_control.h"
#include "filter_control.h"
#include "cli.h"

#include "parameters.h"
#include "mqtt.h"

#include <ArduinoOTA.h>
#include "time.h"
#include <Nextion.h>

SoftTimer timer_pool = SoftTimer();

uintptr_t task;

volatile uint32_t cpt = 0;

uint16_t port;

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
    printlnA(F("Failed to obtain time"));
    return;
  }
  rtc_set_time(timeinfo);
  Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
  Serial.begin(115200);
  //parameters_format();
  board_init();
  delay(500);

  printlnA(F("AutoPool Starting..."));
  wifimanager_init();
  wifimanager_autoconnect();
  parameters_read_json();
  printlnA(F("Ip Address : "));
  printlnA(WiFi.localIP());
  ota_init();
  mqtt_init();
  cli_init();
  configTime(GMTOFFSET, DAYLIGHTOFFSET, NTPSERVER);
  printLocalTime();
  filter_control_init();
  orp_control_init();
  ph_control_init();

  //save the custom parameters to FS
  if (is_should_save_config())
  {
    printlnA(F("Need to write Json config file..."));
    parameters_write_json();
  }
  task = timer_pool.every(500, toggle_led);
}

volatile unsigned long tmp = 0;
volatile unsigned long target = 0x5000;
unsigned long duration;

void loop()
{
  ota_loop();
  display_loop();
  mqtt_loop();
  cli_loop();

  timer_pool.tick(); // tick the timer
  if (cpt > 10)
  {
    timer_pool.cancel(task);
  }
}