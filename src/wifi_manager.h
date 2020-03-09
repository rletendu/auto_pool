#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__


#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32

extern WiFiManager wifiManager;

void wifimanager_init(void);
bool is_should_save_config(void);
void wifimanager_autoconnect(void);
void wifimanager_start_portal(void);

#endif