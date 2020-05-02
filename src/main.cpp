#include "autopool.h"


SoftTimer timer_pool = SoftTimer();
uintptr_t time_update_task;

int bootCount RTC_NOINIT_ATTR;
RTC_NOINIT_ATTR float daily_ml_ph_minus_backup;
RTC_NOINIT_ATTR float daily_ml_ph_plus_backup;
RTC_NOINIT_ATTR float daily_ml_orp_backup;
uint32_t boot_key RTC_NOINIT_ATTR;

void time_update_stop(void)
{
	timer_pool.cancel(time_update_task);
}



bool time_update(void *)
{
	char msg[20];
	sprintf(msg, "%02u/%02u/%02u %02u:%02u", rtc_get_day(), rtc_get_month(), rtc_get_year(), rtc_get_hour(), rtc_get_minute());
	dis_sys_hour.setText(msg);
	if (rtc_get_hour() == 0 && rtc_get_minute() == 0)
	{
		measures.daily_ml_ph_minus = 0;
		measures.daily_ml_ph_plus = 0;
		measures.daily_ml_orp = 0;
		mqtt_publish_log("Reset injecion counters");
	}
	// Backup injection counters in RTC memory
	daily_ml_ph_minus_backup = measures.daily_ml_ph_minus;
	daily_ml_ph_plus_backup = measures.daily_ml_ph_plus;
	daily_ml_orp_backup = measures.daily_ml_orp;
	return true; // repeat? true
}

void rtc_init()
{
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo))
	{
		printlnA(F("Failed to obtain time"));
		return;
	}
	timeinfo.tm_year -= 100;
	timeinfo.tm_mon += 1;
	rtc_set_time(timeinfo);
	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
	if (boot_key == BOOT_KEY_MAGIC)
	{
		bootCount++;
	}
	else
	{
		bootCount = 0;
	}

	Serial.begin(115200);
	//parameters_format();
	board_init();
	display_log_clear();
	delay(500);
	printlnA(F("AutoPool Starting..."));
	wifimanager_init();
	wifimanager_autoconnect();
	if (parameters_read_file()) {
		printlnA("Reading config file OK");
	} else {
		printlnA("Reading config file failed");
	}
	printlnA(F("Ip Address : "));
	printlnA(WiFi.localIP());
	ota_init();
	mqtt_init();
	cli_init();
	configTime(GMTOFFSET, DAYLIGHTOFFSET, NTPSERVER);
	rtc_init();
	filter_control_init();
	orp_control_init();
	ph_control_init();
	measures_init();
	webserver_init();
	telnet_init();

	//save the custom parameters to FS
	if (is_should_save_config())
	{
		printlnA(F("Need to write Json config file..."));
		parameters_write_file();
	}

	time_update_task = timer_pool.every(60 * 1000, time_update);
	printlnA(F("Init Done..."));
	//NexSleep();
	boot_key = BOOT_KEY_MAGIC;
	char boot_msg[25];
	sprintf(boot_msg, "Autopool rebooted (%u)", measures.boot_count);
	log_append(boot_msg);
}

volatile unsigned long tmp = 0;
volatile unsigned long target = 0x5000;
unsigned long duration;

void loop()
{
	timer_pool.tick(); // tick the timer

	ota_loop();
	display_loop(); // Proceed display touch events
	mqtt_loop();
	cli_loop(); // Serial command line input
	webserver_loop();
	telnet_loop();
}