
#ifndef __DISPLAY_COMPONENTS_H__
#define __DISPLAY_COMPONENTS_H__

#include "Nextion.h"

#define ID_IMAGE_WIFI_OFF 0
#define ID_IMAGE_WIFI_ON 1
#define ID_IMAGE_GREEN 7
#define ID_IMAGE_RED 8
#define ID_IMAGE_ORANGE 9

#define ID_IMAGE_TIME_PROG_BLACK 10
#define ID_IMAGE_TIME_PROG_WHITE 11
#define ID_IMAGE_TIME_PROG_RED 12

#define PID_BOOT 0
#define PID_STATUS 1
#define PID_CONTROL 2
#define PID_GRAPH 3
#define PID_LOG 4
#define PID_OPTIONS 5
#define PID_OTA 6
#define PID_SAVER 7
#define PID_OPTIONS2 8
#define PID_OPTIONS3 9
#define PID_CUSTOM 10
#define PID_WIFI 11


#define CID_STATUS_WIFI 6
#define CID_OPTIONS 7
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
#define CID_STATUS_SYSTEM_HOUR 18

#define CID_CONTROL_FILTER_AUTO 3
#define CID_CONTROL_FILTER_ON 4
#define CID_CONTROL_FILTER_OFF 5

#define CID_CONTROL_CL_AUTO 6
#define CID_CONTROL_CL_ON 7
#define CID_CONTROL_CL_OFF 8

#define CID_CONTROL_PH_MINUS_AUTO 9
#define CID_CONTROL_PH_MINUS_ON 10
#define CID_CONTROL_PH_MINUS_OFF 11

#define CID_CONTROL_PH_PLUS_AUTO 15
#define CID_CONTROL_PH_PLUS_ON 16
#define CID_CONTROL_PH_PLUS_OFF 17

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
#define CID_OPTIONS_3H 35
#define CID_OPTIONS_4H 36
#define CID_OPTIONS_5H 37
#define CID_OPTIONS_6H 38
#define CID_OPTIONS_7H 39
#define CID_OPTIONS_8H 40
#define CID_OPTIONS_9H 41
#define CID_OPTIONS_10H 42
#define CID_OPTIONS_11H 43
#define CID_OPTIONS_12H 44
#define CID_OPTIONS_13H 45
#define CID_OPTIONS_14H 46
#define CID_OPTIONS_15H 47
#define CID_OPTIONS_16H 48
#define CID_OPTIONS_17H 49
#define CID_OPTIONS_18H 50
#define CID_OPTIONS_19H 51
#define CID_OPTIONS_20H 52
#define CID_OPTIONS_21H 53
#define CID_OPTIONS_22H 54
#define CID_OPTIONS_23H 55

#define CID_STATUS_NEXT 9
#define CID_CONTROL_NEXT 2
#define CID_GRAPH_NEXT 6
#define CID_LOG_NEXT 3

#define CID_STATUS_PREV 8
#define CID_CONTROL_PREV 1
#define CID_GRAPH_PREV 5
#define CID_LOG_PREV 2

#define DISP_COLOR_RED 63488
#define DISP_COLOR_ORANGE 64512
#define DISP_COLOR_GREEN 1024
#define DISP_COLOR_GRAY 50712

#define DISP_COLOR_LIGHT_GREEN 2016


extern NexPage page_boot;
extern NexPage page_status;
extern NexPage page_control;
extern NexPage page_graph;
extern NexPage page_log;
extern NexPage page_options;
extern NexPage page_ota;
extern NexPage page_saver;
extern NexButton disp_saver_exit;
extern NexPage page_options_2;
extern NexPage page_options_3;
extern NexPage page_portal; 

extern NexText boot_title;
extern NexProgressBar boot_progress;
extern NexText boot_message;
extern NexText boot_version;
extern NexText boot_ip;

extern NexProgressBar ota_progress;
extern NexText ota_title;

extern NexXfloat disp_water_temperature;
extern NexXfloat disp_water_temperature_raw;
extern NexXfloat disp_water_temperature_max;
extern NexXfloat disp_ph;
extern NexXfloat disp_orp;
extern NexXfloat disp_pressure;
extern NexXfloat disp_sys_temperature;
extern NexXfloat disp_sys_humidity;
extern NexPicture disp_wifi;
extern NexText dis_sys_hour;
extern NexText dis_sys_user0;
extern NexXfloat disp_phm_day;
extern NexXfloat disp_cl_day;

extern NexPicture disp_led_ph;
extern NexPicture disp_led_orp;
extern NexPicture disp_led_pressure;

extern NexPicture disp_led_level_water;
extern NexPicture disp_led_level_cl;
extern NexPicture disp_led_level_ph_minus;

extern NexPicture disp_led_pump_water;
extern NexPicture disp_led_pump_cl;
extern NexPicture disp_led_pump_ph_minus;

extern NexPicture disp_timer_prog00;
extern NexPicture disp_timer_prog01;
extern NexPicture disp_timer_prog02;
extern NexPicture disp_timer_prog03;
extern NexPicture disp_timer_prog04;
extern NexPicture disp_timer_prog05;
extern NexPicture disp_timer_prog06;
extern NexPicture disp_timer_prog07;
extern NexPicture disp_timer_prog08;
extern NexPicture disp_timer_prog09;
extern NexPicture disp_timer_prog10;
extern NexPicture disp_timer_prog11;
extern NexPicture disp_timer_prog12;
extern NexPicture disp_timer_prog13;
extern NexPicture disp_timer_prog14;
extern NexPicture disp_timer_prog15;
extern NexPicture disp_timer_prog16;
extern NexPicture disp_timer_prog17;
extern NexPicture disp_timer_prog18;
extern NexPicture disp_timer_prog19;
extern NexPicture disp_timer_prog20;
extern NexPicture disp_timer_prog21;
extern NexPicture disp_timer_prog22;
extern NexPicture disp_timer_prog23;

extern NexProgressBar disp_progress_hour;
extern NexProgressBar disp_progress_filter;

extern NexPicture disp_options;

extern NexPicture disp_next_status;
extern NexPicture disp_prev_status;

extern NexPicture disp_next_control;
extern NexPicture disp_prev_control;

extern NexPicture disp_next_graph;
extern NexPicture disp_prev_graph;

extern NexPicture disp_next_log;
extern NexPicture disp_prev_log;

extern NexButton disp_options_ok;
extern NexButton disp_options_cancel;

extern NexXfloat disp_options_delta_ph;
extern NexXfloat disp_options_ph;
extern NexXfloat disp_options_orp;
extern NexXfloat disp_options_delta_orp;
extern NexXfloat disp_options_cl_flow;
extern NexXfloat disp_options_ph_minus_flow;
extern NexXfloat disp_options_ph_plus_flow;
extern NexXfloat disp_options_pressure_warning;

extern NexRadio disp_options_mode_timer_prog;
extern NexRadio disp_options_mode_fct_t;

extern NexCheckbox disp_otions_0h;
extern NexCheckbox disp_otions_1h;
extern NexCheckbox disp_otions_2h;
extern NexCheckbox disp_otions_3h;
extern NexCheckbox disp_otions_4h;
extern NexCheckbox disp_otions_5h;
extern NexCheckbox disp_otions_6h;
extern NexCheckbox disp_otions_7h;
extern NexCheckbox disp_otions_8h;
extern NexCheckbox disp_otions_9h;
extern NexCheckbox disp_otions_10h;
extern NexCheckbox disp_otions_11h;
extern NexCheckbox disp_otions_12h;
extern NexCheckbox disp_otions_13h;
extern NexCheckbox disp_otions_14h;
extern NexCheckbox disp_otions_15h;
extern NexCheckbox disp_otions_16h;
extern NexCheckbox disp_otions_17h;
extern NexCheckbox disp_otions_18h;
extern NexCheckbox disp_otions_19h;
extern NexCheckbox disp_otions_20h;
extern NexCheckbox disp_otions_21h;
extern NexCheckbox disp_otions_22h;
extern NexCheckbox disp_otions_23h;

extern NexXfloat disp_options_max_cl;
extern NexXfloat disp_options_max_phm;
extern NexXfloat disp_options_orp_offset;
extern NexXfloat disp_options_ph_offset;

extern NexXfloat disp_options_periodic_filter;

extern NexXfloat disp_options_orp_value;
extern NexXfloat disp_options_orp_raw_value;
extern NexXfloat disp_options_ph_value;
extern NexXfloat disp_options_ph_raw_value;
extern NexButton disp_options_rst_cl_day;
extern NexButton disp_options_rst_phm_day;
extern NexButton disp_options_reboot;
extern NexButton disp_options_default_param;
extern NexButton disp_options_portal;

extern NexWaveform disp_graph_temp;
extern NexWaveform disp_graph_orp;
extern NexWaveform disp_graph_ph;
extern NexWaveform disp_graph_press;

extern NexButton disp_control_filter_auto;
extern NexButton disp_control_filter_off;
extern NexButton disp_control_filter_on;

extern NexButton disp_control_cl_auto;
extern NexButton disp_control_cl_off;
extern NexButton disp_control_cl_on;

extern NexButton disp_control_ph_minus_auto;
extern NexButton disp_control_ph_minus_off;
extern NexButton disp_control_ph_minus_on;

extern NexButton disp_control_ph_plus_auto;
extern NexButton disp_control_ph_plus_off;
extern NexButton disp_control_ph_plus_on;

extern NexButton disp_control_pump_pwr_full;
extern NexButton disp_control_pump_pwr_reg;

extern NexButton disp_control_save_default;

extern NexText wifi_ip;
extern NexText wifi_mac;
extern NexText wifi_ssid;
extern NexText wifi_mqtt_server;
extern NexText wifi_mqtt_topic;
extern NexXfloat wifi_rssi;
extern NexPicture wifi_status;
extern NexButton wifi_portal;

extern NexText disp_options_ip;
extern NexText disp_options_mac; 
extern NexText disp_options_ssid;
extern NexText disp_options_mqtt_server;
extern NexText disp_optionmqtt_topic; 
extern NexXfloat disp_options_rssi; 
extern NexPicture disp_options_wifi_status; 
extern NexButton disp_options_portal; 
extern NexText disp_options_version;

extern NexText disp_portal_title;
extern NexProgressBar disp_portal_progress;
extern NexText disp_portal_message; 
extern NexText disp_portal_message2;
extern NexText disp_portal_message3;


extern NexText disp_log_logger;

#endif