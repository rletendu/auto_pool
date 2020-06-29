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
		printlnA(F("Failed to sync internet time"));
		return;
	}
	timeinfo.tm_year -= 100;
	timeinfo.tm_mon += 1;
	rtc_set_time(timeinfo);
	Serial.println(&timeinfo, "%A, %B %d %Y %H:%M:%S");
}

void setup()
{
	char msg[80];
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

	printlnA(F("AutoPool Starting..."));
	sprintf(msg, "AutoPool %.2f %s %s starting", AUTOPOOL_VER, __DATE__, __TIME__);
	disp_boot_progress_message(msg);
	sprintf(msg, "Rev: %.2f : %s %s", AUTOPOOL_VER, __DATE__, __TIME__);
	boot_version.setText(msg);
	disp_options_version.setText(msg);
	delay(500);

	disp_options_mac.setText(WiFi.macAddress().c_str());
	wifimanager_init();
	if (wifimanager_autoconnect())
	{
		IPAddress ip = WiFi.localIP();
		printA(F("Ip Address : "));
		printlnA(ip);
		sprintf(msg, "Wifi Connected : %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		disp_boot_progress_message(msg);
		sprintf(msg, "Wifi IP : %d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		boot_ip.setText(msg);
		sprintf(msg, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
		disp_options_ip.setText(msg);
	}
	else
	{
		page_boot.show();
		printlnA("Wifi Not Connected!:");
		disp_boot_progress_message("Wifi Not Connected !");
		boot_ip.setText("Wifi IP : ---:---:---:---");
	}
	wifi_monitor();

	if (parameters_read_file())
	{
		disp_boot_progress_message("Loading config file : Ok");
		printlnA("Reading config file OK");
	}
	else
	{
		disp_boot_progress_message("Loading config File Fail");
		printlnA("Reading config file failed");
	}

	disp_boot_progress_message("OTA Init");
	ota_init();

#if HAS_MQTT
	disp_boot_progress_message("MQTT Init");
	mqtt_init();
#endif

	disp_boot_progress_message("CLI Init");
	cli_init();

	disp_boot_progress_message("RTC Init");
	configTime(GMTOFFSET, DAYLIGHTOFFSET, NTPSERVER);
	rtc_init();

	if (state_default_read_file()) {
		printlnA(F("State.json file read sucessfully"));
		printA("Read filter mode : ");
		printlnA(state_default.filter_mode);
	} else {
		printlnA(F("Could not read State.json file, applying default"));
		state_default.filter_mode = FILTER_AUTO;
		state_default.ph_minus_mode = PH_MINUS_AUTO;
		state_default.ph_plus_mode = PH_PLUS_OFF;
		state_default.orp_mode = ORP_AUTO;
		state_default.filter_power = FILTER_POWER_FULL;
	}

	disp_boot_progress_message("Filter Control Init");
	filter_control_init();

	disp_boot_progress_message("ORP Control Init");
	orp_control_init();

	disp_boot_progress_message("pH Control Init");
	ph_control_init();

	disp_boot_progress_message("Measure Init");
	measures_init();

#if HAS_WEB_SERVER
	disp_boot_progress_message("Web Server Init");
	webserver_init();
#endif

#if HAS_TELNET_SERVER
	disp_boot_progress_message("Telnet Server Init");
	telnet_init();
#endif

	//save the custom parameters to FS
	if (is_should_save_config())
	{
		printlnA(F("Need to write Json config file..."));
		parameters_write_file();
	}

	time_update_task = timer_pool.every(60 * 1000, time_update);
	printlnA(F("Init Done..."));

	boot_key = BOOT_KEY_MAGIC;
	char boot_msg[25];
	sprintf(boot_msg, "Autopool rebooted (%u)", measures.boot_count);
	log_append(boot_msg);
	time_update(NULL);
	page_status.show();
}

volatile unsigned long tmp = 0;
volatile unsigned long target = 0x5000;
unsigned long duration;

void loop()
{
	timer_pool.tick(); // tick the timer

	ota_loop();
	display_loop(); // Proceed display touch events
#if HAS_MQTT
	mqtt_loop();
#endif
	cli_loop(); // Serial command line input
#if HAS_WEB_SERVER
	webserver_loop();
#endif
#if HAS_TELNET_SERVER
	telnet_loop();
#endif
}