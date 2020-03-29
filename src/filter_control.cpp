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
	state.filter_pump = PUMP_ON;
}

static void filter_off(void)
{
	pump_filtration_off();
	state.filter_pump = PUMP_OFF;
}

void filter_enter_mode(enum filter_mode_t filter_mode)
{
	switch (filter_mode)
	{
	case FILTER_AUTO:
		control_filter_auto();
		state.filter_mode = FILTER_AUTO;
		break;

	case FILTER_OFF:
		control_filter_off();
		state.filter_mode = FILTER_OFF;
		filter_off();
		break;

	case FILTER_ON:
		control_filter_on();
		state.filter_mode = FILTER_ON;
		filter_on();
		break;

	default:
		break;
	}
	
}

void filter_control_init(void)
{
	printlnA(F("Filter Control Init"));
	filter_enter_mode(FILTER_OFF);
	filter_enter_mode(FILTER_AUTO);
	filter_control_update_task = timer_pool.every(FILTER_CONTROL_UPDATE_MS, filter_control_update);
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
				filter_off();
			}
		}
		else if (parameters.filter_auto_mode == AUTO_TIMER_FCT_T)
		{
		}
	}
	return true;
}

void filter_control_loop(void)
{

}