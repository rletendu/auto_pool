#include "measures.h"
#include "config.h"
#include <SerialDebug.h>
#include "board.h"

struct MeasuresStructure measures;

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

