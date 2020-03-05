#include "parameters.h"
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <SerialDebug.h>
#include "config.h"

struct ParametersStruture parameters;
void parameters_set_default(void);

bool parameters_read_json(void)
{
  parameters_set_default();
  printlnA(F("mounting FS..."));
  if (SPIFFS.begin())
  {
    printlnA(F("mounted file system"));
    if (SPIFFS.exists(PARAMETER_FILENAME))
    {
      printlnA(F("reading config file"));
      File configFile = SPIFFS.open(PARAMETER_FILENAME, "r");
      if (configFile)
      {
        printlnA(F("opened config file"));
        size_t size = configFile.size();
        std::unique_ptr<char[]> buf(new char[size]);
        configFile.readBytes(buf.get(), size);
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
          printlnA(F("Parsed json"));
          strcpy(parameters.mqtt_server, json["mqtt_server"]);
          strcpy(parameters.mqtt_port, json["mqtt_port"]);
          strcpy(parameters.mqtt_user, json["mqtt_user"]);
          strcpy(parameters.mqtt_pass, json["mqtt_pass"]);
          strcpy(parameters.mqtt_base_topic, json["base_topic"]);
          printA(F("base_topic:"));
          printlnA(parameters.mqtt_base_topic);
          parameters.target_ph = json["target_ph"];
          printA(F("target_ph:"));
          printlnA(parameters.target_ph);
          parameters.delta_ph = json["delta_ph"];
          printA(F("delta_ph:"));
          printlnA(parameters.delta_ph);
          parameters.target_orp = json["target_orp"];
          printA(F("target_orp:"));
          printlnA(parameters.target_orp);
          parameters.delta_orp = json["delta_orp"];
          printA(F("delta_orp:"));
          printlnA(parameters.delta_orp);
          parameters.flow_cl = json["flow_cl"];
          printA(F("flow_cl:"));
          printlnA(parameters.flow_cl);
          parameters.flow_ph_minus = json["flow_ph_minus"];
          printA(F("flow_ph_minus:"));
          printlnA(parameters.flow_ph_minus);
          parameters.pressure_warning = json["pressure_warning"];
          parameters.flow_ph_plus = json["flow_ph_plus"];
          int mode = json["filter_auto_mode"];
          parameters.filter_auto_mode = (filter_auto_mode_t)mode;
          parameters.timer_prog = json["timer_prog"];
          return true;
        }
        else
        {
          printlnA(F("failed to load json config"));
        }
      }
    }
  }
  else
  {
    printlnA(F("failed to mount FS"));
    SPIFFS.format();
  }
  return false;
}

void parameters_write_json(void)
{
  printlnA(F("Saving config"));
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
  json["mqtt_server"] = parameters.mqtt_server;
  json["mqtt_port"] = parameters.mqtt_port;
  json["mqtt_user"] = parameters.mqtt_user;
  json["mqtt_pass"] = parameters.mqtt_pass;
  json["base_topic"] = parameters.mqtt_base_topic;
  json["target_ph"] = parameters.target_ph;
  json["delta_ph"] = parameters.delta_ph;
  json["target_orp"] = parameters.target_orp;
  json["delta_orp"] = parameters.delta_orp;
  json["flow_cl"] = parameters.flow_cl;
  json["flow_ph_minus"] = parameters.flow_ph_minus;
  json["flow_ph_plus"] = parameters.flow_ph_plus;
  json["filter_auto_mode"] = (int)parameters.filter_auto_mode;
  json["timer_prog"] = parameters.timer_prog;
  json["pressure_warning"] = parameters.pressure_warning;
  File configFile = SPIFFS.open(PARAMETER_FILENAME, "w");
  if (!configFile)
  {
    printlnA(F("failed to open config file for writing"));
  }
  json.printTo(Serial);
  json.printTo(configFile);
  configFile.close();
}

void parameters_format(void)
{
  SPIFFS.format();
}

void parameters_set_default(void)
{
  parameters.target_ph = 7.2;
  parameters.delta_ph = 0.2;
  parameters.target_orp = 650;
  parameters.delta_orp = 50;
  parameters.flow_cl = 12.3;
  parameters.flow_ph_minus = 12.1;
}