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

static int32_t counter_injection_on = 0;
static int32_t counter_injection_off = 0;

bool orp_control_update(void *);


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
}

bool orp_control_update(void *)
{
	printlnA("ORP task update");
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
				mqtt_publish_log("ORP Step 1 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 20 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_MS * 20) / 100) / ORP_CONTROL_UPDATE_MS;
				counter_injection_off = (ORP_REGULATION_CYCLE_MS - counter_injection_on) / ORP_CONTROL_UPDATE_MS;
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP2_CORRECTION:
				printlnA("ORP Step 2 correction needed");
				mqtt_publish_log("ORP Step 2 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 50 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_MS * 50) / 100) / ORP_CONTROL_UPDATE_MS;
				counter_injection_off = (ORP_REGULATION_CYCLE_MS - counter_injection_on) / ORP_CONTROL_UPDATE_MS;
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP3_CORRECTION:
				printlnA("ORP Step 3 correction needed");
				mqtt_publish_log("ORP Step 3 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 75 % injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_MS * 75) / 100) / ORP_CONTROL_UPDATE_MS;
				counter_injection_off = (ORP_REGULATION_CYCLE_MS - counter_injection_on) / ORP_CONTROL_UPDATE_MS;
				mqtt_publish_orp_state();
				cl_on();
				break;
			case STEP4_CORRECTION:
				printlnA("ORP Step 4 correction needed");
				mqtt_publish_log("ORP Step 4 correction needed");
				state.orp_control_state = ORP_INJECTION_ON;
				// 100% injection time
				counter_injection_on = ((ORP_REGULATION_CYCLE_MS * 100) / 100) / ORP_CONTROL_UPDATE_MS;
				counter_injection_off = (ORP_REGULATION_CYCLE_MS - counter_injection_on) / ORP_CONTROL_UPDATE_MS;
				mqtt_publish_orp_state();
				cl_on();
				break;

			default:
				break;
			}
		}
		else
		{
			printlnA("ORP correction not possible")
		}
		break;

	case ORP_INJECTION_ON:
		if (--counter_injection_on <= 0)
		{
			mqtt_publish_log("ORP Injection time Completed");
			state.orp_control_state = ORP_INJECTION_OFF;
			cl_off();
		}
		if (orp_auto_correction_possible() == false)
		{
			printlnA("Need to stop active ORP correction");
			mqtt_publish_log("Need to stop active ORP correction");
			state.orp_control_state = ORP_IDLE;
		}
		break;

	case ORP_INJECTION_OFF:
		if (--counter_injection_off <= 0)
		{
			mqtt_publish_log("ORP Cycle Completed");
			state.orp_control_state = ORP_INJECTION_OFF;
			cl_off();
			state.orp_control_state = ORP_IDLE;
		}

	default:
		break;
	}
	return true;
}
