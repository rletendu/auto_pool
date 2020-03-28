#ifndef __OPR_CONTROL_H__
#define __ORP_CONTROL_H__

#include <Arduino.h>
#include "config.h"




enum cl_mode_t
{
    CL_OFF = 0,
    CL_ON = 1,
	CL_AUTO = 2,
};

enum orp_control_state_t
{
    ORP_IDLE = 0,
};



void orp_control_init(void);
void orp_control_loop(void);

#endif