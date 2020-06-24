#ifndef __DISPLAY_CTRL_H__
#define __DISPLAY_CTRL_H__


#include "board.h"
#include "display.h"
#include "display_components.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"
#include "mqtt.h"
#include "parameters.h"



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
void disp_control_filter_power_full_Callback(void *ptr);
void disp_control_filter_power_reg_Callback(void *ptr);
void control_ph_plus_auto(void);
void control_ph_plus_on(void);
void control_ph_plus_off(void);
void control_ph_minus_auto(void);
void control_ph_minus_on(void);
void control_ph_minus_off(void);
void control_cl_auto(void);
void control_cl_on(void);
void control_cl_off(void);
void control_filter_auto(void);
void control_filter_on(void);
void control_filter_off(void);
void control_filter_pwr_full(void);
void control_filter_pwr_reg(void);

#endif