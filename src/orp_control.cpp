#include "autopool.h"

extern SoftTimer timer_pool;
uintptr_t orp_control_update_task;

static int32_t counter_injection_on = 0;
static int32_t counter_injection_off = 0;

bool orp_control_update(void *);

static void orp_on(void)
{
	pump_orp_on();
	if (state.orp_pump != PUMP_ON)
	{
		state.orp_time_pump_on = millis();
		state.orp_pump = PUMP_ON;
		disp_led_pump_cl.setPic(ID_IMAGE_GREEN);
		mqtt_publish_orp_state();
	}
}

static void orp_off(void)
{
	pump_orp_off();
	if (state.orp_pump != PUMP_OFF)
	{
		state.orp_time_pump_off = millis();
		state.orp_pump = PUMP_OFF;
		disp_led_pump_cl.setPic(ID_IMAGE_RED);
		mqtt_publish_orp_state();
	}
}

void orp_enter_mode(enum orp_mode_t orp_mode)
{
	switch (orp_mode)
	{
	case ORP_AUTO:
		display_control_cl_auto();
		if (state.orp_mode != ORP_AUTO)
		{
			state.orp_mode = ORP_AUTO;
			state.orp_control_state = ORP_IDLE;
			mqtt_publish_orp_state();
		}
		break;

	case ORP_OFF:
		display_control_cl_off();
		if (state.orp_mode != ORP_OFF)
		{
			state.orp_mode = ORP_OFF;
			mqtt_publish_orp_state();
		}
		orp_off();
		break;

	case ORP_ON:
		display_control_cl_on();
		if (state.orp_mode != ORP_ON)
		{
			state.orp_mode = ORP_ON;
			mqtt_publish_orp_state();
		}
		orp_on();
		break;

	default:
		break;
	}
}

void orp_control_init(void)
{
	printlnA(F("ORP Control Init"));
	disp_led_pump_cl.setPic(ID_IMAGE_RED);
	state.orp_control_state = ORP_IDLE;
	orp_enter_mode(state_default.orp_mode);
	orp_control_update_task = timer_pool.every(ORP_CONTROL_UPDATE_S * 1000, orp_control_update);
	mqtt_publish_orp_state();
}

void orp_control_stop(void)
{
	timer_pool.cancel(orp_control_update_task);
}

enum correction_need_t orp_correction_needed(void)
{
	float delta_orp = (parameters.target_orp - measures.orp);

	if (delta_orp > (3 * parameters.delta_orp))
	{
		return STEP4_CORRECTION;
	}
	else if (delta_orp > (2 * parameters.delta_orp))
	{
		return STEP3_CORRECTION;
	}
	else if (delta_orp > (1 * parameters.delta_orp))
	{
		return STEP2_CORRECTION;
	}
	else if (delta_orp > 0)
	{
		return STEP1_CORRECTION;
	}
	else
	{
		return NO_CORRECTION;
	}
}

bool orp_auto_correction_possible(void)
{
	bool is_possible = true;

	if (state.ph_control_state != PH_IDLE)
	{
		is_possible = false;
	}
	if (state.orp_mode != ORP_AUTO)
	{
		is_possible = false;
	}
	if (state.filter_mode != FILTER_AUTO)
	{
		is_possible = false;
	}
	if (state.filter_pump == PUMP_OFF)
	{
		is_possible = false;
	}
	if (measures.level_cl == false)
	{
		is_possible = false;
	}
	if (measures.daily_ml_orp > parameters.cl_max_day)
	{
		is_possible = false;
	}
	if (state.filter_control_state != FILTER_AUTO_ACTIVE)
	{
		is_possible = false;
	}
	return is_possible;
}

bool orp_control_update(void *)
{
	printlnA("ORP task update");
	disp_orp_state_to_display();
	switch (state.orp_control_state)
	{
	case ORP_IDLE:
		if (orp_auto_correction_possible())
		{
			switch (orp_correction_needed())
			{
			case NO_CORRECTION:
				// Ok no correction needed
				break;
			case STEP1_CORRECTION:
				printlnA("ORP Step 1 correction needed");
				log_append("ORP Step 1 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 20 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_S * 20) / 100) / ORP_CONTROL_UPDATE_S;
				counter_injection_off = ((ORP_REGULATION_CYCLE_S * 80) / 100) / ORP_CONTROL_UPDATE_S;
				mqtt_publish_orp_state();
				orp_on();
				break;
			case STEP2_CORRECTION:
				printlnA("ORP Step 2 correction needed");
				log_append("ORP Step 2 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 50 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_S * 50) / 100) / ORP_CONTROL_UPDATE_S;
				counter_injection_off = ((ORP_REGULATION_CYCLE_S * 50) / 100) / ORP_CONTROL_UPDATE_S;
				mqtt_publish_orp_state();
				orp_on();
				break;
			case STEP3_CORRECTION:
				printlnA("ORP Step 3 correction needed");
				log_append("ORP Step 3 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 75 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_S * 75) / 100) / ORP_CONTROL_UPDATE_S;
				counter_injection_off = ((ORP_REGULATION_CYCLE_S * 25) / 100) / ORP_CONTROL_UPDATE_S;
				mqtt_publish_orp_state();
				orp_on();
				break;
			case STEP4_CORRECTION:
				printlnA("ORP Step 4 correction needed");
				log_append("ORP Step 4 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 100% injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_S * 100) / 100) / ORP_CONTROL_UPDATE_S;
				counter_injection_off = 0;
				mqtt_publish_orp_state();
				orp_on();
				break;

			default:
				break;
			}
		}
		else
		{
			printlnA("ORP correction not possible")
			if (state.orp_mode != ORP_ON) {
				printlnA("Inc ORP daily injected (ORP ON)")
				// Inc daily ml injected while in force ON mode
				measures.daily_ml_orp += (float)((double)(parameters.flow_cl / 60.0) * (ORP_CONTROL_UPDATE_S));
				if (measures.daily_ml_orp > parameters.cl_max_day) {
					printlnA("Max ORP daily reached, forcing ORP OFF");
					log_append("Need FORCE ORP OFF (max daily)");
					orp_enter_mode(ORP_OFF);
					beep(10);
					delay(10);
					beep(10);
					delay(10);
					beep(10);
				}
			}
		}
		break;

	case ORP_INJECTION_ON:
		measures.daily_ml_orp += (float)((double)(parameters.flow_cl / 60.0) * (ORP_CONTROL_UPDATE_S));
		if (--counter_injection_on <= 0)
		{
			log_append("ORP Injection time Completed");
			state.orp_control_state = ORP_INJECTION_OFF;
			orp_off();
		}
		if (orp_auto_correction_possible() == false)
		{
			printlnA("Need to stop ORP Injection");
			log_append("Need to stop active ORP correction");
			state.orp_control_state = ORP_IDLE;
			orp_off();
		}
		break;

	case ORP_INJECTION_OFF:
		if (--counter_injection_off <= 0)
		{
			log_append("ORP Cycle Completed");
			state.orp_control_state = ORP_IDLE;
		}
		if (orp_auto_correction_possible() == false)
		{
			printlnA("Need to stop active ORP correction");
			log_append("Need to stop active ORP correction");
			state.orp_control_state = ORP_IDLE;
		}
		break;

	default:
		break;
	}
	return true;
}
