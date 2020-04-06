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

static void cl_on(void)
{
	pump_cl_on();
	if (state.cl_pump != PUMP_ON)
	{
		mqtt_publish_orp_state();
	}
	state.cl_pump = PUMP_ON;
}

static void cl_off(void)
{
	pump_cl_off();
	if (state.cl_pump != PUMP_OFF)
	{
		mqtt_publish_orp_state();
	}
	state.cl_pump = PUMP_OFF;
}

void cl_enter_mode(enum cl_mode_t orp_mode)
{
	switch (orp_mode)
	{
	case CL_AUTO:
		control_cl_auto();
		if (state.cl_mode != CL_AUTO)
		{
			mqtt_publish_orp_state();
		}
		state.cl_mode = CL_AUTO;
		break;

	case CL_OFF:
		control_cl_off();
		if (state.cl_mode != CL_OFF)
		{
			mqtt_publish_orp_state();
		}
		state.cl_mode = CL_OFF;
		cl_off();
		break;

	case CL_ON:
		control_cl_on();
		if (state.cl_mode != CL_ON)
		{
			mqtt_publish_orp_state();
		}
		state.cl_mode = CL_ON;
		cl_on();
		break;

	default:
		break;
	}
}

void orp_control_init(void)
{
	printlnA(F("ORP Control Init"));
	cl_enter_mode(CL_OFF);
	cl_enter_mode(CL_AUTO);
	orp_control_update_task = timer_pool.every(ORP_CONTROL_UPDATE_MS, orp_control_update);
	mqtt_publish_orp_state();
}

void orp_control_loop(void)
{
}

bool orp_control_update(void *)
{
	uint8_t h;

	if (state.filter_pump == PUMP_ON && state.cl_mode == CL_AUTO && state.orp_control_state == ORP_IDLE)
	{
		if (measures.orp-parameters.target_orp > parameters.delta_orp) {
			printlnA(F("ORP Correction Needed"));
		}
	}
	return true;
}
