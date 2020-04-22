#ifndef __PH_CONTROL_H__
#define __PH_CONTROL_H__

#include <Arduino.h>
#include "config.h"
#include "state.h"


void ph_minus_enter_mode(enum ph_minus_mode_t ph_minus_mode);
void ph_plus_enter_mode(enum ph_plus_mode_t ph_plus_mode);

void ph_control_init(void);
void ph_control_stop(void);



#endif