
#include <Nextion.h>
#include "display_components.h"

NexPage page_boot = NexPage(PID_BOOT, 0, "boot");
NexPage page_status = NexPage(PID_STATUS, 0, "status");
NexPage page_control = NexPage(PID_CONTROL, 0, "control");
NexPage page_graph = NexPage(PID_GRAPH, 0, "graph");
NexPage page_log = NexPage(PID_LOG, 0, "log");
NexPage page_options = NexPage(PID_OPTIONS, 0, "options");
NexPage page_ota = NexPage(PID_OTA, 0, "OTA");
NexPage page_saver = NexPage(PID_SAVER, 0, "saver");
NexPage page_options_2 = NexPage(PID_OPTIONS2, 0, "options_2");
NexPage page_options_3 = NexPage(PID_OPTIONS3, 0, "options_3");
NexPage page_portal = NexPage(PID_CUSTOM, 0, "portal");


NexText boot_title = NexText(PID_BOOT, 1, "boot.boot_title");
NexProgressBar boot_progress = NexProgressBar(PID_BOOT, 3, "boot.boot_progress");
NexText boot_message = NexText(PID_BOOT, 4, "boot.boot_text");
NexText boot_version = NexText(PID_BOOT, 5, "boot.ver");
NexText boot_ip = NexText(PID_BOOT, 6, "boot.ip");

NexButton disp_saver_exit = NexButton(PID_SAVER, 1, "saver.saver_exit");

NexProgressBar ota_progress = NexProgressBar(PID_OTA, 1, "j0");
NexText ota_title = NexText(PID_OTA, 2, "ota_text");

NexXfloat disp_water_temperature = NexXfloat(PID_STATUS, CID_STATUS_WATER_TEMP, "status.temp_value");
NexXfloat disp_ph = NexXfloat(PID_STATUS, CID_STATUS_PH, "status.ph_value");
NexXfloat disp_orp = NexXfloat(PID_STATUS, CID_STATUS_ORP, "status.orp_value");
NexXfloat disp_pressure = NexXfloat(PID_STATUS, CID_STATUS_PRESSURE, "status.press_value");
NexXfloat disp_sys_temperature = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_TEMP, "status.sys_temp_value");
NexXfloat disp_sys_humidity = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_HUMIDITY, "status.sys_hum_value");
NexPicture disp_wifi = NexPicture(PID_STATUS, CID_STATUS_WIFI, "status.wifi_value");
NexText dis_sys_hour = NexText(PID_STATUS, CID_STATUS_SYSTEM_HOUR, "status.sys_hour");
NexText dis_sys_user0 = NexText(PID_STATUS, 51, "status.sys_user0");

NexXfloat disp_phm_day = NexXfloat(PID_STATUS, 39, "status.phm_day");
NexXfloat disp_cl_day = NexXfloat(PID_STATUS, 40, "status.cl_day");

NexPicture disp_led_ph = NexPicture(PID_STATUS, CID_STATUS_LED_PH, "status.ph_led");
NexPicture disp_led_orp = NexPicture(PID_STATUS, CID_STATUS_LED_ORP, "status.orp_led");
NexPicture disp_led_pressure = NexPicture(PID_STATUS, CID_STATUS_LED_PRESSURE, "status.press_led");

NexPicture disp_led_level_water = NexPicture(PID_STATUS, CID_STATUS_WATER_LEVEL, "status.water_lev");
NexPicture disp_led_level_cl = NexPicture(PID_STATUS, CID_STATUS_CL_LEVEL, "status.cl_level");
NexPicture disp_led_level_ph_minus = NexPicture(PID_STATUS, CID_STATUS_PH_MINUS_LEVEL, "status.phm_level");

NexPicture disp_led_pump_water = NexPicture(PID_STATUS, 46, "status.filter_pump");
NexPicture disp_led_pump_cl = NexPicture(PID_STATUS, 47, "status.cl_pump");
NexPicture disp_led_pump_ph_minus = NexPicture(PID_STATUS, 48, "status.phm_pump");

NexPicture disp_options = NexPicture(PID_STATUS, CID_OPTIONS, "p1");

NexPicture disp_next_status = NexPicture(PID_STATUS, CID_STATUS_NEXT, "p3");
NexPicture disp_prev_status = NexPicture(PID_STATUS, CID_STATUS_PREV, "p2");

NexPicture disp_next_control = NexPicture(PID_CONTROL, CID_CONTROL_NEXT, "p3");
NexPicture disp_prev_control = NexPicture(PID_CONTROL, CID_CONTROL_PREV, "p2");

NexPicture disp_next_graph = NexPicture(PID_GRAPH, CID_GRAPH_NEXT, "p3");
NexPicture disp_prev_graph = NexPicture(PID_GRAPH, CID_GRAPH_PREV, "p2");

NexPicture disp_next_log = NexPicture(PID_LOG, CID_LOG_NEXT, "p3");
NexPicture disp_prev_log = NexPicture(PID_LOG, CID_LOG_PREV, "p2");

NexButton disp_options_ok = NexButton(PID_OPTIONS, CID_OPTIONS_OK, "b12");
NexButton disp_options_cancel = NexButton(PID_OPTIONS, CID_OPTIONS_CANCEL, "b13");

NexXfloat disp_options_delta_ph = NexXfloat(PID_OPTIONS, CID_OPTIONS_DELTA_PH, "options.x0");
NexXfloat disp_options_ph = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH, "options.x1");
NexXfloat disp_options_orp = NexXfloat(PID_OPTIONS, CID_OPTIONS_ORP, "options.x2");
NexXfloat disp_options_delta_orp = NexXfloat(PID_OPTIONS, CID_OPTIONS_ORP, "options.x3");
NexXfloat disp_options_cl_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_CL_FLOW, "options.x4");
NexXfloat disp_options_ph_minus_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH_MINUS_FLOW, "options.x5");
NexXfloat disp_options_ph_plus_flow = NexXfloat(PID_OPTIONS, CID_OPTIONS_PH_PLUS_FLOW, "options.x6");
NexXfloat disp_options_pressure_warning = NexXfloat(PID_OPTIONS, CID_OPTIONS_PRESSURE, "options.x7");

NexRadio disp_options_mode_timer_prog = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_TIMER_PROG, "options.r0");
NexRadio disp_options_mode_fct_t = NexRadio(PID_OPTIONS, CID_OPTIONS_MODE_FCT_T, "options.r1");

NexCheckbox disp_otions_0h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_0H, "options.c0");
NexCheckbox disp_otions_1h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_1H, "options.c1");
NexCheckbox disp_otions_2h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_2H, "options.c2");
NexCheckbox disp_otions_3h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_3H, "options.c3");
NexCheckbox disp_otions_4h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_4H, "options.c4");
NexCheckbox disp_otions_5h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_5H, "options.c5");
NexCheckbox disp_otions_6h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_6H, "options.c6");
NexCheckbox disp_otions_7h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_7H, "options.c7");
NexCheckbox disp_otions_8h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_8H, "options.c8");
NexCheckbox disp_otions_9h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_9H, "options.c9");
NexCheckbox disp_otions_10h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_10H, "options.c10");
NexCheckbox disp_otions_11h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_11H, "options.c11");
NexCheckbox disp_otions_12h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_12H, "options.c12");
NexCheckbox disp_otions_13h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_13H, "options.c13");
NexCheckbox disp_otions_14h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_14H, "options.c14");
NexCheckbox disp_otions_15h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_15H, "options.c15");
NexCheckbox disp_otions_16h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_16H, "options.c16");
NexCheckbox disp_otions_17h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_17H, "options.c17");
NexCheckbox disp_otions_18h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_18H, "options.c18");
NexCheckbox disp_otions_19h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_19H, "options.c19");
NexCheckbox disp_otions_20h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_20H, "options.c20");
NexCheckbox disp_otions_21h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_21H, "options.c21");
NexCheckbox disp_otions_22h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_22H, "options.c22");
NexCheckbox disp_otions_23h = NexCheckbox(PID_OPTIONS, CID_OPTIONS_23H, "options.c23");

NexXfloat disp_options_max_cl = NexXfloat(PID_OPTIONS2, 5, "options_2.max_cl");
NexXfloat disp_options_max_phm = NexXfloat(PID_OPTIONS2, 4, "options_2.max_phm");
NexXfloat disp_options_orp_offset = NexXfloat(PID_OPTIONS2, 25, "options_2.orp_offset");
NexXfloat disp_options_ph_offset = NexXfloat(PID_OPTIONS2, 26, "options_2.ph_offset");

NexXfloat disp_options_orp_value = NexXfloat(PID_OPTIONS2, 27, "options_2.orp_value");
NexXfloat disp_options_orp_raw_value = NexXfloat(PID_OPTIONS2, 17, "options_2.orp_raw_value");
NexXfloat disp_options_ph_value = NexXfloat(PID_OPTIONS2, 29, "options_2.ph_value");
NexXfloat disp_options_ph_raw_value = NexXfloat(PID_OPTIONS2, 16, "options_2.ph_raw_value");
NexButton disp_options_rst_cl_day = NexButton(PID_OPTIONS2, 12, "options_2.rst_cl_day");
NexButton disp_options_rst_phm_day = NexButton(PID_OPTIONS2, 13, "options_2.rst_phm_day");

//NexButton disp_options_portal = NexButton(PID_OPTIONS2, 38, "options_2.portal");




NexWaveform disp_graph_temp = NexWaveform(PID_GRAPH, 1, "graph.ph_temp");
NexWaveform disp_graph_orp = NexWaveform(PID_GRAPH, 2, "graph.ph_orp");
NexWaveform disp_graph_ph = NexWaveform(PID_GRAPH, 3, "graph.ph_graph");
NexWaveform disp_graph_press = NexWaveform(PID_GRAPH, 4, "graph.ph_press");

NexButton disp_control_filter_auto = NexButton(PID_CONTROL, CID_CONTROL_FILTER_AUTO, "control.filter_auto");
NexButton disp_control_filter_off = NexButton(PID_CONTROL, CID_CONTROL_FILTER_OFF, "control.filter_off");
NexButton disp_control_filter_on = NexButton(PID_CONTROL, CID_CONTROL_FILTER_ON, "control.filter_on");

NexButton disp_control_cl_auto = NexButton(PID_CONTROL, CID_CONTROL_CL_AUTO, "control.cl_auto");
NexButton disp_control_cl_off = NexButton(PID_CONTROL, CID_CONTROL_CL_OFF, "control.cl_off");
NexButton disp_control_cl_on = NexButton(PID_CONTROL, CID_CONTROL_CL_ON, "control.cl_on");

NexButton disp_control_ph_minus_auto = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_AUTO, "control.phm_auto");
NexButton disp_control_ph_minus_off = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_OFF, "control.phm_off");
NexButton disp_control_ph_minus_on = NexButton(PID_CONTROL, CID_CONTROL_PH_MINUS_ON, "control.phm_on");

NexButton disp_control_ph_plus_auto = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_AUTO, "control.php_auto");
NexButton disp_control_ph_plus_off = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_OFF, "control.php_off");
NexButton disp_control_ph_plus_on = NexButton(PID_CONTROL, CID_CONTROL_PH_PLUS_ON, "control.php_on");


NexText disp_options_ip = NexText(PID_OPTIONS3, 6, "options_3.ip");
NexText disp_options_mac = NexText(PID_OPTIONS3, 7, "options_3.mac");
NexText disp_options_ssid = NexText(PID_OPTIONS3, 8, "options_3.ssid");
NexText disp_options_mqtt_server = NexText(PID_OPTIONS3, 15, "options_3.mqtt_server");
NexText disp_optionmqtt_topic = NexText(PID_OPTIONS3, 16, "options_3.mqtt_topic");
NexXfloat disp_options_rssi = NexXfloat(PID_OPTIONS3, 12, "options_3.rssi");
NexPicture disp_options_wifi_status = NexPicture(PID_OPTIONS3, 9, "options_3.wifi_value");
NexButton disp_options_portal = NexButton(PID_OPTIONS3, 5, "options_3.portal");
NexText disp_options_version = NexText(PID_OPTIONS3, 19, "options_3.version");
NexButton disp_options_reboot = NexButton(PID_OPTIONS3, 20, "options_3.reboot");
NexButton disp_options_default_param = NexButton(PID_OPTIONS3, 21, "options_3.default");


NexText disp_portal_title = NexText(PID_CUSTOM, 1, "portal.portal_titlle");
NexProgressBar disp_portal_progress = NexProgressBar(PID_CUSTOM, 2, "portal.portal_prog");
NexText disp_portal_message = NexText(PID_CUSTOM, 3, "portal.portal_text");
NexText disp_portal_message2 = NexText(PID_CUSTOM, 4, "portal.portal_text2");
NexText disp_portal_message3 = NexText(PID_CUSTOM, 5, "portal.portal_text3");

NexText disp_log_logger = NexText(PID_LOG, 1, "log.logger");
