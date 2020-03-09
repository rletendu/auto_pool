#include "measures.h"
#include "config.h"
#include <SerialDebug.h>
#include "board.h"
#include <ArduinoJson.h>
#include "soft_timer.h"
#include "display.h"

struct MeasuresStructure measures;
char measures_json_string[MEASURES_JSON_MESSAGE_LEN];
extern SoftTimer timer_pool;

uintptr_t update_measures_task;
uintptr_t update_graph_task;

void measures_to_json_string(void);

bool update_measures(void *)
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
    measures_to_json_string();
    disp_measures_to_display();
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
    json["system_temperature"] = measures.system_temperature;
    json["system_humidity"] = measures.system_humidity;
    json["water_temperature"] = measures.water_temperature;
    json["pump_pressure"] = measures.pump_pressure;
    json["ph"] = measures.ph;
    json["orp"] = measures.orp;
    json["level_cl"] = measures.level_cl;
    json["level_ph_minus"] = measures.level_ph_minus;
    json["level_ph_plus"] = measures.level_ph_plus;
    json["level_water"] = measures.level_water;
    json.printTo(measures_json_string);
}

void measures_init(void)
{
    update_measures_task = timer_pool.every(MEASURES_UPDATE_MS, update_measures);
    update_graph_task = timer_pool.every(GRAPH_UPDATE_MS, update_graph);
}
void measures_loop(void)
{
}
