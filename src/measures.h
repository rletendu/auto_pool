
#ifndef __MEASURES_H__
#define __MEASURES_H__

#include <Arduino.h>
#include "config.h"

struct MeasuresStructure
{
	uint32_t index;
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
void measures_to_json_string(void);
bool measures_json_to_measures(char *json_str);
void measures_init(void);
void measures_loop(void);
void measures_set_virtual(bool state);

#endif