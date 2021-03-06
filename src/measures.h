
#ifndef __MEASURES_H__
#define __MEASURES_H__

#include <Arduino.h>
#include "config.h"

struct MeasuresStructure
{
	uint32_t index;
	float water_temperature_raw;
	float water_temperature;
	float day_max_water_temperature;
	float system_temperature;
	float system_humidity;
	float pump_pressure;
	float ph_raw;
	float orp_raw;
	float ph;
	float orp;
	bool level_water;
	bool level_cl;
	bool level_ph_minus;
	bool level_ph_plus;
	float daily_ml_orp;
	float daily_ml_ph_plus;
	float daily_ml_ph_minus;
	uint32_t boot_count;
};

extern struct MeasuresStructure measures;
extern char measures_json_string[MEASURES_JSON_MESSAGE_LEN];
void measures_to_json_string(void);
bool measures_json_to_measures(char *json_str);
void measures_init(void);
void measures_set_virtual(bool state);
void measures_loop_stop(void);

#endif