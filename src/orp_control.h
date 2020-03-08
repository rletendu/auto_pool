#ifndef __OPR_CONTROL_H__
#define __ORP_CONTROL_H__

#include <Arduino.h>
#include "config.h"



enum orp_mode_t
{
    ORP_OFF = 0,
    ORP_ON = 1,
	ORP_AUTO = 2,
};

enum or_control_state_t
{
    ORP_IDLE = 0,
};

extern enum orp_mode_t orp_mode;

void orp_control_init(void);
void orp_control_loop(void);

#endif