#include "state.h"
#include "config.h"
#include <SerialDebug.h>
#include "board.h"
#include <ArduinoJson.h>
#include "display.h"

struct StateStructure state;

char state_filter_json_string[STATE_FILTER_CTRL_JSON_MESSAGE_LEN];
char state_ph_json_string[STATE_PH_CTRL_JSON_MESSAGE_LEN];
char state_orp_json_string[STATE_ORP_CTRL_JSON_MESSAGE_LEN];

void filter_state_to_json_string(void)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["filter_mode"] = (int)state.filter_mode;
	json["filter_pump"] = (int)state.filter_pump;
	json["filter_control_state"] = (int)state.filter_control_state;
	json.printTo(state_filter_json_string, sizeof(state_filter_json_string));
	printA("Fiter Json:");
	printlnA(state_filter_json_string);
}

bool filter_state_json_to_state(char *json_str)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.parseObject(json_str);
	if (json.success())
	{
		state.filter_mode = (filter_mode_t)(int)json["filter_mode"];
		state.filter_pump = (pump_state_t)(int)json["filter_pump"];
		state.filter_control_state = (filter_control_state_t)(int)json["filter_control_state"];
		return true;
	}
	else
	{
		return false;
	}
}

void orp_state_to_json_string(void)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["orp_mode"] = (int)state.orp_mode;
	json["orp_pump"] = (int)state.orp_pump;
	json["orp_control_state"] = (int)state.orp_control_state;
	json.printTo(state_orp_json_string, sizeof(state_orp_json_string));
	printA("ORP Json:");
	printlnA(state_orp_json_string);
}

bool orp_state_json_to_state(char *json_str)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.parseObject(json_str);
	if (json.success())
	{
		state.orp_mode = (orp_mode_t)(int)json["orp_mode"];
		state.orp_pump = (pump_state_t)(int)json["orp_pump"];
		state.orp_control_state = (orp_control_state_t)(int)json["orp_control_state"];
		return true;
	}
	else
	{
		return false;
	}
}

void ph_state_to_json_string(void)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["ph_minus_mode"] = (int)state.ph_minus_mode;
	json["ph_plus_mode"] = (int)state.ph_plus_mode;
	json["ph_minus_pump"] = (int)state.ph_minus_pump;
	json["ph_plus_pump"] = (int)state.ph_plus_pump;
	json["ph_control_state"] = (int)state.ph_control_state;
	json.printTo(state_ph_json_string, sizeof(state_ph_json_string));
	printA("pH Json:");
	printlnA(state_ph_json_string);
}

bool ph_state_json_to_state(char *json_str)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.parseObject(json_str);
	if (json.success())
	{
		state.ph_minus_mode = (ph_minus_mode_t)(int)json["ph_minus_mode"];
		state.ph_plus_mode = (ph_plus_mode_t)(int)json["ph_plus_mode"];
		state.ph_minus_pump = (pump_state_t)(int)json["ph_minus_pump"];
		state.ph_plus_pump = (pump_state_t)(int)json["ph_plus_pump"];
		state.ph_control_state = (ph_control_state_t)(int)json["ph_control_state"];
		return true;
	}
	else
	{
		return false;
	}
}