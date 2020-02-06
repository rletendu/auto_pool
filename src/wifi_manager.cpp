#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32
#include "parameters.h"

WiFiManager wifiManager;

static bool shouldSaveConfig = false;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", parameters.mqtt_server, 40);
  //itoa(parameters.mqtt_port, str, 10);
WiFiManagerParameter custom_mqtt_port("port", "mqtt port", parameters.mqtt_port, 6);
WiFiManagerParameter custom_mqtt_user("user", "mqtt user", parameters.mqtt_user, 20);
WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", parameters.mqtt_pass, 20);
WiFiManagerParameter custom_text("</p>MQTT Server");

void saveConfigCallback () {
  Serial.println("Should save config");
  shouldSaveConfig = true;
}

bool is_should_save_config(void)
{
  return shouldSaveConfig;
}

void setup_wifimanager(void)
{
  char str[PARAM_MAX_STR];


  //set config save notify callback
  wifiManager.setSaveConfigCallback(saveConfigCallback);

  wifiManager.addParameter(&custom_text);
  wifiManager.addParameter(&custom_mqtt_server);
  wifiManager.addParameter(&custom_mqtt_port);
  wifiManager.addParameter(&custom_mqtt_user);
  wifiManager.addParameter(&custom_mqtt_pass);

}

void reset_wifimanager(void)
{
  wifiManager.resetSettings();
}

void wifimanager_autoconnect(void)
{
  wifiManager.autoConnect("AutoConnectAP");
  //wifiManager.startConfigPortal();

  Serial.println("connected...yeey :)");


  //read updated parameters
  strcpy(mqtt_server, custom_mqtt_server.getValue());
  strcpy(mqtt_port, custom_mqtt_port.getValue());
  strcpy(mqtt_user, custom_mqtt_user.getValue());
  strcpy(mqtt_pass, custom_mqtt_pass.getValue());
}