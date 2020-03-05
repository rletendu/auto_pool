#include <Arduino.h>
#include "config.h"

struct MeasuresStructure
{
	float water_temperature;
	float system_temperature;
	float system_humidity;
	float pump_pressure;
	float ph;
	float orp;
	bool level_water;
	bool level_cl;
	bool level_ph_minus;
	bool level_ph_plus;
};

extern struct MeasuresStructure measures;
extern char measures_json_string[MEASURES_JSON_MESSAGE_LEN];