#include "parameters.h"
#include <FS.h>                   //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include <ArduinoJson.h>          //https://github.com/bblanchon/ArduinoJson


struct ParametersStruture parameters;



void parameters_read_json(void)
{
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
          strcpy(parameters.mqtt_server, json["mqtt_server"]);
          strcpy(parameters.mqtt_port, json["mqtt_port"]);
          strcpy(parameters.mqtt_user, json["mqtt_user"]);
          strcpy(parameters.mqtt_pass, json["mqtt_pass"]);
        } else {
          Serial.println("failed to load json config");
        }
      }
    }
  } else {
    Serial.println("failed to mount FS");
  }
}


void parameters_write_json(void)
{
  Serial.println("saving config");
  DynamicJsonBuffer jsonBuffer;
  JsonObject& json = jsonBuffer.createObject();
  json["mqtt_server"] = parameters.mqtt_server;
  json["mqtt_port"] = parameters.mqtt_port;
  json["mqtt_user"] = parameters.mqtt_user;
  json["mqtt_pass"] = parameters.mqtt_pass;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile) {
    Serial.println("failed to open config file for writing");
  }

  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
  //end save
}

void parameters_format(void)
{
  SPIFFS.format();
}