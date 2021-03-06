#include "autopool.h"

void ota_init(void)
{
	ArduinoOTA
		.onStart([]() {
			String type;
			printlnA(F("Start updating "));
			stop_display_tasks();
			disp_page_ota();
			if (ArduinoOTA.getCommand() == U_FLASH)
			{
				type = "sketch";
				ota_title.setText("Firmware Update");
				printlnA(F("Updating Firmware"));
			}
			else // U_SPIFFS
			{
				type = "filesystem";
				ota_title.setText("FileSys. Update");
				printlnA(F("Updating FileSystem"));
			}
			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using
			SPIFFS.end();
			pump_all_off();
			beep(10);
			delay(10);
			beep(10);
		})
		.onEnd([]() {
			printlnA(F("End Update"));
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
			disp_ota_progress((progress / (total / 100)));
		})
		.onError([](ota_error_t error) {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR)
			{
				printlnA(F("Auth Failed"));
			}
			else if (error == OTA_BEGIN_ERROR)
			{
				printlnA(F("Begin Failed"));
			}
			else if (error == OTA_CONNECT_ERROR)
			{
				printlnA(F("Connect Failed"));
			}
			else if (error == OTA_RECEIVE_ERROR)
			{
				printlnA(F("Receive Failed"));
			}
			else if (error == OTA_END_ERROR)
			{
				printlnA(F("End Failed"));
			}
		});

	ArduinoOTA.begin();
}

void ota_loop(void)
{
	if (wifi_is_available())
	{
		ArduinoOTA.handle();
	}
}
