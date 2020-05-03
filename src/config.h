
#ifndef __CONFIG_H__
#define __CONFIG_H__

#define AUTOPOOL_VER 0.5
#define PORTAL_NAME "AUTOPOOL_CONFIG"
#define MQTT_CLIENT_NAME "AUTOPOOL"
#define HOST_NAME "AUTOPOOL"
#define NTPSERVER "pool.ntp.org"
#define GMTOFFSET 3600
#define DAYLIGHTOFFSET 3600

#define PARAMETER_FILENAME "/config.json"
#define PARAMETERS_JSON_MESSAGE_LEN 512
#define MEASURES_JSON_MESSAGE_LEN 512
#define STATE_FILTER_CTRL_JSON_MESSAGE_LEN 300
#define STATE_ORP_CTRL_JSON_MESSAGE_LEN 300
#define STATE_PH_CTRL_JSON_MESSAGE_LEN 300

#define MEASURES_UPDATE_S 4
#define GRAPH_UPDATE_S 5
#define FILTER_CONTROL_UPDATE_S 10
#define ORP_CONTROL_UPDATE_S 10
#define ORP_REGULATION_CYCLE_S 10*60
#define PH_CONTROL_UPDATE_S 10
#define PH_REGULATION_CYCLE_S 10*60

#define BOOT_KEY_MAGIC 0x55AA1234

#define DEBUG_PIN1 PIN_EXT1

#define HAS_PH_PLUS_PUMP 0

#define DISPLAY_BAUD      115200
#define DISP_LOG_NB_LINES 12
#define DISP_LOG_NB_VISIBLE_LINES DISP_LOG_NB_LINES
#define DISP_LOG_NB_CHAR_PER_LINE 42

#define NB_BOOT_STEP_MSG 15
#define GRAPH_MAX_PTS 150

#define TOUCH_TIMEOUT_S 30



#endif