#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32
#include "parameters.h"

WiFiManager wifiManager;


void setup_wifimanager(void)
{
  WiFiManagerParameter custom_mqtt_server("server", "mqtt server", mqtt_server, 40);
  WiFiManagerParameter custom_mqtt_port("port", "mqtt port", mqtt_port, 6);
  WiFiManagerParameter custom_mqtt_user("user", "mqtt user", mqtt_user, 20);
  WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", mqtt_pass, 20);
  WiFiManagerParameter custom_text("</p>MQTT Server");

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