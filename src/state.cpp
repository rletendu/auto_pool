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

void orp_state_to_json_string(void)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["cl_mode"] = (int)state.cl_mode;
	json["cl_pump"] = (int)state.cl_pump;
	json["orp_control_state"] = (int)state.orp_control_state;
	json.printTo(state_orp_json_string, sizeof(state_orp_json_string));
	printA("ORP Json:");
	printlnA(state_orp_json_string);
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
