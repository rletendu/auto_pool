#include "autopool.h"

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
	filter_control_update_task = timer_pool.every(FILTER_CONTROL_UPDATE_S*1000, filter_control_update);
	mqtt_publish_filter_state();
}

bool filter_control_update(void *)
{

	uint32_t timer_prog_ok;

	timer_prog_ok = parameters.timer_prog & uint32_t(1 << rtc_get_hour());
	if (state.filter_mode == FILTER_AUTO)
	{
		switch (state.filter_control_state)
		{
		case FILTER_IDLE:
			if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
			{
				if (timer_prog_ok)
				{
					filter_on();
					state.filter_control_state = FILTER_AUTO_ACTIVE;
					log_append("Filter started from timer prog");
				}
			}
			else if (parameters.filter_auto_mode == AUTO_TIMER_FCT_T)
			{
#warning TODO not implemented yet
			}
			break;
		case FILTER_AUTO_ACTIVE:
			if (timer_prog_ok)
			{
				// Ok Keep going;
			}
			else
			{
				if (state.ph_control_state == PH_IDLE && state.orp_control_state == ORP_IDLE)
				{
					state.filter_control_state = FILTER_IDLE;
					filter_off();
					log_append("Filter stoped from timer prog");
				}
				else
				{
					state.filter_control_state = FILTER_AUTO_ACTIVE_EXTENDED;
					printlnA(F("Filter OFF on hold, pH or ORP active"));
					log_append("Filter time Extended (ORP/PH)");
				}
			}
			break;
		case FILTER_AUTO_ACTIVE_EXTENDED:
			if (state.ph_control_state == PH_IDLE && state.orp_control_state == ORP_IDLE)
			{
				state.filter_control_state = FILTER_IDLE;
				filter_off();
				log_append("Filter Extended stoped");
			}
			else
			{
				// Continue extension time
			}
			break;
		default:
			break;
		}
	}
	return true;
}