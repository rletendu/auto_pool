
#ifndef __BOARD_H__
#define __BOARD_H__

#include <Arduino.h>
#include "config.h"

#define PIN_LED0 2
#define PIN_LED1 12

#define PIN_RELAY_PUMP_PH_P 4
#define PIN_BUZZER 5

#define PIN_DS18B20 13
#define PIN_EXT1 14
#define PIN_EXT2 15

#define PIN_RX2 16
#define PIN_TX2 17

#define PIN_DISPLAY_RX PIN_RX2
#define PIN_DISPLAY_TX PIN_TX2

#define PIN_DHT 18

#define PIN_FLOW 19
#define PIN_SDA 21
#define PIN_SCL 22

#define PIN_RTC_INTERRUPT 23
#define PIN_RELAY_PUMP_CL 25

#define PIN_ADC_INTERRUPT 26

#define PIN_RELAY_PUMP_FILTER 27
#define PIN_RELAY_PUMP_PH_M 32

#define PIN_PRESENCE 33

#define PIN_LEVEL_MIN_CL 34
#define PIN_LEVEL_MIN_PH_M 35
#define PIN_LEVEL_MIN_PH_P 36
#define PIN_LEVEL_MIN_WATER 39

#define ADS_CH_PH 0
#define ADS_CH_CL 1
#define ADS_CH_PRESSURE 2
#define ADS_CH_EXTRA 3

void board_init(void);

void pump_all_off(void);
void pump_ph_minus_on(void);
void pump_ph_minus_off(void);
void pump_ph_plus_on(void);
void pump_ph_plus_off(void);
void pump_ph_cl_on(void);
void pump_ph_cl_off(void);
void pump_filtration_on(void);
void pump_filtration_off(void);

#define PUMP_ACTIVE_VALUE false
#define PUMP_INACTIVE_VALUE ~PUMP_ACTIVE_VALUE

#define LEVEL_WATER_OK_VALUE false
#define LEVEL_CL_OK_VALUE false
#define LEVEL_PH_MINUS_OK_VALUE false
#define LEVEL_PH_PLUS_OK_VALUE false

inline void led0_off(void)
{
    digitalWrite(PIN_LED0, false);
}
inline void led0_on(void) { digitalWrite(PIN_LED0, true); }
inline void led0_toggle(void) { digitalWrite(PIN_LED0, !digitalRead(PIN_LED0)); }
inline void led1_off(void) { digitalWrite(PIN_LED1, false); }
inline void led1_on(void) { digitalWrite(PIN_LED1, true); }
inline void led1_toggle(void) { digitalWrite(PIN_LED1, !digitalRead(PIN_LED1)); }

inline void pump_filtration_on(void) { digitalWrite(PIN_RELAY_PUMP_FILTER, PUMP_ACTIVE_VALUE); }
inline void pump_filtration_off(void) { digitalWrite(PIN_RELAY_PUMP_FILTER, PUMP_INACTIVE_VALUE); }
inline bool pump_filtration_is_on(void) { return digitalRead(PIN_RELAY_PUMP_FILTER) == PUMP_ACTIVE_VALUE ? true : false; }

inline void pump_ph_minus_on(void) { digitalWrite(PIN_RELAY_PUMP_PH_M, PUMP_ACTIVE_VALUE); }
inline void pump_ph_minus_off(void) { digitalWrite(PIN_RELAY_PUMP_PH_M, PUMP_INACTIVE_VALUE); }
inline bool pump_ph_minus_is_on(void) { return digitalRead(PIN_RELAY_PUMP_PH_M) == PUMP_ACTIVE_VALUE ? true : false; }

inline void pump_ph_plus_on(void) { digitalWrite(PIN_RELAY_PUMP_PH_P, PUMP_ACTIVE_VALUE); }
inline void pump_ph_plus_off(void) { digitalWrite(PIN_RELAY_PUMP_PH_P, PUMP_INACTIVE_VALUE); }
inline bool pump_ph_plus_is_on(void) { return digitalRead(PIN_RELAY_PUMP_PH_P) == PUMP_ACTIVE_VALUE ? true : false; }

inline void pump_cl_on(void) { digitalWrite(PIN_RELAY_PUMP_CL, PUMP_ACTIVE_VALUE); }
inline void pump_cl_off(void) { digitalWrite(PIN_RELAY_PUMP_CL, PUMP_INACTIVE_VALUE); }
inline bool pump_cl_is_on(void) { return digitalRead(PIN_RELAY_PUMP_CL) == PUMP_ACTIVE_VALUE ? true : false; }



inline void debug_pin1_off(void) { digitalWrite(DEBUG_PIN1, false); }
inline void debug_pin1_on(void) { digitalWrite(DEBUG_PIN1, true); }
inline void debug_pin1_toggle(void) { digitalWrite(DEBUG_PIN1, !digitalRead(DEBUG_PIN1)); }

inline void pump_all_off(void)
{
    pump_filtration_off();
    pump_ph_minus_off();
    pump_ph_plus_off();
    pump_cl_off();
}

inline void buzzer_on(void) { digitalWrite(PIN_BUZZER, true); }
inline void buzzer_off(void) { digitalWrite(PIN_BUZZER, false); }

inline bool level_water_is_ok(void) { return digitalRead(PIN_LEVEL_MIN_WATER) == LEVEL_WATER_OK_VALUE ? true : false; }
inline bool level_cl_is_ok(void) { return digitalRead(PIN_LEVEL_MIN_CL) == LEVEL_CL_OK_VALUE ? true : false; }
inline bool level_ph_minus_is_ok(void) { return digitalRead(PIN_LEVEL_MIN_PH_M) == LEVEL_PH_MINUS_OK_VALUE ? true : false; }
inline bool level_ph_plus_is_ok(void) { return digitalRead(PIN_LEVEL_MIN_PH_P) == LEVEL_PH_PLUS_OK_VALUE ? true : false; }

void pump_filtration_off(void);

float dht_get_temperature(void);
float dht_get_humidity(void);
float rtc_get_temperature(void);
uint8_t ds18_count(void);
float ds18_get_temperature(uint8_t index);
float water_get_temperature(void);
float water_get_ph(void);
float water_get_orp(void);
float pump_filtration_get_pressure(void);

bool rtc_set_time(struct tm time);

uint8_t rtc_get_hour(void);
uint8_t rtc_get_minute(void);
uint8_t rtc_get_second(void);
uint16_t rtc_get_year(void);
uint8_t rtc_get_month(void);
uint8_t rtc_get_day(void);

#endif