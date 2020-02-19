#include "board.h"
//#include <arduino.h>
#include "display.h"

#include <DHT.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <Nextion.h>
#include <Adafruit_ADS1015.h> // https://github.com/adafruit/Adafruit_ADS1X15.git

#include <DS3231.h>
#include <SerialDebug.h>

DS3231 rtc;
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

  buzzer_on();
  delay(20);
  buzzer_off();

  dht.begin();
  ds18b20.begin();
  ads.begin();
  display_init();
  

  printA(F("DHT Temperature : "));
  printlnA(dht_get_temperature());
  printA(F("DHT Humidity : "));
  printlnA(dht_get_humidity());
  printA(F("RTC Temperature : "));
  printlnA(rtc_get_temperature());
  printA(F("Water Temperature : "));
  printlnA(ds18_get_temperature(0));
}

float dht_get_temperature(void)
{
  float t = dht.readTemperature();
  if (t == NAN || t > 100 || t < -100)
  {
    return 255;
  }
  else
  {
    return t;
  }
}

float dht_get_humidity(void)
{
  float t = dht.readHumidity();
  if (t == NAN || t > 100 || t < 0)
  {
    return 255;
  }
  else
  {
    return t;
  }
}

float rtc_get_temperature(void)
{
  float t = rtc.getTemperature();
  if (t == NAN || t > 100 || t < -100)
  {
    return 255;
  }
  else
  {
    return t;
  }
}

uint8_t ds18_count(void)
{
  return ds18b20.getDeviceCount();
}


float ds18_get_temperature(uint8_t index)
{
  float t;
  ds18b20.requestTemperatures();
  t = ds18b20.getTempCByIndex(index);
  if (t == NAN || t > 100 || t < -100)
  {
    return 255;
  }
  else
  {
    return t;
  }
}

float water_get_temperature(void)
{
  return ds18_get_temperature(0);
}

float water_get_ph(void)
{
  int16_t adc;
  adc = ads.readADC_SingleEnded(ADS_CH_PH);

}

float water_get_redox(void)
{
  int16_t adc;
  adc = ads.readADC_SingleEnded(ADS_CH_CL);
}

float pump_filtration_get_pressure(void)
{
  int16_t adc;
  adc = ads.readADC_SingleEnded(ADS_CH_PRESSURE);
}
