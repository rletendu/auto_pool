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
#include "measures.h"
#include "display_components.h"
#include "parameters.h"
#include "mqtt.h"
#include <ArduinoOTA.h>
#include "time.h"
#include <Nextion.h>

SoftTimer timer_pool = SoftTimer();
uintptr_t time_update_task;

bool time_update(void *)
{
  char msg[20];
  sprintf(msg, "%02u/%02u/%02u %02u:%02u", rtc_get_day(), rtc_get_month(), rtc_get_year(), rtc_get_hour(), rtc_get_minute());
  dis_sys_hour.setText(msg);
  return true; // repeat? true
}

void rtc_init()
{
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo))
  {
    printlnA(F("Failed to obtain time"));
    return;
  }
  timeinfo.tm_year -= 100;
  timeinfo.tm_mon += 1;
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
  rtc_init();
  filter_control_init();
  orp_control_init();
  ph_control_init();
  measures_init();

  //save the custom parameters to FS
  if (is_should_save_config())
  {
    printlnA(F("Need to write Json config file..."));
    parameters_write_json();
  }
  time_update_task = timer_pool.every(60 * 100, time_update);
  printlnA(F("Init Done..."));
}

volatile unsigned long tmp = 0;
volatile unsigned long target = 0x5000;
unsigned long duration;

void loop()
{
  ota_loop();
  measures_loop();
  display_loop();
  mqtt_loop();
  cli_loop();
  filter_control_loop();
  timer_pool.tick(); // tick the timer
}