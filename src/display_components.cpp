
#include <Nextion.h>
#include "display_components.h"



NexPage page_boot = NexPage(PID_BOOT, 0, "boot");
NexPage page_status = NexPage(PID_STATUS, 0, "status");
NexPage page_control = NexPage(PID_CONTROL, 0, "control");
NexPage page_graph = NexPage(PID_GRAPH, 0, "graph");
NexPage page_log = NexPage(PID_LOG, 0, "log");
NexPage page_options = NexPage(PID_OPTIONS, 0, "option");
NexPage page_ota = NexPage(PID_OTA, 0, "OTA");

NexProgressBar ota_progress = NexProgressBar(PID_OTA, 1, "j0");

NexXfloat disp_water_temperature = NexXfloat(PID_STATUS, CID_STATUS_WATER_TEMP, "status.temp_value");
NexXfloat disp_ph = NexXfloat(PID_STATUS, CID_STATUS_PH, "status.ph_value");
NexXfloat disp_orp = NexXfloat(PID_STATUS, CID_STATUS_ORP, "status.orp_value");
NexXfloat disp_pressure = NexXfloat(PID_STATUS, CID_STATUS_PRESSURE, "status.press_value");
NexXfloat disp_sys_temperature = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_TEMP, "status.sys_temp_value");
NexXfloat disp_sys_humidity = NexXfloat(PID_STATUS, CID_STATUS_SYSTEM_HUMIDITY, "status.sys_hum_value");
NexPicture disp_wifi = NexPicture(PID_STATUS, CID_STATUS_WIFI, "status.wifi_value");

NexPicture disp_led_ph = NexPicture(PID_STATUS, CID_STATUS_LED_PH, "status.ph_led");
NexPicture disp_led_orp = NexPicture(PID_STATUS, CID_STATUS_LED_ORP, "status.orp_led");
NexPicture disp_led_pressure = NexPicture(PID_STATUS, CID_STATUS_LED_PRESSURE, "status.press_led");

NexPicture disp_led_level_water = NexPicture(PID_STATUS, CID_STATUS_WATER_LEVEL, "status.water_lev");
NexPicture disp_led_level_cl = NexPicture(PID_STATUS, CID_STATUS_CL_LEVEL, "status.cl_level");
NexPicture disp_led_level_ph_minus = NexPicture(PID_STATUS, CID_STATUS_PH_MINUS_LEVEL, "status.phm_level");

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

NexWaveform disp_graph_temp = NexWaveform(PID_GRAPH, 1, "graph.ph_temp");
NexWaveform disp_graph_orp= NexWaveform(PID_GRAPH, 2, "graph.ph_orp");
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

