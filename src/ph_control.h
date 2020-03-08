#ifndef __PH_CONTROL_H__
#define __PH_CONTROL_H__

#include <Arduino.h>
#include "config.h"


enum ph_mode_t
{
    PH_OFF = 0,
    PH_ON = 1,
	PH_AUTO = 2,
};


enum ph_control_state_t
{
    PH_IDLE = 0,
};

extern enum ph_mode_t ph_mode;

void ph_control_init(void);
void ph_control_loop(void);


#endif