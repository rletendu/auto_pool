
#ifndef __BOARD_H__
#define __BOARD_H__

#define PIN_LED0              2
#define PIN_LED1              12
						      
#define PIN_RELAY_PUMP_PH_P   4
#define PIN_BUZZER            5
						      
#define PIN_DS18B20           13 
#define PIN_EXT1              14
#define PIN_EXT2              15
						      
#define PIN_RX2  		      16
#define PIN_TX2  		      17

#define PIN_DISPLAY_RX        PIN_RX2
#define PIN_DISPLAY_TX        PIN_TX2
						      
#define PIN_DHT               18
						      
#define PIN_FLOW              19
#define PIN_SDA               21
#define PIN_SCL               22
						      
#define PIN_RTC_INTERRUPT     23
#define PIN_RELAY_PUMP_CL     25
						      
#define PIN_ADC_INTERRUPT     26

#define PIN_RELAY_PUMP_FILTER 27
#define PIN_RELAY_PUMP_PH_M   32

#define PIN_PRESENCE          33


#define PIN_LEVEL_MIN_CL      34
#define PIN_LEVEL_MIN_PH_M    35
#define PIN_LEVEL_MIN_PH_P    36
#define PIN_LEVEL_MIN_WATER   39


void pump_all_off(void);
void pump_ph_minus_on(void);
void pump_ph_minus_off(void);
void pump_ph_plus_on(void);
void pump_ph_plus_off(void);
void pump_ph_cl_on(void);
void pump_ph_cl_off(void);
void pump_filtration_on(void);
void pump_filtration_off(void);

#endif