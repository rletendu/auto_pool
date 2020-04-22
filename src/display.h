
#ifndef __DISPLAY_H__
#define __DISPLAY_H__

#include "display_ctrl.h"

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
void display_saver_stop(void);
void display_loop(void);
void stop_display_tasks(void);

void disp_page_ota();
void disp_ota_progress(uint8_t progress);
void disp_parameters_to_display(void);
void disp_measures_to_display(void);
void disp_orp_state_to_display(void);
void disp_ph_state_to_display(void);
void disp_measures_to_graph(void);

void disp_control_ph_plus_auto_Callback(void *ptr);
void disp_control_ph_plus_on_Callback(void *ptr);
void disp_control_ph_plus_off_Callback(void *ptr);
void disp_control_ph_minus_auto_Callback(void *ptr);
void disp_control_ph_minus_on_Callback(void *ptr);
void disp_control_ph_minus_off_Callback(void *ptr);
void disp_control_cl_auto_Callback(void *ptr);
void disp_control_cl_on_Callback(void *ptr);
void disp_control_cl_off_Callback(void *ptr);
void disp_control_filter_auto_Callback(void *ptr);
void disp_control_filter_on_Callback(void *ptr);
void disp_control_filter_off_Callback(void *ptr);
void control_ph_plus_disable_controls(void);
#endif