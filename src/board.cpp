#include "board.h"
#include <arduino.h>

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Nextion.h>

OneWire oneWire(PIN_DS18B20);
DallasTemperature ds18b20(&oneWire);
DHT dht(PIN_DHT, DHT22);



void board_init()
{
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
  nexInit();
}

void pump_all_off(void)
{
  digitalWrite(PIN_RELAY_PUMP_CL, false);
  digitalWrite(PIN_RELAY_PUMP_FILTER, false);
  digitalWrite(PIN_RELAY_PUMP_PH_M, false);
  digitalWrite(PIN_RELAY_PUMP_PH_P, false);
}