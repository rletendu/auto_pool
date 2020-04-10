#include "parameters.h"
#include "orp_control.h"
#include "measures.h"
#include "parameters.h"
#include "config.h"
#include <SerialDebug.h>
#include "display_ctrl.h"
#include <soft_timer.h>
#include "state.h"

extern SoftTimer timer_pool;
uintptr_t orp_control_update_task;
bool orp_control_update(void *);
bool orp_control_injection_timer_completed(void *);

static void cl_on(void)
{
	pump_cl_on();
	if (state.cl_pump != PUMP_ON)
	{
		state.cl_pump = PUMP_ON;
		mqtt_publish_orp_state();
	}
}

static void cl_off(void)
{
	pump_cl_off();
	if (state.cl_pump != PUMP_OFF)
	{
		state.cl_pump = PUMP_OFF;
		mqtt_publish_orp_state();
	}
}

void cl_enter_mode(enum cl_mode_t orp_mode)
{
	switch (orp_mode)
	{
	case CL_AUTO:
		control_cl_auto();
		if (state.cl_mode != CL_AUTO)
		{
			state.cl_mode = CL_AUTO;
			mqtt_publish_orp_state();
		}
		break;

	case CL_OFF:
		control_cl_off();
		if (state.cl_mode != CL_OFF)
		{
			state.cl_mode = CL_OFF;
			mqtt_publish_orp_state();
		}
		cl_off();
		break;

	case CL_ON:
		control_cl_on();
		if (state.cl_mode != CL_ON)
		{
			state.cl_mode = CL_ON;
			mqtt_publish_orp_state();
		}
		cl_on();
		break;

	default:
		break;
	}
}

void orp_control_init(void)
{
	printlnA(F("ORP Control Init"));
	state.orp_control_state = ORP_IDLE;
	cl_enter_mode(CL_OFF);
	cl_enter_mode(CL_AUTO);
	orp_control_update_task = timer_pool.every(ORP_CONTROL_UPDATE_MS, orp_control_update);
	mqtt_publish_orp_state();
}

bool orp_control_injection_timer_completed(void *)
{
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
	if (state.cl_mode != CL_AUTO)
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
	return is_possible;
}

void orp_control_loop(void)
{
	state.orp_control_state = ORP_IDLE;
	cl_off();
}

bool orp_control_update(void *)
{
	uint8_t h;
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
				state.orp_control_state = ORP_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 20) / 100, orp_control_injection_timer_completed);
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP2_CORRECTION:
				printlnA("ORP Step 2 correction needed");
				state.orp_control_state = ORP_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 50) / 100, orp_control_injection_timer_completed);
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP3_CORRECTION:
				printlnA("ORP Step 3 correction needed");
				state.orp_control_state = ORP_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 75) / 100, orp_control_injection_timer_completed);
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP4_CORRECTION:
				printlnA("ORP Step 4 correction needed");
				state.orp_control_state = ORP_ACTIVE_CORRECTION;
				timer_pool.in(((10 * 60 * 1000) * 100) / 100, orp_control_injection_timer_completed);
				mqtt_publish_orp_state();
				cl_on();
				break;

			default:
				break;
			}
		}
		else
		{
		}
		break;

	case ORP_ACTIVE_CORRECTION:
		if (orp_auto_correction_possible() == false)
		{
			printlnA("Need to stop active ORP correction");
		}
		else
		{
		}
		break;

	default:
		break;
	}
	return true;
}
