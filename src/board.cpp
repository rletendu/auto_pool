#include "board.h"
//#include <arduino.h>

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Nextion.h>
#include <Adafruit_ADS1015.h> // https://github.com/adafruit/Adafruit_ADS1X15.git

#include <DS3231.h>

DS3231 Rtc;
OneWire oneWire(PIN_DS18B20);
DallasTemperature ds18b20(&oneWire);
DHT dht(PIN_DHT, DHT22);
Adafruit_ADS1115 ads;


void board_init()
{
  Wire.begin();
  
  pinMode(PIN_LED0, OUTPUT);
  pinMode(PIN_LED1, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  pinMode(PIN_RELAY_PUMP_CL, OUTPUT);
  pinMode(PIN_RELAY_PUMP_FILTER, OUTPUT);
  pinMode(PIN_RELAY_PUMP_PH_M, OUTPUT);
  pinMode(PIN_RELAY_PUMP_PH_P, OUTPUT);
  pump_all_off();

  pinMode(PIN_LEVEL_MIN_CL, INPUT);
  pinMode(PIN_LEVEL_MIN_PH_M, INPUT);
  pinMode(PIN_LEVEL_MIN_PH_P, INPUT);
  pinMode(PIN_LEVEL_MIN_WATER, INPUT);

  pinMode(PIN_PRESENCE, INPUT);
  pinMode(PIN_ADC_INTERRUPT, INPUT);
  pinMode(PIN_FLOW, INPUT);

  dht.begin();
  ds18b20.begin();
  ads.begin();
  nexInit();
}

