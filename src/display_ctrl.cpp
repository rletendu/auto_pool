#include "autopool.h"


void disp_control_ph_plus_auto_Callback(void *ptr)
{
	ph_plus_enter_mode(PH_PLUS_AUTO);
}
void disp_control_ph_plus_on_Callback(void *ptr)
{
	ph_plus_enter_mode(PH_PLUS_ON);
}
void disp_control_ph_plus_off_Callback(void *ptr)
{
	ph_plus_enter_mode(PH_PLUS_OFF);
}
void disp_control_ph_minus_auto_Callback(void *ptr)
{
	ph_minus_enter_mode(PH_MINUS_AUTO);
}
void disp_control_ph_minus_on_Callback(void *ptr)
{
	ph_minus_enter_mode(PH_MINUS_ON);
}
void disp_control_ph_minus_off_Callback(void *ptr)
{
	ph_minus_enter_mode(PH_MINUS_OFF);
}
void disp_control_cl_auto_Callback(void *ptr)
{
	orp_enter_mode(ORP_AUTO);
}
void disp_control_cl_on_Callback(void *ptr)
{
	orp_enter_mode(ORP_ON);
}
void disp_control_cl_off_Callback(void *ptr)
{
	orp_enter_mode(ORP_OFF);
}
void disp_control_filter_auto_Callback(void *ptr)
{
	filter_enter_mode(FILTER_AUTO);
}
void disp_control_filter_on_Callback(void *ptr)
{
	filter_enter_mode(FILTER_ON);
}
void disp_control_filter_off_Callback(void *ptr)
{
	filter_enter_mode(FILTER_OFF);
}
void disp_control_filter_power_full_Callback(void *ptr)
{
	filter_enter_power_mode(FILTER_POWER_FULL);
}

void disp_control_filter_power_reg_Callback(void *ptr)
{
	filter_enter_power_mode(FILTER_POWER_REG);
}

void control_ph_plus_auto(void)
{
	disp_control_ph_plus_auto.Set_background_color_bco(DISP_COLOR_GREEN);
	disp_control_ph_plus_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_plus_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_ph_plus_on(void)
{
	disp_control_ph_plus_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_plus_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_plus_on.Set_background_color_bco(DISP_COLOR_ORANGE);
}
void control_ph_plus_off(void)
{
	disp_control_ph_plus_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_plus_off.Set_background_color_bco(DISP_COLOR_RED);
	disp_control_ph_plus_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_ph_minus_auto(void)
{
	disp_control_ph_minus_auto.Set_background_color_bco(DISP_COLOR_GREEN);
	disp_control_ph_minus_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_minus_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_ph_minus_on(void)
{
	disp_control_ph_minus_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_minus_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_minus_on.Set_background_color_bco(DISP_COLOR_ORANGE);
}
void control_ph_minus_off(void)
{
	disp_control_ph_minus_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_ph_minus_off.Set_background_color_bco(DISP_COLOR_RED);
	disp_control_ph_minus_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_cl_auto(void)
{
	disp_control_cl_auto.Set_background_color_bco(DISP_COLOR_GREEN);
	disp_control_cl_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_cl_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_cl_on(void)
{
	disp_control_cl_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_cl_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_cl_on.Set_background_color_bco(DISP_COLOR_ORANGE);
}
void control_cl_off(void)
{
	disp_control_cl_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_cl_off.Set_background_color_bco(DISP_COLOR_RED);
	disp_control_cl_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_filter_auto(void)
{
	disp_control_filter_auto.Set_background_color_bco(DISP_COLOR_GREEN);
	disp_control_filter_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_filter_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_filter_on(void)
{
	disp_control_filter_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_filter_off.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_filter_on.Set_background_color_bco(DISP_COLOR_ORANGE);
}
void control_filter_off(void)
{
	disp_control_filter_auto.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_filter_off.Set_background_color_bco(DISP_COLOR_RED);
	disp_control_filter_on.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_filter_pwr_full(void)
{
	disp_control_pump_pwr_full.Set_background_color_bco(DISP_COLOR_GREEN);
	disp_control_pump_pwr_reg.Set_background_color_bco(DISP_COLOR_GRAY);
}
void control_filter_pwr_reg(void)
{
	disp_control_pump_pwr_full.Set_background_color_bco(DISP_COLOR_GRAY);
	disp_control_pump_pwr_reg.Set_background_color_bco(DISP_COLOR_GREEN);
}
