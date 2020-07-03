#include "autopool.h"

extern SoftTimer timer_pool;
uintptr_t filter_control_update_task;
bool filter_control_update(void *);

static void filter_on(void)
{
	pump_filtration_on();
	if (state.filter_pump != PUMP_ON)
	{
		state.filter_time_pump_on = millis();
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
		state.filter_time_pump_off = millis();
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
		display_control_filter_auto();
		if (state.filter_mode != FILTER_AUTO)
		{
			filter_off();
			state.filter_mode = FILTER_AUTO;
			state.filter_control_state = FILTER_IDLE;
			mqtt_publish_filter_state();
		}
		break;

	case FILTER_OFF:
		display_control_filter_off();
		if (state.filter_mode != FILTER_OFF)
		{
			state.filter_mode = FILTER_OFF;
			mqtt_publish_filter_state();
		}
		filter_off();
		break;

	case FILTER_ON:
		display_control_filter_on();
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

void filter_enter_power_mode(enum filter_power_t filter_power)
{
#if HAS_FILTER_PWR_CTRL
	switch (filter_power)
	{
	case FILTER_POWER_FULL:
		display_control_filter_pwr_full();
		if (state.filter_power != FILTER_POWER_FULL)
		{
			state.filter_power = FILTER_POWER_FULL;
			mqtt_publish_filter_state();
		}
		pump_filtration_pwr_full();
		break;

	case FILTER_POWER_REG:
		display_control_filter_pwr_reg();
		if (state.filter_power != FILTER_POWER_REG)
		{
			state.filter_power = FILTER_POWER_REG;
			mqtt_publish_filter_state();
		}
		pump_filtration_pwr_reg();
		break;

	default:
		break;
	}
#endif
}

void filter_control_init(void)
{
	printlnA(F("Filter Control Init"));
	disp_led_pump_water.setPic(ID_IMAGE_RED);
	filter_enter_mode(state_default.filter_mode);
	filter_enter_power_mode(state_default.filter_power);
	filter_control_update_task = timer_pool.every(FILTER_CONTROL_UPDATE_S * 1000, filter_control_update);
	mqtt_publish_filter_state();
}

bool filter_control_update(void *)
{
	static int32_t counter_warm_up = 0;
	static int32_t counter_active_periodic = 0;
	uint32_t timer_prog_ok;
	uint8_t temperature_index;
	static unsigned long last_periodic_filter_time = 0;

	if (state.filter_mode == FILTER_AUTO)
	{
		if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
		{
			timer_prog_ok = parameters.timer_prog & uint32_t(1 << rtc_get_hour());
		}
		else if (parameters.filter_auto_mode == AUTO_TIMER_FCT_T)
		{
			if (measures.day_max_water_temperature <= PARAM_FIRST_TEMP_TIMER_PROG)
			{
				temperature_index = 0;
			}
			else if ((measures.day_max_water_temperature >= (PARAM_FIRST_TEMP_TIMER_PROG + (PARAM_NB_TEMP_TIMER_PROG - 1) * PARAM_STEP_TEMP_TIMER_PROG)))
			{
				temperature_index = PARAM_NB_TEMP_TIMER_PROG - 1;
			}
			else
			{
				temperature_index = (measures.day_max_water_temperature - PARAM_FIRST_TEMP_TIMER_PROG) / PARAM_STEP_TEMP_TIMER_PROG;
			}
			timer_prog_ok = parameters.timer_prog_temperature[temperature_index] & uint32_t(1 << rtc_get_hour());
		}
		else
		{
			timer_prog_ok = parameters.timer_prog & uint32_t(1 << rtc_get_hour());
		}

		switch (state.filter_control_state)
		{
		case FILTER_IDLE:
			if (timer_prog_ok)
			{
				filter_on();
				counter_warm_up = FILTER_CONTROL_WARM_UP_S / FILTER_CONTROL_UPDATE_S;
				state.filter_control_state = FILTER_AUTO_ACTIVE_WARM_UP;
				log_append("Filter started from timer prog");
				printA(F("Filter enter warm-up state for"));
				printlnA(counter_warm_up);
			}
			else if (parameters.periodic_filter_time && (abs(millis() - last_periodic_filter_time) >= (parameters.periodic_filter_time*1000*60)))
			{
				last_periodic_filter_time = millis();
				filter_on();
				counter_active_periodic = FILTER_CONTROL_PERIODIC_DURATION_S / FILTER_CONTROL_UPDATE_S;
				state.filter_control_state = FILTER_AUTO_ACTIVE_PERIODIC;
				printA(F("Activate periodic filter for "));
				printlnA(counter_active_periodic);
			}
			break;

		case FILTER_AUTO_ACTIVE_PERIODIC:
			if (--counter_active_periodic <= 0)
			{
				state.filter_control_state = FILTER_IDLE;
				filter_off();
				printlnA(F("End of periodic filter"));
			}
			break;

		case FILTER_AUTO_ACTIVE_WARM_UP:
			if (--counter_warm_up <= 0)
			{
				state.filter_control_state = FILTER_AUTO_ACTIVE;
				printlnA(F("Filter enter FILTER_AUTO_ACTIVE state"));
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
	else
	{
	}
	return true;
}