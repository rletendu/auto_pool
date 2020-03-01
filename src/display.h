
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#define ID_IMAGE_WIFI_OFF 0
#define ID_IMAGE_WIFI_ON 1
#define ID_IMAGE_GREEN 7
#define ID_IMAGE_RED 8
#define ID_IMAGE_ORANGE 9

#define PID_BOOT 0
#define PID_STATUS 1
#define PID_CONTROL 2
#define PID_GRAPH 3
#define PID_LOG 4
#define PID_OPTIONS 5
#define PID_OTA 6

#define CID_STATUS_WIFI 6
#define CID_STATUS_WATER_TEMP 10
#define CID_STATUS_PH 11
#define CID_STATUS_ORP 12
#define CID_STATUS_PRESSURE 13
#define CID_STATUS_LED_PH 20
#define CID_STATUS_LED_ORP 21
#define CID_STATUS_LED_PRESSURE 22
#define CID_STATUS_WATER_LEVEL 27
#define CID_STATUS_CL_LEVEL 28
#define CID_STATUS_PH_MINUS_LEVEL 29
#define CID_STATUS_SYSTEM_TEMP 33
#define CID_STATUS_SYSTEM_HUMIDITY 37


#define CID_OPTIONS_PH 7
#define CID_OPTIONS_DELTA_PH 15
#define CID_OPTIONS_ORP 8
#define CID_OPTIONS_DELTA_ORP 16

#define CID_OPTIONS_CL_FLOW 21
#define CID_OPTIONS_PH_MINUS_FLOW 5
#define CID_OPTIONS_PH_PLUS_FLOW 63
#define CID_OPTIONS_PRESSURE 68
#define CID_OPTIONS_MODE_TIMER_PROG 74
#define CID_OPTIONS_MODE_FCT_T 75

#define CID_OPTIONS_OK 28
#define CID_OPTIONS_CANCEL 29

#define CID_OPTIONS_0H 32
#define CID_OPTIONS_1H 33
#define CID_OPTIONS_2H 34

#define CID_STATUS_NEXT 4
#define CID_CONTROL_NEXT 2
#define CID_GRAPH_NEXT 6
#define CID_LOG_NEXT 3

#define CID_STATUS_PREV 3
#define CID_CONTROL_PREV 1
#define CID_GRAPH_PREV 5
#define CID_LOG_PREV 2

enum display_page_t
{
    PAGE_BOOT,
    PAGE_STATUS,
    PAGE_CONTROL,
    PAGE_GRAPH,
    PAGE_LOG,
    PAGE_OPTIONS,
    PAGE_OTA
};

void display_init(void);
void display_loop(void);

void disp_page_ota();
void disp_ota_progress(uint8_t progress);

#endif