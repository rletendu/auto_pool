#include "config.h"
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
	uint8_t current_page = 0;
	Wire.begin();
#ifdef DEBUG_PIN1
	pinMode(DEBUG_PIN1, OUTPUT);
#endif

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

	dht.begin(60);
	ds18b20.begin();
	ads.begin();
	ads.setGain(GAIN_ONE); // 1x gain   +/- 4.096V  1 bit =  0.125mV
	display_init();

	printA(F("DHT Temperature : "));
	printlnA(dht_get_temperature());
	printA(F("DHT Humidity : "));
	printlnA(dht_get_humidity());
	printA(F("RTC Temperature : "));
	printlnA(rtc_get_temperature());
	printA(F("Water Temperature : "));
	printlnA(ds18_get_temperature(0));
	printA(F("Pump Pressure : "));
	printlnA(pump_filtration_get_pressure());
	/*
  while(current_page!=2) {
    GetPageId(&current_page);
  }
  */
}

float dht_get_temperature(void)
{
	int retry = 0;
read_temp:
	float t = dht.readTemperature();
	if (isnan(t))
	{
		delay(200);
		if (++retry > 3)
		{
			return 255;
		}
		goto read_temp;
	}
	if ((t == NAN) || (t > 100) || (t < 0))
	{
		goto read_temp;
	}
	else
	{
		return t;
	}
}

float dht_get_humidity(void)
{
	int retry = 0;
read_hum:
	float t = dht.readHumidity();
	if (isnan(t))
	{
		delay(200);
		if (++retry > 3)
		{
			return 255;
		}

		goto read_hum;
	}
	if ((t == NAN) || (t > 100) || (t < 0))
	{
		goto read_hum;
	}
	else
	{
		return t;
	}
}

float rtc_get_temperature(void)
{

	float t = rtc.getTemperature();
	if ((t == NAN) || (t > 100) || (t < 0))
	{
		return 255;
	}
	else
	{
		return t;
	}
}

bool rtc_set_time(struct tm time)
{
	rtc.setHour(time.tm_hour);
	rtc.setMinute(time.tm_min);
	rtc.setSecond(time.tm_sec);
	rtc.setYear(time.tm_year);
	rtc.setMonth(time.tm_mon);
	rtc.setDate(time.tm_mday);
	return true;
}

uint8_t rtc_get_hour(void)
{
	bool h12, PM;
	return rtc.getHour(h12, PM);
}

uint8_t rtc_get_second(void)
{
	return rtc.getSecond();
}

uint8_t rtc_get_minute(void)
{
	return rtc.getMinute();
}

uint16_t rtc_get_year(void)
{
	return rtc.getYear();
}

uint8_t rtc_get_month(void)
{
	bool century = false;
	return rtc.getMonth(century);
}

uint8_t rtc_get_day(void)
{
	return rtc.getDate();
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
	double vout = (adc * 0.125) / 1000;
	double ph = 3.56 * vout - 1.889;
	if (ph < 0)
	{
		ph = 0;
	}
	return (float)ph;
}

float water_get_orp(void)
{
	int16_t adc;
	adc = ads.readADC_SingleEnded(ADS_CH_CL);
	double vout = (adc * 0.125) / 1000;
	double orp = 1000 * ((2.5 - vout) / 1.037);
	return (float)orp;
}

float pump_filtration_get_pressure(void)
{
	int16_t adc;
	adc = ads.readADC_SingleEnded(ADS_CH_PRESSURE);
	// Gain x1 1 bit =  0.125mV
	double vout = (adc * 0.125) / 1000;
	if (vout < .500)
	{
		vout = .500;
	}
	double p = 10 * ((vout - .500) / (0.6667 * 5));
	return (float)p;
}
