#include "parameters.h"
#include "ph_control.h"
#include "config.h"
#include "measures.h"
#include "parameters.h"
#include <SerialDebug.h>
#include "display_ctrl.h"
#include "state.h"
#include "soft_timer.h"

extern SoftTimer timer_pool;
bool ph_control_update(void *);
uintptr_t ph_control_update_task;
uintptr_t ph_control_timer_injection_task;

enum ph_control_state_t ph_control_state = PH_IDLE;
bool ph_minus_control_injection_timer_completed(void *);
bool ph_plus_control_injection_timer_completed(void *);

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
	printlnA(F("Ph Control Init"));
	ph_minus_enter_mode(PH_MINUS_OFF);
	ph_minus_enter_mode(PH_MINUS_AUTO);
#if HAS_PH_PLUS_PUMP
	ph_plus_enter_mode(PH_PLUS_OFF);
	ph_plus_enter_mode(PH_PLUS_AUTO);
#else
	control_ph_plus_disable_controls();
#endif
	ph_control_update_task = timer_pool.every(PH_CONTROL_UPDATE_MS, ph_control_update);
	mqtt_publish_ph_state();
}

static void ph_minus_on(void)
{
	pump_ph_minus_on();
	if (state.ph_minus_pump != PUMP_ON)
	{
		state.ph_minus_pump = PUMP_ON;
		mqtt_publish_ph_state();
	}
}

static void ph_minus_off(void)
{
	pump_ph_minus_off();
	if (state.ph_minus_pump != PUMP_OFF)
	{
		state.ph_minus_pump = PUMP_OFF;
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
	return is_possible;
}

bool ph_plus_auto_correction_possible(void)
{
#if HAS_PH_PLUS_PUMP
	bool is_possible = true;
	if (state.ph_control_state != PH_IDLE)
	{
		is_possible = false;
	}
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

void ph_control_loop(void)
{
}

bool ph_minus_control_injection_timer_completed(void *)
{
	mqtt_publish_log("PH Minus Injection Completed");
	state.ph_control_state = PH_IDLE;
	ph_minus_off();
	return false;
}

bool ph_plus_control_injection_timer_completed(void *)
{
	mqtt_publish_log("PH Plus Injection Completed");
	state.ph_control_state = PH_IDLE;
	ph_plus_off();
	return false;
}

bool ph_control_update(void *)
{
	printlnA("pH control task") switch (state.ph_control_state)
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
				mqtt_publish_log("PH MINUS Step 1 correction needed");
				state.ph_control_state = PH_MINUS_ACTIVE_CORRECTION;
				ph_control_timer_injection_task = timer_pool.in(((10 * 60 * 1000) * 20) / 100, ph_minus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP2_CORRECTION:
				printlnA("PH MINUS Step 2 correction needed");
				mqtt_publish_log("PH MINUS Step 2 correction needed");
				state.ph_control_state = PH_MINUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 50) / 100, ph_minus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP3_CORRECTION:
				printlnA("PH MINUS Step 3 correction needed");
				mqtt_publish_log("PH MINUS Step 3 correction needed");
				state.ph_control_state = PH_MINUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 75) / 100, ph_minus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_minus_on();
				break;

			case STEP4_CORRECTION:
				printlnA("PH MINUS Step 4 correction needed");
				mqtt_publish_log("PH MINUS Step 4 correction needed");
				state.ph_control_state = PH_MINUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 100) / 100, ph_minus_control_injection_timer_completed);
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
				mqtt_publish_log("PH PLUS Step 1 correction needed");
				state.ph_control_state = PH_PLUS_ACTIVE_CORRECTION;
				ph_control_timer_injection_task = timer_pool.in(((10 * 60 * 1000) * 20) / 100, ph_plus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP2_CORRECTION:
				printlnA("PH PLUS Step 2 correction needed");
				mqtt_publish_log("PH PLUS Step 2 correction needed");
				state.ph_control_state = PH_PLUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 50) / 100, ph_plus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP3_CORRECTION:
				printlnA("PH PLUS Step 3 correction needed");
				mqtt_publish_log("PH PLUS Step 3 correction needed");
				state.ph_control_state = PH_PLUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 75) / 100, ph_plus_control_injection_timer_completed);
				mqtt_publish_ph_state();
				ph_plus_on();
				break;

			case STEP4_CORRECTION:
				printlnA("PH PLUS Step 4 correction needed");
				mqtt_publish_log("PH PLUS Step 4 correction needed");
				state.ph_control_state = PH_PLUS_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 100) / 100, ph_plus_control_injection_timer_completed);
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

	case PH_MINUS_ACTIVE_CORRECTION:
		if (ph_minus_auto_correction_possible() == false)
		{
			printlnA("Need to stop active PH Minus correction");
			mqtt_publish_log("Need to stop active PH Minus correction");
			timer_pool.cancel(ph_control_timer_injection_task);
			ph_minus_control_injection_timer_completed(NULL);
		}
		break;

	case PH_PLUS_ACTIVE_CORRECTION:
		if (ph_plus_auto_correction_possible() == false)
		{
			printlnA("Need to stop active PH Plus correction");
			mqtt_publish_log("Need to stop active PH Plus correction");
			timer_pool.cancel(ph_control_timer_injection_task);
			ph_plus_control_injection_timer_completed(NULL);
		}
		break;

	default:
		break;
	}
	return true;
}