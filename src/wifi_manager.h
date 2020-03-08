#ifndef __PH_CONTROL_H__
#define __PH_CONTROL_H__

#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32

extern WiFiManager wifiManager;

void wifimanager_init(void);
bool is_should_save_config(void);
void wifimanager_autoconnect(void);
void wifimanager_start_portal(void);

#endif