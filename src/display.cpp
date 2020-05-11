#include "autopool.h"

extern SoftTimer timer_pool;
void disp_timer_prog_to_display(uint32_t timer_prog);
uint32_t disp_disp_to_timer_prog_value(void);
void disp_options_to_parameters(void);
bool disp_touch_events_enable = true;
uintptr_t disp_enter_screen_saver_task;
uint8_t graph_temperature_buf[GRAPH_MAX_PTS];
uint8_t graph_ph_buf[GRAPH_MAX_PTS];
uint8_t graph_orp_buf[GRAPH_MAX_PTS];
uint8_t graph_pressure_buf[GRAPH_MAX_PTS];
uint8_t graph_nb_pts = 0;

static bool screen_saver_entered = false;
bool disp_screen_saver_timeout(void *);
void time_update_stop(void);



void disp_wifi_status(bool enable)
{
	static bool first_time = true;
	static bool prev = true;
	char msg[30];
	if (enable != prev || first_time)
	{
		prev = enable;
		first_time = false;
		disp_options_mac.setText(WiFi.macAddress().c_str());
		if (enable)
		{
			IPAddress ip = WiFi.localIP();
			sprintf(msg, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			dis_sys_user0.setText(msg);
			sprintf(msg, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
			disp_options_ip.setText(msg);
			disp_wifi.setPic(ID_IMAGE_WIFI_ON);
			disp_options_wifi_status.setPic(ID_IMAGE_WIFI_ON);
			disp_options_ssid.setText(WiFi.SSID().c_str());
		}
		else
		{
			dis_sys_user0.setText("---.---.---.--- ");
			disp_options_ip.setText("---.---.---.--- ");
			disp_wifi.setPic(ID_IMAGE_WIFI_OFF);
			disp_options_wifi_status.setPic(ID_IMAGE_WIFI_OFF);
			disp_options_ssid.setText("- - - - -");
		}
	}
}

void disp_boot_progress_message(char *msg)
{
	static uint8_t progress = 0;
	progress += 100 / NB_BOOT_STEP_MSG;
	if (progress > 100)
	{
		progress = 100;
	}
	disp_boot_progress(progress);
	disp_boot_mesg(msg);
	delay(500);
}

void disp_boot_mesg(char *msg)
{
	boot_message.setText(msg);
}

void disp_boot_progress(uint8_t progress)
{
	boot_progress.setValue(progress);
}

void stop_display_tasks()
{
	NexDim(100);
	display_saver_stop();
	time_update_stop();
	orp_control_stop();
	measures_loop_stop();
	ph_control_stop();
	disp_touch_events_enable = false;
}

void display_saver_stop(void)
{
	timer_pool.cancel(disp_enter_screen_saver_task);
}

void disp_next_prev_Callback(void *ptr)
{
	NexPicture *btn = (NexPicture *)ptr;
	uint16_t id = (btn->getObjPid() << 8) + btn->getObjCid();

	switch (id)
	{
	case ((PID_STATUS << 8) + CID_STATUS_NEXT):
	case ((PID_GRAPH << 8) + CID_GRAPH_PREV):
		page_control.show();
		printlnA(F("Entering control page"));
		break;

	case ((PID_GRAPH << 8) + CID_GRAPH_NEXT):
	case ((PID_STATUS << 8) + CID_STATUS_PREV):
		page_log.show();
		printlnA(F("Entering log page"));
		break;

	case ((PID_LOG << 8) + CID_LOG_PREV):
	case ((PID_CONTROL << 8) + CID_CONTROL_NEXT):
		page_graph.show();
		printlnA(F("Entering Graph page"));
		break;

	case ((PID_LOG << 8) + CID_LOG_NEXT):
	case ((PID_CONTROL << 8) + CID_CONTROL_PREV):
		page_status.show();
		printlnA(F("Entering Status page"));
		break;

	default:
		break;
	}
}

/*
  Register object textNumber, buttonPlus, buttonMinus, to the touch event list.
*/
NexTouch *nex_listen_list[] =
	{
		&disp_next_status,
		&disp_prev_status,
		&disp_next_control,
		&disp_prev_control,
		&disp_next_graph,
		&disp_prev_graph,
		&disp_next_log,
		&disp_prev_log,
		&disp_options_ok,
		&disp_options_cancel,
		&disp_options,
		&disp_control_cl_auto,
		&disp_control_cl_off,
		&disp_control_cl_on,
		&disp_control_ph_minus_auto,
		&disp_control_ph_minus_off,
		&disp_control_ph_minus_on,
		&disp_control_ph_plus_auto,
		&disp_control_ph_plus_off,
		&disp_control_ph_plus_on,
		&disp_control_filter_auto,
		&disp_control_filter_off,
		&disp_control_filter_on,
		&disp_saver_exit,
		&disp_options_rst_cl_day,
		&disp_options_rst_phm_day,
		&disp_options_reboot,
		&disp_options_default_param,
		&disp_options_portal,
		NULL};

void disp_page_saver_exit_Callback(void *ptr)
{
	if (screen_saver_entered)
	{
		page_status.show();
		NexDim(100);
		screen_saver_entered = false;
	}
}

bool disp_screen_saver_timeout(void *)
{
	uint8_t page;

	GetPageId(&page);
	if (disp_page_options_is_active())
	{
		disp_enter_screen_saver_task = timer_pool.in(TOUCH_TIMEOUT_S * 1000, disp_screen_saver_timeout);
		printlnA(F("Screen saver timeout in option pages : NOP"));
		return true;
	}
	printA(F("Page when entering Screen Saver :"));
	printlnA(page);
	printlnA(F("Dim Screen and Enter Screen Saver"));
	page_saver.show();
	NexDim(0);
	screen_saver_entered = true;
	return false;
}

void disp_global_push_Callback(void *ptr)
{
	timer_pool.cancel(disp_enter_screen_saver_task);
	disp_enter_screen_saver_task = timer_pool.in(TOUCH_TIMEOUT_S * 1000, disp_screen_saver_timeout);
	buzzer_on();
	delay(5);
	buzzer_off();
	if (screen_saver_entered)
	{
		printlnA(F("Exit Screen Saver"));
		page_status.show();
		NexDim(100);
		screen_saver_entered = false;
	}
}

void display_init()
{
	printlnA(F("Display Init"));
	nexInit(DISPLAY_BAUD);
	printlnA(F("Display CallBack Init"));
	disp_saver_exit.attachPush(disp_page_saver_exit_Callback, &disp_saver_exit);

	disp_next_status.attachPush(disp_next_prev_Callback, &disp_next_status);
	disp_prev_status.attachPush(disp_next_prev_Callback, &disp_prev_status);
	disp_next_control.attachPush(disp_next_prev_Callback, &disp_next_control);
	disp_prev_control.attachPush(disp_next_prev_Callback, &disp_prev_control);
	disp_next_graph.attachPush(disp_next_prev_Callback, &disp_next_graph);
	disp_prev_graph.attachPush(disp_next_prev_Callback, &disp_prev_graph);
	disp_next_log.attachPush(disp_next_prev_Callback, &disp_next_log);
	disp_prev_log.attachPush(disp_next_prev_Callback, &disp_prev_log);
	disp_options.attachPush(disp_enter_options_Callback, &disp_options);
	disp_options_ok.attachPush(disp_options_ok_Callback, &disp_options_ok);
	disp_options_cancel.attachPush(disp_options_cancel_Callback, &disp_options_cancel);

	disp_control_cl_auto.attachPush(disp_control_cl_auto_Callback, &disp_control_cl_auto);
	disp_control_cl_on.attachPush(disp_control_cl_on_Callback, &disp_control_cl_on);
	disp_control_cl_off.attachPush(disp_control_cl_off_Callback, &disp_control_cl_off);

	disp_control_ph_minus_auto.attachPush(disp_control_ph_minus_auto_Callback, &disp_control_ph_minus_auto);
	disp_control_ph_minus_on.attachPush(disp_control_ph_minus_on_Callback, &disp_control_ph_minus_on);
	disp_control_ph_minus_off.attachPush(disp_control_ph_minus_off_Callback, &disp_control_ph_minus_off);

	disp_control_ph_plus_auto.attachPush(disp_control_ph_plus_auto_Callback, &disp_control_ph_plus_auto);
	disp_control_ph_plus_on.attachPush(disp_control_ph_plus_on_Callback, &disp_control_ph_plus_on);
	disp_control_ph_plus_off.attachPush(disp_control_ph_plus_off_Callback, &disp_control_ph_plus_off);

	disp_control_filter_auto.attachPush(disp_control_filter_auto_Callback, &disp_control_filter_auto);
	disp_control_filter_on.attachPush(disp_control_filter_on_Callback, &disp_control_filter_on);
	disp_control_filter_off.attachPush(disp_control_filter_off_Callback, &disp_control_filter_off);

	disp_options_rst_cl_day.attachPush(disp_options_rst_cl_day_Callback, &disp_options_rst_cl_day);
	disp_options_rst_phm_day.attachPush(disp_options_rst_phm_day_Callback, &disp_options_rst_phm_day);
	disp_options_reboot.attachPush(disp_options_reboot_Callback, &disp_options_reboot);
	disp_options_default_param.attachPush(disp_options_default_param_Callback, &disp_options_default_param);
	disp_options_portal.attachPush(disp_options_portal_Callback, &disp_options_portal);

	nexSetGlobalPushCb(disp_global_push_Callback);
	//delay(2000);

	disp_enter_screen_saver_task = timer_pool.in(TOUCH_TIMEOUT_S * 1000, disp_screen_saver_timeout);

	printlnA(F("Display Init Done"));
}

void display_loop(void)
{
	if (disp_touch_events_enable)
	{
		nexLoop(nex_listen_list);
	}
}

void disp_page_ota()
{
	page_ota.show();
	NexDim(100);
}

void disp_ota_progress(uint8_t progress)
{
	ota_progress.setValue(progress);
}

void disp_measures_to_display(void)
{
	float val_f;
	disp_water_temperature.setValue(measures.water_temperature);
	disp_ph.setValue(measures.ph);
	disp_options_ph_raw_value.setValue(measures.ph_raw);
	if (disp_page_options_is_active())
	{
		disp_options_ph_offset.getValue(&val_f);
		disp_options_ph_value.setValue(measures.ph_raw + val_f);
	}
	else
	{
		disp_options_ph_value.setValue(measures.ph);
	}

	disp_orp.setValue(measures.orp);
	disp_options_orp_raw_value.setValue(measures.orp_raw);
	if (disp_page_options_is_active())
	{
		disp_options_orp_offset.getValue(&val_f);
		disp_options_orp_value.setValue(measures.orp_raw + val_f);
	}
	else
	{
		disp_options_orp_value.setValue(measures.orp);
	}

	disp_pressure.setValue(measures.pump_pressure);
	disp_sys_humidity.setValue(measures.system_humidity);
	disp_sys_temperature.setValue(measures.system_temperature);
	disp_phm_day.setValue(measures.daily_ml_ph_minus);
	if (measures.daily_ml_ph_minus <= parameters.phm_max_day)
	{
		disp_phm_day.Set_font_color_pco(DISP_COLOR_LIGHT_GREEN);
	}
	else
	{
		disp_phm_day.Set_font_color_pco(DISP_COLOR_RED);
	}
	disp_cl_day.setValue(measures.daily_ml_orp);
	if (measures.daily_ml_orp <= parameters.cl_max_day)
	{
		disp_cl_day.Set_font_color_pco(DISP_COLOR_LIGHT_GREEN);
	}
	else
	{
		disp_cl_day.Set_font_color_pco(DISP_COLOR_RED);
	}
	if (measures.level_cl)
	{
		disp_led_level_cl.setPic(ID_IMAGE_GREEN);
	}
	else
	{
		disp_led_level_cl.setPic(ID_IMAGE_RED);
	}
	if (measures.level_ph_minus)
	{
		disp_led_level_ph_minus.setPic(ID_IMAGE_GREEN);
	}
	else
	{
		disp_led_level_ph_minus.setPic(ID_IMAGE_RED);
	}
	if (measures.level_water)
	{
		disp_led_level_water.setPic(ID_IMAGE_GREEN);
	}
	else
	{
		disp_led_level_water.setPic(ID_IMAGE_RED);
	}
}

void disp_orp_state_to_display(void)
{

	if (state.orp_control_state != ORP_IDLE)
	{
		disp_led_orp.setPic(ID_IMAGE_ORANGE);
	}
	else if (orp_correction_needed() != NO_CORRECTION)
	{
		disp_led_orp.setPic(ID_IMAGE_RED);
	}
	else
	{
		disp_led_orp.setPic(ID_IMAGE_GREEN);
	}
}

void disp_ph_state_to_display(void)
{
	if (state.ph_control_state != PH_IDLE)
	{
		disp_led_ph.setPic(ID_IMAGE_ORANGE);
	}
	else if (orp_correction_needed() != NO_CORRECTION)
	{
		disp_led_ph.setPic(ID_IMAGE_RED);
	}
	else
	{
		disp_led_ph.setPic(ID_IMAGE_GREEN);
	}
}

void disp_measures_to_graph(void)
{
	uint8_t val;
	val = map(measures.ph, 0, 9, 0, 150);
	disp_graph_ph.addValue(0, val);
	val = map(measures.orp, 0, 9, 0, 150);
	disp_graph_orp.addValue(0, val);
	val = map(measures.water_temperature, 0, 9, 0, 150);
	disp_graph_temp.addValue(0, val);
	val = map(measures.pump_pressure, 0, 9, 0, 150);
	disp_graph_press.addValue(0, val);
}

void disp_compute_graph_buffers(void)
{
	uint8_t i;
	if (graph_nb_pts < GRAPH_MAX_PTS)
	{
		graph_nb_pts++;
	}
	else
	{
		for (i = 0; i < GRAPH_MAX_PTS - 1; i++)
		{
			graph_temperature_buf[i] = graph_temperature_buf[i + 1];
			graph_ph_buf[i] = graph_ph_buf[i + 1];
			graph_pressure_buf[i] = graph_pressure_buf[i + 1];
			graph_orp_buf[i] = graph_orp_buf[i + 1];
		}
	}
	graph_temperature_buf[graph_nb_pts - 1] = map(measures.water_temperature, 0, 9, 0, 150);
	graph_ph_buf[graph_nb_pts - 1] = map(measures.ph, 0, 9, 0, 150);
	graph_pressure_buf[graph_nb_pts - 1] = map(measures.pump_pressure, 0, 9, 0, 150);
	graph_orp_buf[graph_nb_pts - 1] = map(measures.orp, 0, 9, 0, 150);

	disp_graph_ph.addValues(0, graph_nb_pts, graph_ph_buf);
	disp_graph_orp.addValues(0, graph_nb_pts, graph_orp_buf);
	disp_graph_temp.addValues(0, graph_nb_pts, graph_temperature_buf);
	disp_graph_press.addValues(0, graph_nb_pts, graph_pressure_buf);
}