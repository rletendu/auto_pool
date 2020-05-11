#ifndef __WIFI_MANAGER_H__
#define __WIFI_MANAGER_H__

#include <WiFiManager.h> //https://github.com/Brunez3BD/WIFIMANAGER-ESP32

extern WiFiManager wifiManager;

void wifimanager_init(void);
bool is_should_save_config(void);
bool wifimanager_autoconnect(void);
bool wifimanager_start_portal(void);
void wifimanager_reset_portal(void);
bool wifi_is_available(void);
void wifi_monitor(void);
#endif