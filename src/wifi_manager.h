#include <WiFiManager.h>          //https://github.com/Brunez3BD/WIFIMANAGER-ESP32

extern WiFiManager wifiManager;

void setup_wifimanager(void);
bool is_should_save_config(void);
void wifimanager_autoconnect(void);
void wifimanager_start_portal(void);