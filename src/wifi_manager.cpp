#include <WiFiManager.h> //https://github.com/Brunez3BD/WIFIMANAGER-ESP32
#include "parameters.h"
#include <SerialDebug.h>
#include <config.h>

WiFiManager wifiManager;

static bool shouldSaveConfig = false;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", parameters.mqtt_server, 40);
WiFiManagerParameter custom_mqtt_port("port", "mqtt port", parameters.mqtt_port, 6);
WiFiManagerParameter custom_mqtt_user("user", "mqtt user", parameters.mqtt_user, 20);
WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", parameters.mqtt_pass, 20);
WiFiManagerParameter custom_mqtt_base_topic("topic", "mqtt topic", parameters.mqtt_base_topic, 20);
WiFiManagerParameter custom_text("</p>MQTT Server configuration");

void saveConfigCallback()
{
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

bool is_should_save_config(void)
{
  return shouldSaveConfig;
}

void wifimanager_init(void)
{
  printlnA(F("Setup Wifi manager..."));
  wifiManager.setDebugOutput(false);
  wifiManager.setSaveConfigCallback(saveConfigCallback);
  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);
  wifiManager.addParameter(&custom_mqtt_base_topic);
  if (parameters_read_json())
  {
    custom_mqtt_server.setValue(parameters.mqtt_server);
    custom_mqtt_port.setValue(parameters.mqtt_port);
    custom_mqtt_user.setValue(parameters.mqtt_user);
    custom_mqtt_pass.setValue(parameters.mqtt_pass);
    custom_mqtt_base_topic.setValue(parameters.mqtt_base_topic);
  }
}

void reset_wifimanager(void)
{
  wifiManager.resetSettings();
}

void wifimanager_autoconnect(void)
{
  wifiManager.autoConnect(PORTAL_NAME);
  printlnA(F("Connected !"));

  //read updated parameters
  strcpy(parameters.mqtt_server, custom_mqtt_server.getValue());
  strcpy(parameters.mqtt_port, custom_mqtt_port.getValue());
  strcpy(parameters.mqtt_user, custom_mqtt_user.getValue());
  strcpy(parameters.mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(parameters.mqtt_base_topic, custom_mqtt_base_topic.getValue());
  if (is_should_save_config())
  {
    printlnA(F("Saveconfig Detected !"));
    parameters_write_json();
  }
}

void wifimanager_start_portal(void)
{
  printlnA(F("Portal request"));
  wifiManager.startConfigPortal(PORTAL_NAME);
  strcpy(parameters.mqtt_server, custom_mqtt_server.getValue());
  strcpy(parameters.mqtt_port, custom_mqtt_port.getValue());
  strcpy(parameters.mqtt_user, custom_mqtt_user.getValue());
  strcpy(parameters.mqtt_pass, custom_mqtt_pass.getValue());
  strcpy(parameters.mqtt_base_topic, custom_mqtt_base_topic.getValue());
  if (is_should_save_config())
  {
    printlnA(F("Need to write Json config file..."));
    parameters_write_json();
  }
}