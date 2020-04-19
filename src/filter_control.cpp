#include "parameters.h"
#include "filter_control.h"
#include <SerialDebug.h>
#include "config.h"
#include "measures.h"
#include "timer.h"
#include "display_ctrl.h"
#include "state.h"
#include "board.h"
#include "soft_timer.h"
#include "filter_control.h"

extern SoftTimer timer_pool;
uintptr_t filter_control_update_task;
bool filter_control_update(void *);

static void filter_on(void)
{
	pump_filtration_on();
	if (state.filter_pump != PUMP_ON)
	{
		state.filter_pump = PUMP_ON;
		disp_led_pump_water.setPic(ID_IMAGE_GREEN);
		mqtt_publish_filter_state();
	}
}

static void filter_off(void)
{
	pump_filtration_off();
	if (state.filter_pump != PUMP_OFF)
	{
		state.filter_pump = PUMP_OFF;
		disp_led_pump_water.setPic(ID_IMAGE_RED);
		mqtt_publish_filter_state();
	}
}

void filter_enter_mode(enum filter_mode_t filter_mode)
{
	switch (filter_mode)
	{
	case FILTER_AUTO:
		control_filter_auto();
		if (state.filter_mode != FILTER_AUTO)
		{
			state.filter_mode = FILTER_AUTO;
			mqtt_publish_filter_state();
		}
		break;

	case FILTER_OFF:
		control_filter_off();
		if (state.filter_mode != FILTER_OFF)
		{
			state.filter_mode = FILTER_OFF;
			mqtt_publish_filter_state();
		}
		filter_off();
		break;

	case FILTER_ON:
		control_filter_on();
		if (state.filter_mode != FILTER_ON)
		{
			state.filter_mode = FILTER_ON;
			mqtt_publish_filter_state();
		}
		filter_on();
		break;

	default:
		break;
	}
}

void filter_control_init(void)
{
	printlnA(F("Filter Control Init"));
	disp_led_pump_water.setPic(ID_IMAGE_RED);
	filter_enter_mode(FILTER_OFF);
	filter_enter_mode(FILTER_AUTO);
	filter_control_update_task = timer_pool.every(FILTER_CONTROL_UPDATE_MS, filter_control_update);
	mqtt_publish_filter_state();
}

bool filter_control_update(void *)
{
	uint8_t h;
	uint32_t r;
	if (state.filter_mode == FILTER_AUTO)
	{
		if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
		{
			h = rtc_get_hour();
			r = parameters.timer_prog & uint32_t(1 << h);
			if (r)
			{
				filter_on();
			}
			else
			{
				if (state.ph_control_state == PH_IDLE && state.orp_control_state == ORP_IDLE) {
					filter_off();
				} else {
					printlnA(F("Filter OFF on hold, pH or ORP active"));
					asm("NOP");
				}
			}
		}
		else if (parameters.filter_auto_mode == AUTO_TIMER_FCT_T)
		{

		}
	}
	return true;
}