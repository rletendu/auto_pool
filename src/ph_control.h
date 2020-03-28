#ifndef __PH_CONTROL_H__
#define __PH_CONTROL_H__

#include <Arduino.h>
#include "config.h"



enum ph_minus_mode_t
{
    PH_MINUS_OFF = 0,
    PH_MINUS_ON = 1,
	PH_MINUS_AUTO = 2,
};

enum ph_plus_mode_t
{
    PH_PLUS_OFF = 0,
    PH_PLUS_ON = 1,
	PH_PLUS_AUTO = 2,
};

enum ph_control_state_t
{
    PH_IDLE = 0,
};



void ph_control_init(void);
void ph_control_loop(void);


#endif