#include "parameters.h"
#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <SPIFFS.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson
#include <SerialDebug.h>
#include "config.h"

struct ParametersStruture parameters;
void parameters_set_default(void);
char parameters_json_string[PARAMETERS_JSON_MESSAGE_LEN];

bool parameters_read_file(void)
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
				if (parameters_json_to_param(buf.get()))
				{
					return true;
				}
				else
				{
					printlnA(F("failed to load parameters from json config"));
					return false;
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

bool parameters_write_file(void)
{
	uint8_t i;
	printlnA(F("Saving parameters to file"));
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
	json["cl_max_day"] = parameters.cl_max_day;
	json["phm_max_day"] = parameters.phm_max_day;
	json["orp_offset"] = parameters.orp_offset;
	json["ph_offset"] = parameters.ph_offset;
	json["periodic_filter_time"] = parameters.periodic_filter_time;
	JsonArray &data = json.createNestedArray("timer_prog_temperature");
	for (i = 0; i < PARAM_NB_TEMP_TIMER_PROG; i++)
	{
		data.add(parameters.timer_prog_temperature[i]);
	}
	File configFile = SPIFFS.open(PARAMETER_FILENAME, "w");
	if (!configFile)
	{
		printlnA(F("failed to open config file for writing"));
		return false;
	}
	json.printTo(configFile);
	json.printTo(parameters_json_string);
	configFile.close();
	printlnA("Parameter Json:");
	printlnA(parameters_json_string);
	return true;
}

void parameters_format(void)
{
	SPIFFS.format();
}

void parameters_set_default(void)
{
	uint8_t i;
	parameters.target_ph = 7.4;
	parameters.delta_ph = 0.15;
	parameters.target_orp = 650;
	parameters.delta_orp = 70;
	parameters.flow_cl = 25.0;
	parameters.flow_ph_minus = 25.0;
	parameters.flow_ph_plus = 25.0;
	parameters.filter_auto_mode = AUTO_TIMER_PROG;
	parameters.timer_prog = 0b111111111111000000000000; // 16773120; //12h to 24h
	parameters.pressure_warning = 1.9;

	parameters.cl_max_day = 2000.0;
	parameters.phm_max_day = 2000.0;
	parameters.orp_offset = 0.0;
	parameters.ph_offset = 0.0;
	parameters.periodic_filter_time = 0;
	for (i = 0; i < PARAM_NB_TEMP_TIMER_PROG; i++)
	{
		parameters.timer_prog_temperature[i] = 0;
	}
}

bool parameters_json_to_param(char *json_str)
{
	uint8_t i;
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.parseObject(json_str);
	if (json.success())
	{
		strcpy(parameters.mqtt_server, json["mqtt_server"]);
		strcpy(parameters.mqtt_port, json["mqtt_port"]);
		strcpy(parameters.mqtt_user, json["mqtt_user"]);
		strcpy(parameters.mqtt_pass, json["mqtt_pass"]);
		strcpy(parameters.mqtt_base_topic, json["base_topic"]);
		parameters.target_ph = json["target_ph"];
		parameters.delta_ph = json["delta_ph"];
		parameters.target_orp = json["target_orp"];
		parameters.delta_orp = json["delta_orp"];
		parameters.flow_cl = json["flow_cl"];
		parameters.flow_ph_minus = json["flow_ph_minus"];
		parameters.pressure_warning = json["pressure_warning"];
		parameters.flow_ph_plus = json["flow_ph_plus"];
		parameters.filter_auto_mode = (filter_auto_mode_t)(int)json["filter_auto_mode"];
		parameters.timer_prog = json["timer_prog"];
		parameters.cl_max_day = json["cl_max_day"];
		parameters.phm_max_day = json["phm_max_day"];
		parameters.orp_offset = json["orp_offset"];
		parameters.ph_offset = json["ph_offset"];
		parameters.periodic_filter_time = json["periodic_filter_time"];
		for (i = 0; i < PARAM_NB_TEMP_TIMER_PROG; i++)
		{
			parameters.timer_prog_temperature[i] = json["timer_prog_temperature"][i];
		}
		json.printTo(parameters_json_string);
		return true;
	}
	else
	{
		printlnA("Json str to parameters fail") return false;
	}
}