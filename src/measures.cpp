#include "measures.h"
#include "config.h"
#include <SerialDebug.h>
#include "board.h"
#include <ArduinoJson.h> 

struct MeasuresStructure measures;
char measures_json_string[MEASURES_JSON_MESSAGE_LEN];

void update_measures(void)
{
    measures.system_temperature = dht_get_temperature();
    measures.system_humidity = dht_get_humidity();
    measures.water_temperature = water_get_temperature();
    measures.pump_pressure = pump_filtration_get_pressure();
    measures.ph = water_get_ph();
    measures.orp = water_get_orp();
    measures.level_cl = level_water_is_ok();
    measures.level_ph_minus = level_ph_minus_is_ok();
    measures.level_ph_plus = level_ph_plus_is_ok();
    measures.level_water = level_water_is_ok();
}

void measures_to_json_string(void)
{
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
    json["system_temperature"] = measures.system_temperature;
    json["mqtt_server"] = measures.system_humidity;
    json["mqtt_server"] = measures.water_temperature;
    json["mqtt_server"] = measures.pump_pressure;
    json["mqtt_server"] = measures.ph;
    json["mqtt_server"] = measures.orp;
    json["mqtt_server"] = measures.level_cl;
    json["mqtt_server"] = measures.level_ph_minus;
    json["mqtt_server"] = measures.level_ph_plus;
    json["mqtt_server"] = measures.level_water;
    json.printTo(measures_json_string);
}