#include "board.h"
#include "display.h"
#include "display_components.h"
#include "display_ctrl.h"
#include <Nextion.h>
#include <SerialDebug.h>
#include "measures.h"
#include "mqtt.h"
#include "parameters.h"
#include "orp_control.h"
#include "state.h"
#include "config.h"

display_page_t display_page = PAGE_STATUS;
void disp_timer_prog_to_display(uint32_t timer_prog);
uint32_t disp_disp_to_timer_prog_value(void);
void disp_options_to_parameters(void);
uint32_t touch_timeout = TOUCH_TIMEOUT_S;
bool saver_done;

uint8_t graph_temperature_buf[GRAPH_MAX_PTS];
uint8_t graph_ph_buf[GRAPH_MAX_PTS];
uint8_t graph_orp_buf[GRAPH_MAX_PTS];
uint8_t graph_pressure_buf[GRAPH_MAX_PTS];
uint8_t graph_nb_pts = 0;

void disp_options_ok_Callback(void *ptr)
{
	printlnA(F("Options OK pressed, reading back updated parameters and save Json"));
	delay(10);
	//page_options.show();
	disp_options_to_parameters();
	page_status.show();
	display_page = PAGE_STATUS;
	parameters_write_file();
	mqtt_publish_parameters();
}

void disp_enter_options_Callback(void *ptr)
{
	display_page = PAGE_OPTIONS;
	disp_parameters_to_display();
}

void disp_next_prev_Callback(void *ptr)
{
	NexPicture *btn = (NexPicture *)ptr;
	uint16_t id = (btn->getObjPid() << 8) + btn->getObjCid();

	switch (id)
	{
	case ((PID_STATUS << 8) + CID_STATUS_NEXT):
	case ((PID_GRAPH << 8) + CID_GRAPH_PREV):
		display_page = PAGE_CONTROL;
		page_control.show();
		printlnA(F("Entering control page"));
		break;

	case ((PID_GRAPH << 8) + CID_GRAPH_NEXT):
	case ((PID_STATUS << 8) + CID_STATUS_PREV):
		display_page = PAGE_LOG;
		page_log.show();
		printlnA(F("Entering log page"));
		break;

	case ((PID_LOG << 8) + CID_LOG_PREV):
	case ((PID_CONTROL << 8) + CID_CONTROL_NEXT):
		display_page = PAGE_GRAPH;
		page_graph.show();
		printlnA(F("Entering Graph page"));
		break;

	case ((PID_LOG << 8) + CID_LOG_NEXT):
	case ((PID_CONTROL << 8) + CID_CONTROL_PREV):
		display_page = PAGE_STATUS;
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
		NULL};

void disp_global_push_Callback(void *ptr)
{
	touch_timeout = millis() + TOUCH_TIMEOUT_S * 1000;
	buzzer_on();
	delay(5);
	buzzer_off();
}

void disp_page_saver_Callback(void *ptr)
{
	saver_done =true;
}

void display_init()
{
	printlnA(F("Display Init"));
	nexInit();
	printlnA(F("Display CallBack Init"));
	//page_saver.attachPush(disp_page_saver_Callback, &page_saver);
	disp_saver_exit.attachPush(disp_page_saver_Callback, &disp_saver_exit);

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
	printlnA(F("Display Display boot page"));
	page_boot.show();
	nexSetGlobalPushCb(disp_global_push_Callback);
	delay(2000);
	touch_timeout = TOUCH_TIMEOUT_S * 1000;
	printlnA(F("Display Init Done"));
}

void display_loop(void)
{
	static bool saver_entered = false;

	if ((millis() > touch_timeout) && (saver_entered==false))
	{
		page_saver.show();
		NexDim(0);
		saver_entered = true;
	}
	if (saver_done) {
		page_status.show();
		NexDim(100);
		saver_done = false;
		saver_entered = false;
	}
	nexLoop(nex_listen_list);
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

void disp_parameters_to_display(void)
{
	disp_options_ph.setValue(parameters.target_ph);
	disp_options_delta_ph.setValue(parameters.delta_ph);
	disp_options_orp.setValue(parameters.target_orp);
	disp_options_delta_orp.setValue(parameters.delta_orp);
	disp_options_cl_flow.setValue(parameters.flow_cl);
	disp_options_ph_minus_flow.setValue(parameters.flow_ph_minus);
	disp_options_ph_plus_flow.setValue(parameters.flow_ph_plus);
	disp_options_pressure_warning.setValue(parameters.pressure_warning);
	if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
	{
		disp_options_mode_timer_prog.setValue(1);
		disp_options_mode_fct_t.setValue(0);
	}
	else
	{
		disp_options_mode_timer_prog.setValue(0);
		disp_options_mode_fct_t.setValue(1);
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

void disp_measures_to_display(void)
{
	disp_water_temperature.setValue(measures.water_temperature);
	disp_ph.setValue(measures.ph);
	disp_pressure.setValue(measures.pump_pressure);
	disp_orp.setValue(measures.orp);
	disp_sys_humidity.setValue(measures.system_humidity);
	disp_sys_temperature.setValue(measures.system_temperature);
	disp_phm_day.setValue(measures.daily_ml_ph_minus);
	disp_cl_day.setValue(measures.daily_ml_orp);
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
	if (graph_nb_pts < GRAPH_MAX_PTS) {
		graph_nb_pts++;
	} else {
		for(i=0;i<GRAPH_MAX_PTS-1;i++) {
			graph_temperature_buf[i] = graph_temperature_buf[i+1];
			graph_ph_buf[i] = graph_ph_buf[i+1];
			graph_pressure_buf[i] = graph_pressure_buf[i+1];
			graph_orp_buf[i] = graph_orp_buf[i+1];
		}
	}
	graph_temperature_buf[graph_nb_pts-1] =  map(measures.water_temperature, 0, 9, 0, 150);
	graph_ph_buf[graph_nb_pts-1] =  map(measures.ph, 0, 9, 0, 150);
	graph_pressure_buf[graph_nb_pts-1] =  map(measures.pump_pressure, 0, 9, 0, 150);
	graph_orp_buf[graph_nb_pts-1] =  map(measures.orp, 0, 9, 0, 150);

	disp_graph_ph.addValues(0,graph_nb_pts, graph_ph_buf);
	disp_graph_orp.addValues(0,graph_nb_pts, graph_orp_buf);
	disp_graph_temp.addValues(0,graph_nb_pts, graph_temperature_buf);
	disp_graph_press.addValues(0,graph_nb_pts, graph_pressure_buf);


}