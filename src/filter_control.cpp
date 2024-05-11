#include "autopool.h"

extern SoftTimer timer_pool;
uintptr_t filter_control_update_task;
bool filter_control_update(void *);
void disp_timer_prog_in_use(uint32_t timer_prog);

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
	#if HAS_FILTER_CONTROL
	filter_enter_mode(state_default.filter_mode);
	filter_enter_power_mode(state_default.filter_power);
	filter_control_update_task = timer_pool.every(FILTER_CONTROL_UPDATE_S * 1000, filter_control_update);
	mqtt_publish_filter_state();
	#else
	printlnA(F("!! Not Filter Control !!"));
	#endif
}

bool filter_control_update(void *)
{
	static int32_t counter_warm_up = 0;
	static int32_t counter_active_periodic = 0;
	uint32_t timer_prog_ok;
	uint8_t temperature_index;
	static unsigned long last_periodic_filter_time = 0;
	static uint32_t last_timer_prog;
	uint32_t timer_prog;
	uint8_t i, current_hour, current_minute;
	uint32_t done_filter_time, daily_fiter_time;

	printlnA("Filter control task Update");

	// In filter AUTO mode , get the hours when filtering will be activated
	if (state.filter_mode == FILTER_AUTO)
	{
		// Fixed dayly timer mode
		if (parameters.filter_auto_mode == AUTO_TIMER_PROG)
		{
			timer_prog = parameters.timer_prog;
		}
		// Temperature dependant timer prog
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
			timer_prog = parameters.timer_prog_temperature[temperature_index];
		}
		else
		{
			timer_prog = parameters.timer_prog;
		}

		// Timer prog OK ?
		timer_prog_ok = timer_prog & uint32_t(1 << rtc_get_hour());
		// If different timer prog scheme, update display
		if (timer_prog != last_timer_prog)
		{
			last_timer_prog = timer_prog;
			disp_timer_prog_in_use(timer_prog);
		}

		current_hour = rtc_get_hour();
		current_minute = rtc_get_minute();
		daily_fiter_time = 0;
		done_filter_time = 0;
		for (i = 0; i < 24; i++)
		{
			if (timer_prog & uint32_t(1 << i))
			{
				daily_fiter_time += 60;
				if (current_hour > i)
				{
					done_filter_time += 60;
				}
				else if (current_hour == i)
				{
					done_filter_time += current_minute;
				}
			}
		}
		disp_progress_filter.setValue((uint8_t)map(done_filter_time, 0, daily_fiter_time, 0, 100));
		switch (state.filter_control_state)
		{
		case FILTER_IDLE:
			if (timer_prog_ok)
			{
				filter_on();
				counter_warm_up = FILTER_CONTROL_WARM_UP_S / FILTER_CONTROL_UPDATE_S;
				state.filter_control_state = FILTER_AUTO_ACTIVE_WARM_UP;
				log_append((char*)"Filter started from timer prog");
				printA(F("Filter enter warm-up state for"));
				printlnA(counter_warm_up);
			}
			else if ((last_periodic_filter_time == 0) || (parameters.periodic_filter_time && (abs((int)(millis() - last_periodic_filter_time)) >= (parameters.periodic_filter_time * 1000 * 60))))
			{
				last_periodic_filter_time = millis();
				filter_on();
				counter_active_periodic = FILTER_CONTROL_PERIODIC_DURATION_S / FILTER_CONTROL_UPDATE_S;
				state.filter_control_state = FILTER_AUTO_ACTIVE_PERIODIC;
				log_append("Auto Periodic Filter active");
				printA(F("Activate periodic filter for "));
				printlnA(counter_active_periodic);
			}
			break;

		case FILTER_AUTO_ACTIVE_PERIODIC:
			// Periodc small filter timer each parameters.periodic_filter_time minutes to perform some measures (mainly temp)
			if (--counter_active_periodic <= 0)
			{
				state.filter_control_state = FILTER_IDLE;
				filter_off();
				log_append("Auto Periodic Filter Done");
				printlnA(F("End of periodic filter"));
			}
			break;

		case FILTER_AUTO_ACTIVE_WARM_UP:
			// Within this state auto corrections are not possible
			// Ensure min filtering time before allowing corrections...
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
			else // Timer prog completed, check if extended time is needed for ORP / pH injections
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
			// Filter time extended for ORP / pH on-going injections
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
	printlnA("Filter control task Done");
	return true;
}

void disp_timer_prog_in_use(uint32_t timer_prog)
{
	if ((timer_prog >> 0) & 0x01)
	{
		disp_timer_prog00.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog00.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 1) & 0x01)
	{
		disp_timer_prog01.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog01.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 2) & 0x01)
	{
		disp_timer_prog02.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog02.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 3) & 0x01)
	{
		disp_timer_prog03.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog03.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 4) & 0x01)
	{
		disp_timer_prog04.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog04.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 5) & 0x01)
	{
		disp_timer_prog05.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog05.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 6) & 0x01)
	{
		disp_timer_prog06.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog06.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 7) & 0x01)
	{
		disp_timer_prog07.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog07.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 8) & 0x01)
	{
		disp_timer_prog08.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog08.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 9) & 0x01)
	{
		disp_timer_prog09.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog09.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 10) & 0x01)
	{
		disp_timer_prog10.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog10.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 11) & 0x01)
	{
		disp_timer_prog11.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog11.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 12) & 0x01)
	{
		disp_timer_prog12.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog12.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 13) & 0x01)
	{
		disp_timer_prog13.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog13.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 14) & 0x01)
	{
		disp_timer_prog14.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog14.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 15) & 0x01)
	{
		disp_timer_prog15.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog15.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 16) & 0x01)
	{
		disp_timer_prog16.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog16.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 17) & 0x01)
	{
		disp_timer_prog17.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog17.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 18) & 0x01)
	{
		disp_timer_prog18.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog18.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 19) & 0x01)
	{
		disp_timer_prog19.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog19.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 20) & 0x01)
	{
		disp_timer_prog20.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog20.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 21) & 0x01)
	{
		disp_timer_prog21.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog21.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 22) & 0x01)
	{
		disp_timer_prog22.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog22.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
	if ((timer_prog >> 23) & 0x01)
	{
		disp_timer_prog23.setPic(ID_IMAGE_TIME_PROG_BLACK);
	}
	else
	{
		disp_timer_prog23.setPic(ID_IMAGE_TIME_PROG_WHITE);
	}
}
