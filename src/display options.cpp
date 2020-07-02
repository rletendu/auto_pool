#include "autopool.h"

void disp_timer_prog_to_display(uint32_t timer_prog);
uint32_t disp_disp_to_timer_prog_value(void);
void disp_options_to_parameters(void);
static bool page_options_active = false;

bool disp_page_options_is_active(void)
{
	return page_options_active;
}

void disp_options_ok_Callback(void *ptr)
{
	printlnA(F("Options OK pressed, reading back updated parameters and save Json"));
	delay(10);
	//page_options.show();
	disp_options_to_parameters();
	page_status.show();
	page_options_active = false;
	parameters_write_file();
	mqtt_publish_parameters();
}

void disp_options_cancel_Callback(void *ptr)
{
	printlnA(F("Options CANCEL pressed"));
	page_status.show();
	page_options_active = false;
}

void disp_enter_options_Callback(void *ptr)
{
	printlnA("Entering Page option")
		page_options.show();
	page_options_active = true;
	printlnA("Updating Options")
		disp_parameters_to_display();
}

void disp_options_rst_cl_day_Callback(void *ptr)
{
	measures.daily_ml_orp = 0;
}
void disp_options_rst_phm_day_Callback(void *ptr)
{
	measures.daily_ml_ph_minus = 0;
}
void disp_options_reboot_Callback(void *ptr)
{
	esp_restart();
}
void disp_options_default_param_Callback(void *ptr)
{
	parameters_set_default();
	disp_parameters_to_display();
}

void disp_options_portal_Callback(void *ptr)
{
#if HAS_WEB_SERVER
	webserver_stop();
#endif
	wifimanager_reset_portal();
	wifimanager_start_portal();
	esp_restart();
}

void disp_parameters_to_display(void)
{
	printlnA("Update Options Values")
		delay(100);
	if (disp_options_ph.setValue(parameters.target_ph) == false)
	{
		printlnA(F("Error disp_options_ph.setValue"));
	}
	if (disp_options_delta_ph.setValue(parameters.delta_ph) == false)
	{
		printlnA(F("disp_options_delta_ph.setValue"));
	}
	if (disp_options_orp.setValue(parameters.target_orp) == false)
	{
		printlnA(F("disp_options_orp.setValue(parameters.target_orp)"));
	}
	if (disp_options_delta_orp.setValue(parameters.delta_orp) == false)
	{
		printlnA(F("disp_options_delta_orp.setValue(parameters.delta_orp)"));
	}
	if (disp_options_cl_flow.setValue(parameters.flow_cl) == false)
	{
		printlnA(F("disp_options_cl_flow.setValue(parameters.flow_cl)"));
	}
	if (disp_options_ph_minus_flow.setValue(parameters.flow_ph_minus) == false)
	{
		printlnA(F("disp_options_ph_minus_flow.setValue(parameters.flow_ph_minus)"));
	}
	if (disp_options_ph_plus_flow.setValue(parameters.flow_ph_plus) == false)
	{
		printlnA(F("disp_options_ph_plus_flow.setValue(parameters.flow_ph_plus)"));
	}
	if (disp_options_pressure_warning.setValue(parameters.pressure_warning) == false)
	{
		printlnA(F("disp_options_pressure_warning.setValue(parameters.pressure_warning)"));
	}
	if (disp_options_max_cl.setValue(parameters.cl_max_day) == false)
	{
		printlnA(F("disp_options_max_cl.setValue(parameters.cl_max_day)"));
	}
	if (disp_options_max_phm.setValue(parameters.phm_max_day) == false)
	{
		printlnA(F("disp_options_max_phm.setValue(parameters.phm_max_day)"));
	}
	if (disp_options_orp_offset.setValue(parameters.orp_offset) == false)
	{
		printlnA(F("disp_options_orp_offset.setValue(parameters.orp_offset)"));
	}
	if (disp_options_ph_offset.setValue(parameters.ph_offset) == false)
	{
		printlnA(F("disp_options_ph_offset.setValue(parameters.ph_offset)"));
	}
	if (disp_options_periodic_filter.setValue(parameters.periodic_filter_time) == false)
	{
		printlnA(F("disp_options_periodic_filter.setValue(parameters.periodic_filter_time)"));
	}
	if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
	{

		if (disp_options_mode_timer_prog.setValue(1) == false)
		{
			printlnA(F("disp_options_mode_timer_prog.setValue(1)"));
		}
		if (disp_options_mode_fct_t.setValue(0) == false)
		{
			printlnA(F("disp_options_mode_fct_t.setValue(0)"));
		}
	}
	else
	{
		if (disp_options_mode_timer_prog.setValue(0) == false)
		{
			printlnA(F("disp_options_mode_timer_prog.setValue(0)"));
		}
		if (disp_options_mode_fct_t.setValue(1) == false)
		{
			printlnA(F("disp_options_mode_fct_t.setValue(1)"));
		}
	}
	disp_timer_prog_to_display(parameters.timer_prog);
}

void disp_options_to_parameters(void)
{
	float val_f;
	uint32_t val_u;

	printA(F("Reading back pH Target : "));
	disp_options_ph.getValue(&val_f);
	parameters.target_ph = val_f;
	printlnA(parameters.target_ph);
	printA(F("Reading back delta pH Target"));
	disp_options_delta_ph.getValue(&val_f);
	parameters.delta_ph = val_f;
	printlnA(parameters.delta_ph);
	printlnA(F("Reading back ORP Target"));
	disp_options_orp.getValue(&val_f);
	parameters.target_orp = val_f;
	printlnA(F("Reading back ORP delta Target"));
	disp_options_delta_orp.getValue(&val_f);
	parameters.delta_orp = val_f;
	printlnA(F("Reading back cl flow"));
	disp_options_cl_flow.getValue(&val_f);
	parameters.flow_cl = val_f;
	printlnA(F("Reading back pHminus flow"));
	disp_options_ph_minus_flow.getValue(&val_f);
	parameters.flow_ph_minus = val_f;
	printlnA(F("Reading back pHplus flow"));
	disp_options_ph_plus_flow.getValue(&val_f);
	parameters.flow_ph_plus = val_f;
	disp_options_pressure_warning.getValue(&val_f);
	parameters.pressure_warning = val_f;
	disp_options_max_cl.getValue(&val_f);
	parameters.cl_max_day = val_f;
	disp_options_max_phm.getValue(&val_f);
	parameters.phm_max_day = val_f;
	disp_options_orp_offset.getValue(&val_f);
	parameters.orp_offset = val_f;
	disp_options_ph_offset.getValue(&val_f);
	parameters.ph_offset = val_f;
	disp_options_periodic_filter.getValue(&val_f);
	parameters.periodic_filter_time = val_f;
	disp_options_mode_timer_prog.getValue(&val_u);
	if (val_u)
	{
		parameters.filter_auto_mode = AUTO_TIMER_PROG;
	}
	else
	{
		parameters.filter_auto_mode = AUTO_TIMER_FCT_T;
	}
	parameters.timer_prog = disp_disp_to_timer_prog_value();
}

void disp_timer_prog_to_display(uint32_t timer_prog)
{
	disp_otions_0h.setValue((timer_prog >> 0) & 0x01);
	disp_otions_1h.setValue((timer_prog >> 1) & 0x01);
	disp_otions_2h.setValue((timer_prog >> 2) & 0x01);
	disp_otions_3h.setValue((timer_prog >> 3) & 0x01);
	disp_otions_4h.setValue((timer_prog >> 4) & 0x01);
	disp_otions_5h.setValue((timer_prog >> 5) & 0x01);
	disp_otions_6h.setValue((timer_prog >> 6) & 0x01);
	disp_otions_7h.setValue((timer_prog >> 7) & 0x01);
	disp_otions_8h.setValue((timer_prog >> 8) & 0x01);
	disp_otions_9h.setValue((timer_prog >> 9) & 0x01);
	disp_otions_10h.setValue((timer_prog >> 10) & 0x01);
	disp_otions_11h.setValue((timer_prog >> 11) & 0x01);
	disp_otions_12h.setValue((timer_prog >> 12) & 0x01);
	disp_otions_13h.setValue((timer_prog >> 13) & 0x01);
	disp_otions_14h.setValue((timer_prog >> 14) & 0x01);
	disp_otions_15h.setValue((timer_prog >> 15) & 0x01);
	disp_otions_16h.setValue((timer_prog >> 16) & 0x01);
	disp_otions_17h.setValue((timer_prog >> 17) & 0x01);
	disp_otions_18h.setValue((timer_prog >> 18) & 0x01);
	disp_otions_19h.setValue((timer_prog >> 19) & 0x01);
	disp_otions_20h.setValue((timer_prog >> 20) & 0x01);
	disp_otions_21h.setValue((timer_prog >> 21) & 0x01);
	disp_otions_22h.setValue((timer_prog >> 22) & 0x01);
	disp_otions_23h.setValue((timer_prog >> 23) & 0x01);
}

uint32_t disp_disp_to_timer_prog_value(void)
{
	uint32_t timer_prog = 0;
	uint32_t cb_val;
	disp_otions_0h.getValue(&cb_val);
	timer_prog += cb_val << 0;
	disp_otions_1h.getValue(&cb_val);
	timer_prog += cb_val << 1;
	disp_otions_2h.getValue(&cb_val);
	timer_prog += cb_val << 2;
	disp_otions_3h.getValue(&cb_val);
	timer_prog += cb_val << 3;
	disp_otions_4h.getValue(&cb_val);
	timer_prog += cb_val << 4;
	disp_otions_5h.getValue(&cb_val);
	timer_prog += cb_val << 5;
	disp_otions_6h.getValue(&cb_val);
	timer_prog += cb_val << 6;
	disp_otions_7h.getValue(&cb_val);
	timer_prog += cb_val << 7;
	disp_otions_8h.getValue(&cb_val);
	timer_prog += cb_val << 8;
	disp_otions_9h.getValue(&cb_val);
	timer_prog += cb_val << 9;
	disp_otions_10h.getValue(&cb_val);
	timer_prog += cb_val << 10;
	disp_otions_11h.getValue(&cb_val);
	timer_prog += cb_val << 11;
	disp_otions_12h.getValue(&cb_val);
	timer_prog += cb_val << 12;
	disp_otions_13h.getValue(&cb_val);
	timer_prog += cb_val << 13;
	disp_otions_14h.getValue(&cb_val);
	timer_prog += cb_val << 14;
	disp_otions_15h.getValue(&cb_val);
	timer_prog += cb_val << 15;
	disp_otions_16h.getValue(&cb_val);
	timer_prog += cb_val << 16;
	disp_otions_17h.getValue(&cb_val);
	timer_prog += cb_val << 17;
	disp_otions_18h.getValue(&cb_val);
	timer_prog += cb_val << 18;
	disp_otions_19h.getValue(&cb_val);
	timer_prog += cb_val << 19;
	disp_otions_20h.getValue(&cb_val);
	timer_prog += cb_val << 20;
	disp_otions_21h.getValue(&cb_val);
	timer_prog += cb_val << 21;
	disp_otions_22h.getValue(&cb_val);
	timer_prog += cb_val << 22;
	disp_otions_23h.getValue(&cb_val);
	timer_prog += cb_val << 23;
	return timer_prog;
}
