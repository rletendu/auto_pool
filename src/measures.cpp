#include "autopool.h"

struct MeasuresStructure measures;
char measures_json_string[MEASURES_JSON_MESSAGE_LEN];
extern SoftTimer timer_pool;
static bool measures_are_vitual = false;

uintptr_t update_measures_task;
uintptr_t update_graph_task;

extern float daily_ml_ph_minus_backup;
extern float daily_ml_ph_plus_backup;
extern float daily_ml_orp_backup;
extern uint32_t daily_filter_min_backup;
extern uint32_t total_filter_min_backup;
extern int bootCount;
extern uint32_t boot_key;

void measures_to_json_string(void);
bool update_measures(void *);
bool update_graph(void *);

void measures_init(void)
{
	printlnA(F("Mesures Init"));
	measures.index = millis();
	if (boot_key != BOOT_KEY_MAGIC)
	{
		daily_ml_orp_backup = 0;
		daily_ml_ph_minus_backup = 0;
		daily_ml_ph_plus_backup = 0;
		daily_filter_min_backup = 0;
		// Cold boot: total runtime is recovered from state.json (already read by state_default_read_file)
		total_filter_min_backup = state_default.total_filter_min;
	}
	measures.daily_ml_orp = daily_ml_orp_backup;
	measures.daily_ml_ph_minus = daily_ml_ph_minus_backup;
	measures.daily_ml_ph_plus = daily_ml_ph_plus_backup;
	measures.daily_filter_min = daily_filter_min_backup;
	measures.total_filter_min = total_filter_min_backup;
	state.total_filter_min = total_filter_min_backup;
	measures.boot_count = bootCount;
	measures_are_vitual = false;
	#if HAS_MEASURE_CONTROL
	update_measures_task = timer_pool.every(MEASURES_UPDATE_S * 1000, update_measures);
	update_graph_task = timer_pool.every(GRAPH_UPDATE_S * 1000, update_graph);
	update_measures(NULL);
	#else
	printlnA(F("!!! No Measure control !!!"));
	#endif
	
}

void measures_loop_stop(void)
{
	timer_pool.cancel(update_graph_task);
	timer_pool.cancel(update_measures_task);
}

void measures_set_virtual(bool state)
{
	measures_are_vitual = state;
}

bool update_measures(void *)
{
	printA("Updating measures : ");
	float dht;
	static int quiet_measure_cnt = 0;
	static bool first_temp_set = false;
	bool quiet_measure = false;
	static struct MeasuresStructure last_published;
	static unsigned long last_mqtt_ms = 0;

	debug_pin1_on();
	led0_on();
	measures.index = millis() / 1000;
	printlnA(measures.index);

#if HAS_QUIET_MEASURES
	if (++quiet_measure_cnt > (MEASURE_QUIET_MODE_UPDATE_S / MEASURES_UPDATE_S))
	{
		quiet_measure_cnt = 0;
		if (pump_filtration_is_on())
		{
			quiet_measure = true;
			pump_filtration_off();
		}
	}
#endif
	if (measures_are_vitual)
	{
		// Nothing to do here...
	}
	else
	{
		dht = dht_get_temperature();
		if (dht <= 100)
		{
			measures.system_temperature = dht;
		}
		else
		{ // Get alternative system temp from RTC ...
			measures.system_temperature = rtc_get_temperature();
		}

		dht = dht_get_humidity();
		if (dht <= 100)
		{
			measures.system_humidity = dht;
		}
		// Raw measure value
		measures.water_temperature_raw = water_get_temperature();
		// Valid: above 1°C (pool can't be ice) and below 100°C (255 = sensor error)
		bool raw_is_valid = (measures.water_temperature_raw > 1.0f && measures.water_temperature_raw < 100.0f);

		// Update water_temperature only when pump has circulated long enough for an
		// accurate reading, or on the first valid reading after boot (no time limit).
		if (pump_filtration_is_on() &&
		    (abs((int)(millis() - state.filter_time_pump_on)) / 1000) >= FILTER_PUMP_ON_MIN_TIME_S &&
		    raw_is_valid)
		{
			measures.water_temperature = measures.water_temperature_raw;
			first_temp_set = true;
			if (measures.water_temperature_raw > measures.day_max_water_temperature)
			{
				measures.day_max_water_temperature = measures.water_temperature_raw;
				printA("New day_max_water_temperature :");
				printlnA(measures.day_max_water_temperature);
			}
		}
		else if (!first_temp_set && raw_is_valid)
		{
			printlnA(F("Force temperature assignment - first valid reading"));
			measures.water_temperature = measures.water_temperature_raw;
			first_temp_set = true;
		}
		// Pressure value
		measures.pump_pressure = pump_filtration_get_pressure(false);
		measures.pressure_warning_active = pump_filtration_is_on() &&
			(parameters.pressure_warning > 0.0f) &&
			(measures.pump_pressure > parameters.pressure_warning);
#if HAS_QUIET_MEASURES
		if (quiet_measure)
		{
			measures.ph_raw = water_get_ph();
			measures.orp_raw = water_get_orp();
		}
#else
		measures.ph_raw = water_get_ph();
		measures.orp_raw = water_get_orp();
#endif
		measures.ph = measures.ph_raw + parameters.ph_offset;
		measures.orp = measures.orp_raw + parameters.orp_offset;
		measures.level_cl = level_cl_is_ok();
		measures.level_ph_minus = level_ph_minus_is_ok();
		measures.level_ph_plus = level_ph_plus_is_ok();
		measures.level_water = level_water_is_ok();
	}
	measures_to_json_string();
	disp_measures_to_display();
	{
		struct MeasuresStructure cmp_new = measures;
		struct MeasuresStructure cmp_old = last_published;
		cmp_new.index = 0;
		cmp_old.index = 0;
		bool changed = memcmp(&cmp_new, &cmp_old, sizeof(cmp_new)) != 0;
		bool heartbeat = (millis() - last_mqtt_ms) >= (MQTT_HEARTBEAT_S * 1000UL);
		if (changed || heartbeat)
		{
			last_published = measures;
			last_mqtt_ms = millis();
			mqtt_publish_measures();
		}
	}
	debug_pin1_off();
	led0_off();

#if HAS_QUIET_MEASURES
	if (quiet_measure)
	{
		pump_filtration_on();
	}
#endif
	return true;
}

bool update_graph(void *)
{
	return true;
}

void measures_to_json_string(void)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.createObject();
	json["index"] = measures.index;
	json["system_temperature"] = measures.system_temperature;
	json["system_humidity"] = measures.system_humidity;
	json["water_temperature"] = measures.water_temperature;
	json["water_temperature_raw"] = measures.water_temperature_raw;
	json["day_max_water_temperature"] = measures.day_max_water_temperature;
	json["pump_pressure"] = measures.pump_pressure;
	json["ph"] = measures.ph;
	json["orp"] = measures.orp;
	json["level_cl"] = measures.level_cl;
	json["level_ph_minus"] = measures.level_ph_minus;
	json["level_ph_plus"] = measures.level_ph_plus;
	json["level_water"] = measures.level_water;
	json["pressure_warning_active"] = measures.pressure_warning_active;
	json["daily_ml_orp"] = measures.daily_ml_orp;
	json["daily_ml_ph_minus"] = measures.daily_ml_ph_minus;
	json["daily_ml_ph_plus"] = measures.daily_ml_ph_plus;
	json["daily_filter_min"] = measures.daily_filter_min;
	json["total_filter_min"] = measures.total_filter_min;
	json["boot_count"] = measures.boot_count;
	json.printTo(measures_json_string, sizeof(measures_json_string));
}

bool measures_json_to_measures(char *json_str)
{
	DynamicJsonBuffer jsonBuffer;
	JsonObject &json = jsonBuffer.parseObject(json_str);
	if (json.success())
	{
		measures.system_temperature = json["system_temperature"];
		measures.system_humidity = json["system_humidity"];
		measures.water_temperature_raw = json["water_temperature_raw"];
		measures.water_temperature = json["water_temperature"];
		measures.day_max_water_temperature = json["day_max_water_temperature"];
		measures.pump_pressure = json["pump_pressure"];
		measures.ph = json["ph"];
		measures.orp = json["orp"];
		measures.level_cl = json["level_cl"];
		measures.level_ph_minus = json["level_ph_minus"];
		measures.level_ph_plus = json["level_ph_plus"];
		measures.level_water = json["level_water"];
		if (json.containsKey("pressure_warning_active")) measures.pressure_warning_active = json["pressure_warning_active"];
		measures.daily_ml_orp = json["daily_ml_orp"];
		measures.daily_ml_ph_minus = json["daily_ml_ph_minus"];
		measures.daily_ml_ph_plus = json["daily_ml_ph_plus"];
		if (json.containsKey("daily_filter_min")) measures.daily_filter_min = json["daily_filter_min"];
		if (json.containsKey("total_filter_min")) measures.total_filter_min = json["total_filter_min"];
		measures.boot_count = json["boot_count"];
		return true;
	}
	else
	{
		return false;
	}
}
