#ifndef __FILTER_CONTROL_H__
#define __FILTER_CONTROL_H__

#include <Arduino.h>
#include "config.h"
#include "state.h"


void filter_control_init(void);
void filter_control_stop(void);

void filter_enter_mode(enum filter_mode_t filter_mode);
void filter_enter_power_mode(enum filter_power_t filter_power);

#endif