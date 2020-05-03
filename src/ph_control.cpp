#include "autopool.h"

extern SoftTimer timer_pool;
bool ph_control_update(void *);
uintptr_t ph_control_update_task;

static int32_t counter_injection_on = 0;
static int32_t counter_injection_off = 0;

enum ph_control_state_t ph_control_state = PH_IDLE;
bool ph_minus_control_injection_timer_completed(void *);
bool ph_plus_control_injection_timer_completed(void *);

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
	printlnA(F("Ph Control Init"));
	disp_led_pump_ph_minus.setPic(ID_IMAGE_RED);
	ph_minus_enter_mode(PH_MINUS_OFF);
	ph_minus_enter_mode(PH_MINUS_AUTO);

#if HAS_PH_PLUS_PUMP
	ph_plus_enter_mode(PH_PLUS_OFF);
	ph_plus_enter_mode(PH_PLUS_AUTO);
#endif
	ph_control_update_task = timer_pool.every(PH_CONTROL_UPDATE_S * 1000, ph_control_update);
	mqtt_publish_ph_state();
}
void ph_control_stop(void)
{
	timer_pool.cancel(ph_control_update_task);
}

static void ph_minus_on(void)
{
	pump_ph_minus_on();
	if (state.ph_minus_pump != PUMP_ON)
	{
		state.ph_minus_pump = PUMP_ON;
		disp_led_pump_ph_minus.setPic(ID_IMAGE_GREEN);
		mqtt_publish_ph_state();
	}
}

static void ph_minus_off(void)
{
	pump_ph_minus_off();
	if (state.ph_minus_pump != PUMP_OFF)
	{
		state.ph_minus_pump = PUMP_OFF;
		disp_led_pump_ph_minus.setPic(ID_IMAGE_RED);
		mqtt_publish_ph_state();
	}
}

static void ph_plus_on(void)
{
	pump_ph_plus_on();
	if (state.ph_plus_pump != PUMP_ON)
	{
		state.ph_minus_pump = PUMP_ON;
		mqtt_publish_ph_state();
	}
}

static void ph_plus_off(void)
{
	pump_ph_plus_off();
	if (state.ph_plus_pump != PUMP_OFF)
	{
		state.ph_plus_pump = PUMP_OFF;
		mqtt_publish_ph_state();
	}
}

void ph_plus_enter_mode(enum ph_plus_mode_t ph_plus_mode)
{
	switch (ph_plus_mode)
	{
	case PH_PLUS_AUTO:
		control_ph_plus_auto();
		if (state.ph_plus_mode != PH_PLUS_AUTO)
		{
			state.ph_plus_mode = PH_PLUS_AUTO;
			state.ph_control_state = PH_IDLE;
			mqtt_publish_ph_state();
		}
		break;

	case PH_PLUS_OFF:
		control_ph_plus_off();
		if (state.ph_plus_mode != PH_PLUS_OFF)
		{
			state.ph_plus_mode = PH_PLUS_OFF;
			mqtt_publish_ph_state();
		}
		ph_plus_off();
		break;

	case PH_PLUS_ON:
		control_ph_plus_on();
		if (state.ph_plus_mode != PH_PLUS_ON)
		{
			state.ph_plus_mode = PH_PLUS_ON;
			mqtt_publish_ph_state();
		}
		ph_plus_on();
		break;

	default:
		break;
	}
}

void ph_minus_enter_mode(enum ph_minus_mode_t ph_minus_mode)
{
	switch (ph_minus_mode)
	{
	case PH_PLUS_AUTO:
		control_ph_minus_auto();
		if (state.ph_minus_mode != PH_MINUS_AUTO)
		{
			state.ph_minus_mode = PH_MINUS_AUTO;
			mqtt_publish_ph_state();
		}
		break;

	case PH_PLUS_OFF:
		control_ph_minus_off();
		if (state.ph_minus_mode != PH_MINUS_OFF)
		{
			state.ph_minus_mode = PH_MINUS_OFF;
			mqtt_publish_ph_state();
		}
		ph_minus_off();
		break;

	case PH_PLUS_ON:
		control_ph_minus_on();
		if (state.ph_minus_mode != PH_MINUS_ON)
		{
			state.ph_minus_mode = PH_MINUS_ON;
			mqtt_publish_ph_state();
		}
		ph_minus_on();
		break;

	default:
		break;
	}
}

bool ph_minus_auto_correction_possible(void)
{
	bool is_possible = true;

	if (state.orp_control_state != ORP_IDLE)
	{
		is_possible = false;
	}
	if (state.ph_minus_mode != PH_MINUS_AUTO)
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
	if (measures.level_ph_minus == false)
	{
		is_possible = false;
	}
	if (measures.daily_ml_ph_minus > parameters.phm_max_day)
	{
		is_possible = false;
	}
	return is_possible;
}

bool ph_plus_auto_correction_possible(void)
{
#if HAS_PH_PLUS_PUMP
	bool is_possible = true;
	if (state.orp_control_state != ORP_IDLE)
	{
		is_possible = false;
	}
	if (state.ph_plus_mode != PH_PLUS_AUTO)
	{
		is_possible = false;
	}
	if (measures.level_ph_plus == false)
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
	if (measures.daily_ml_ph_plus > parameters.php_max_day)
	{
		is_possible = false;
	}
	return is_possible;
#else
	return false;
#endif
}

enum correction_need_t ph_plus_correction_needed(void)
{
	float delta = parameters.target_ph - measures.ph;

	if (delta > (3 * parameters.delta_ph))
	{
		return STEP4_CORRECTION;
	}
	else if (delta > (2 * parameters.delta_ph))
	{
		return STEP3_CORRECTION;
	}
	else if (delta > (1 * parameters.delta_ph))
	{
		return STEP2_CORRECTION;
	}
	else if (delta > 0)
	{
		return STEP2_CORRECTION;
	}
	else
	{
		return NO_CORRECTION;
	}
}

enum correction_need_t ph_minus_correction_needed(void)
{
	float delta = measures.ph - parameters.target_ph;

	if (delta > (3 * parameters.delta_ph))
	{
		return STEP4_CORRECTION;
	}
	else if (delta > (2 * parameters.delta_ph))
	{
		return STEP3_CORRECTION;
	}
	else if (delta > (parameters.delta_ph))
	{
		return STEP2_CORRECTION;
	}
	else if (delta > 0)
	{
		return STEP1_CORRECTION;
	}
	else
	{
		return NO_CORRECTION;
	}
}

bool ph_control_update(void *)
{
	printlnA("pH control task");
	disp_ph_state_to_display();
	switch (state.ph_control_state)
	{
	case PH_IDLE:
		if (ph_minus_auto_correction_possible())
		{
			switch (ph_minus_correction_needed())
			{
			case NO_CORRECTION:
				// Ok no correction needed
				break;

			case STEP1_CORRECTION:
				printlnA("PH MINUS Step 1 correction needed");
				log_append("PH MINUS Step 1 correction needed");
				state.ph_control_state = PH_MINUS_INJECTION_ON;
				// 20 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 20) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 80) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP2_CORRECTION:
				printlnA("PH MINUS Step 2 correction needed");
				log_append("PH MINUS Step 2 correction needed");
				state.ph_control_state = PH_MINUS_INJECTION_ON;
				// 50 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 50) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 50) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP3_CORRECTION:
				printlnA("PH MINUS Step 3 correction needed");
				log_append("PH MINUS Step 3 correction needed");
				state.ph_control_state = PH_MINUS_INJECTION_ON;
				// 75 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 75) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 25) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP4_CORRECTION:
				printlnA("PH MINUS Step 4 correction needed");
				log_append("PH MINUS Step 4 correction needed");
				state.ph_control_state = PH_MINUS_INJECTION_ON;
				// 100% injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 100) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = 0;
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			default:
				break;
			}
		}
#if HAS_PH_PLUS_PUMP
		else if (ph_plus_auto_correction_possible())
		{
			switch (ph_plus_correction_needed())
			{
			case NO_CORRECTION:
				// Ok no correction needed
				break;

			case STEP1_CORRECTION:
				printlnA("PH PLUS Step 1 correction needed");
				log_append("PH PLUS Step 1 correction needed");
				state.ph_control_state = PH_PLUS_INJECTION_ON;
				// 20 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 20) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 80) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP2_CORRECTION:
				printlnA("PH PLUS Step 2 correction needed");
				log_append("PH PLUS Step 2 correction needed");
				// 50 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 50) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 50) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP3_CORRECTION:
				printlnA("PH PLUS Step 3 correction needed");
				log_append("PH PLUS Step 3 correction needed");
				state.ph_control_state = PH_PLUS_INJECTION_ON;
				// 75 % injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 75) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = ((PH_REGULATION_CYCLE_S * 25) / 100) / PH_CONTROL_UPDATE_S;
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP4_CORRECTION:
				printlnA("PH PLUS Step 4 correction needed");
				log_append("PH PLUS Step 4 correction needed");
				state.ph_control_state = PH_PLUS_INJECTION_ON;
				// 100% injection time
				counter_injection_on = ((PH_REGULATION_CYCLE_S * 100) / 100) / PH_CONTROL_UPDATE_S;
				counter_injection_off = 0;
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			default:
				break;
			}
		}
#endif
		else
		{
			printlnA("pH correction not possible")
		}
		break;

	case PH_MINUS_INJECTION_ON:
		measures.daily_ml_ph_minus += (float)((double)(parameters.flow_ph_minus / 60.0) * (PH_CONTROL_UPDATE_S));
		if (--counter_injection_on <= 0)
		{
			log_append("pH Minus Injection time Completed");
			state.ph_control_state = PH_MINUS_INJECTION_OFF;
			ph_minus_off();
		}
		if (ph_minus_auto_correction_possible() == false)
		{
			printlnA("Need to stop pH Minus injection");
			log_append("Need to stop pH Minus injection");
			ph_minus_off();
			state.ph_control_state = PH_IDLE;
		}
		break;

	case PH_PLUS_INJECTION_ON:
		measures.daily_ml_ph_plus += (float)((double)(parameters.flow_ph_plus / 60.0) * (PH_CONTROL_UPDATE_S));
		if (--counter_injection_on <= 0)
		{
			log_append("pH Plus Injection time Completed");
			state.ph_control_state = PH_PLUS_INJECTION_OFF;
			ph_plus_off();
		}
		if (ph_plus_auto_correction_possible() == false)
		{
			printlnA("Need to stop pH Plus injection");
			log_append("Need to stop pH Plus injection");
			ph_plus_off();
			state.ph_control_state = PH_IDLE;
		}
		break;

	case PH_MINUS_INJECTION_OFF:
		if (--counter_injection_off <= 0)
		{
			log_append("ph Minus Cycle Completed");
			state.ph_control_state = PH_IDLE;
		}
		if (ph_minus_auto_correction_possible() == false)
		{
			printlnA("Need to stop active ORP correction");
			log_append("Need to stop active ORP correction");
			state.ph_control_state = PH_IDLE;
		}
		break;

	case PH_PLUS_INJECTION_OFF:
		if (--counter_injection_off <= 0)
		{
			log_append("ph Plus Cycle Completed");
			state.ph_control_state = PH_IDLE;
		}
		if (ph_plus_auto_correction_possible() == false)
		{
			printlnA("Need to stop active ORP correction");
			log_append("Need to stop active ORP correction");
			state.ph_control_state = PH_IDLE;
		}
		break;
	default:
		break;
	}
	return true;
}
