#include "autopool.h"
#include <WiFiManager.h> //https://github.com/Brunez3BD/WIFIMANAGER-ESP32

WiFiManager wifiManager;

static bool shouldSaveConfig = false;
static char portal_ssid[40] = PORTAL_NAME;
WiFiManagerParameter custom_mqtt_server("server", "mqtt server", parameters.mqtt_server, 40);
WiFiManagerParameter custom_mqtt_port("port", "mqtt port", parameters.mqtt_port, 6);
WiFiManagerParameter custom_mqtt_user("user", "mqtt user", parameters.mqtt_user, 20);
WiFiManagerParameter custom_mqtt_pass("pass", "mqtt pass", parameters.mqtt_pass, 20);
WiFiManagerParameter custom_mqtt_base_topic("topic", "mqtt topic", parameters.mqtt_base_topic, 20);
WiFiManagerParameter custom_device_suffix("suffix", "device suffix (multi-device, optional)", parameters.device_suffix, PARAM_DEVICE_SUFFIX_MAX);
WiFiManagerParameter custom_text("</p>MQTT Server configuration");

static const char *portal_name(void)
{
	if (parameters.device_suffix[0] != 0 && device_suffix_is_valid(parameters.device_suffix))
	{
		snprintf(portal_ssid, sizeof(portal_ssid), "%s_%s", PORTAL_NAME, parameters.device_suffix);
	}
	else
	{
		strncpy(portal_ssid, PORTAL_NAME, sizeof(portal_ssid) - 1);
		portal_ssid[sizeof(portal_ssid) - 1] = 0;
	}
	return portal_ssid;
}

void saveConfigCallback()
{
	Serial.println("Should save config");
	shouldSaveConfig = true;
}

void enterApCallback(WiFiManager *wm)
{
	printlnA("Entered AP configCallback");
	char msg[80];
	uint8_t page;
	sprintf(msg, "Wifi Portal Started @192.168.4.1 on %s", portal_name());
	/*
	GetPageId(&page);
	if (page == PID_BOOT)
	{
		disp_boot_mesg(msg);
	}
	else*/
	{
		page_portal.show();
		disp_portal_message.setText(msg);
		disp_portal_progress.setValue(0);
	}
}

bool is_should_save_config(void)
{
	return shouldSaveConfig;
}
void wifimanager_progress_callback(WiFiManager *wm, uint32_t call_count)
{
	char buf[40];
	beep(10);
	int progress_timeout = (100 * PORTAL_CONFIG_PROGRESS_S * call_count) / PORTAL_CONFIG_TIMEOUT_S;
	disp_portal_progress.setValue((int)progress_timeout);
}

void wifimanager_init(void)
{
	printlnA(F("Setup Wifi manager..."));
	wifiManager.setDebugOutput(true);
	wifiManager.setSaveConfigCallback(saveConfigCallback);
	wifiManager.setAPCallback(enterApCallback);
	wifiManager.setConfigPortalTimeout(PORTAL_CONFIG_TIMEOUT_S);
	wifiManager.setProgressCallback(wifimanager_progress_callback, PORTAL_CONFIG_PROGRESS_S);
	wifiManager.addParameter(&custom_text);
	wifiManager.addParameter(&custom_mqtt_server);
	wifiManager.addParameter(&custom_mqtt_port);
	wifiManager.addParameter(&custom_mqtt_user);
	wifiManager.addParameter(&custom_mqtt_pass);
	wifiManager.addParameter(&custom_mqtt_base_topic);
	wifiManager.addParameter(&custom_device_suffix);
	if (parameters_read_file())
	{
		printlnA(F("Wifi manager got default parameters from config file"));
		custom_mqtt_server.setValue(parameters.mqtt_server);
		custom_mqtt_port.setValue(parameters.mqtt_port);
		custom_mqtt_user.setValue(parameters.mqtt_user);
		custom_mqtt_pass.setValue(parameters.mqtt_pass);
		custom_mqtt_base_topic.setValue(parameters.mqtt_base_topic);
		custom_device_suffix.setValue(parameters.device_suffix);
	}
	else
	{
		printlnA(F("Wifi manager failed to get default parameters from config file"))
	}
	printlnA(F("Wifi manager Init done"))
}

void reset_wifimanager(void)
{
	wifiManager.resetSettings();
}

bool wifimanager_autoconnect(void)
{
	bool ret;
	ret = wifiManager.autoConnect(portal_name());
	if (ret)
	{
		printlnA(F("Wifi Connected !"));
	}
	else
	{
		printlnA(F("Wifi Not Connected !"));
	}

	if (is_should_save_config())
	{
		printlnA(F("Saveconfig Detected !"));
		//read updated parameters
		strcpy(parameters.mqtt_server, custom_mqtt_server.getValue());
		strcpy(parameters.mqtt_port, custom_mqtt_port.getValue());
		strcpy(parameters.mqtt_user, custom_mqtt_user.getValue());
		strcpy(parameters.mqtt_pass, custom_mqtt_pass.getValue());
		strcpy(parameters.mqtt_base_topic, custom_mqtt_base_topic.getValue());
		device_suffix_sanitize(parameters.device_suffix, custom_device_suffix.getValue(), PARAM_DEVICE_SUFFIX_LEN);
		parameters_write_file();
	}
	return ret;
}

bool wifimanager_start_portal(void)
{
	bool ret;
	printlnA(F("Portal request"));
	ret = wifiManager.startConfigPortal(portal_name());
	strcpy(parameters.mqtt_server, custom_mqtt_server.getValue());
	strcpy(parameters.mqtt_port, custom_mqtt_port.getValue());
	strcpy(parameters.mqtt_user, custom_mqtt_user.getValue());
	strcpy(parameters.mqtt_pass, custom_mqtt_pass.getValue());
	strcpy(parameters.mqtt_base_topic, custom_mqtt_base_topic.getValue());
	if (is_should_save_config())
	{
		printlnA(F("Need to write Json config file..."));
		parameters_write_file();
	}
	return ret;
}

void wifimanager_reset_portal(void)
{
	printlnA(F("Portal reset"));
	wifiManager.resetSettings();
}

bool wifi_is_available(void)
{
	if (WiFi.status() == WL_CONNECTED)
	{
		return true;
	}
	else
	{
		return false;
	}
}

void wifi_monitor(void)
{
	int dbm;
	int quality;
	if (wifi_is_available())
	{
		dbm = WiFi.RSSI();
		// dBm to Quality:
		if (dbm <= -100)
		{
			quality = 0;
		}
		else if (dbm >= -50)
		{
			quality = 100;
		}
		else
		{
			quality = 2 * (dbm + 100);
			quality = map(quality, 0, 100, 0, 12);
		}
	}
	else
	{
		quality = 0;
	}
	disp_options_rssi.setValue(quality);
	disp_wifi_status(wifi_is_available());
}