#include "parameters.h"
#include "ph_control.h"
#include "config.h"
#include "measures.h"
#include "parameters.h"
#include <SerialDebug.h>
#include "display_ctrl.h"
#include "state.h"

enum ph_control_state_t ph_control_state = PH_IDLE;

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

bool ph_auto_correction_possible(void)
{
	bool is_possible = true;
#if HAS_PH_PLUS_PUMP
	if (state.ph_plus_mode != PH_PLUS_AUTO)
	{
		is_possible = false;
	}
	if (measures.level_ph_plus == false)
	{
		is_possible = false;
	}
#endif
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

enum correction_need_t ph_plus_correction_needed(void)
{
	float delta = (parameters.target_ph - measures.ph);
	if (delta < parameters.delta_ph)
	{
		return NO_CORRECTION;
	}
	else if (delta > (1 * parameters.delta_ph))
	{
		return STEP1_CORRECTION;
	}
	else if (delta > (2 * parameters.delta_ph))
	{
		return STEP2_CORRECTION;
	}
	else if (delta > (3 * parameters.delta_ph))
	{
		return STEP3_CORRECTION;
	}
	else if (delta > (4 * parameters.delta_ph))
	{
		return STEP4_CORRECTION;
	}
}

enum correction_need_t ph_minus_correction_needed(void)
{
	float delta = (parameters.target_ph - measures.ph);
	if (delta < parameters.delta_ph)
	{
		return NO_CORRECTION;
	}
	else if (delta > (1 * parameters.delta_ph))
	{
		return STEP1_CORRECTION;
	}
	else if (delta > (2 * parameters.delta_ph))
	{
		return STEP2_CORRECTION;
	}
	else if (delta > (3 * parameters.delta_ph))
	{
		return STEP3_CORRECTION;
	}
	else if (delta > (4 * parameters.delta_ph))
	{
		return STEP4_CORRECTION;
	}
}

void ph_control_init(void)
{
	ph_control_state = PH_IDLE;
	printlnA(F("Ph Control Init"));
	ph_minus_enter_mode(PH_MINUS_OFF);
	ph_minus_enter_mode(PH_MINUS_AUTO);
	ph_plus_enter_mode(PH_PLUS_OFF);
	ph_plus_enter_mode(PH_PLUS_AUTO);
	mqtt_publish_ph_state();
}

void ph_control_loop(void)
{
}