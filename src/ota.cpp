#include "board.h"
#include <ArduinoOTA.h>
#include <SerialDebug.h>
#include "display.h"

void ota_init(void)
{
	ArduinoOTA
	    .onStart([]() {
		    String type;
		    if (ArduinoOTA.getCommand() == U_FLASH)
			    type = "sketch";
		    else // U_SPIFFS
			    type = "filesystem";

		    // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
		    printlnA(F("Start updating "));
		    disp_page_ota();
		    pump_all_off();
		    buzzer_on();
		    delay(50);
		    buzzer_off();
		    delay(50);
		    buzzer_on();
		    delay(50);
		    buzzer_off();
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
	ArduinoOTA.handle();
}
