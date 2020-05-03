
#ifndef __DISPLAY_OPTIONS_H__
#define __DISPLAY_OPTIONS_H__

#include "display.h"

void disp_parameters_to_display(void);
void disp_options_ok_Callback(void *ptr);
void disp_options_cancel_Callback(void *ptr);
void disp_enter_options_Callback(void *ptr);
bool disp_page_options_is_active(void);
void disp_options_rst_cl_day_Callback(void *ptr);
void disp_options_rst_phm_day_Callback(void *ptr);
void disp_options_reboot_Callback(void *ptr);
void disp_options_default_param_Callback(void *ptr);
void disp_options_portal_Callback(void *ptr);
#endif