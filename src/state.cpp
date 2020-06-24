#include "autopool.h"

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
	json["filter_power"]= (int)state.filter_power;
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
		state.filter_power = (filter_power_t)(int)json["filter_power"];
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

bool state_write_file(void)
{
	printlnA(F("Saving state to file"));
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["ph_minus_mode"] = (int)state.ph_minus_mode;
	json["ph_plus_mode"] = (int)state.ph_plus_mode;
	json["filter_mode"] = (int)state.filter_mode;
	json["orp_mode"] = (int)state.orp_mode;

	File stateFile = SPIFFS.open(STATE_FILENAME, "w");
	if (!stateFile)
	{
		printlnA(F("failed to open state file for writing"));
		return false;
	}
	json.printTo(stateFile);
	stateFile.close();
	return true;
}

bool state_read_file(void)
{
	if (SPIFFS.begin())
	{
		printlnA(F("mounted file system"));
		if (SPIFFS.exists(STATE_FILENAME))
		{
			printlnA(F("reading state file"));
			File configFile = SPIFFS.open(PARAMETER_FILENAME, "r");
			if (configFile)
			{
				printlnA(F("opened state file"));
				size_t size = configFile.size();
				std::unique_ptr<char[]> buf(new char[size]);
				configFile.readBytes(buf.get(), size);
				DynamicJsonBuffer jsonBuffer;
				JsonObject &json = jsonBuffer.parseObject(buf.get());
				if (json.success())
				{
					state.filter_mode = (filter_mode_t)(int)json["filter_mode"];
					state.orp_mode = (orp_mode_t)(int)json["orp_mode"];
					state.ph_minus_mode = (ph_minus_mode_t)(int)json["ph_minus_mode"];
					state.ph_plus_mode = (ph_plus_mode_t)(int)json["ph_plus_mode"];
					return true;
				}
				else
				{
					printlnA("Json str to parameters fail");
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