#ifndef __OPR_CONTROL_H__
#define __ORP_CONTROL_H__

#include <Arduino.h>
#include "config.h"
#include "state.h"



void orp_control_init(void);
void orp_enter_mode(enum orp_mode_t orp_mode);
enum correction_need_t orp_correction_needed(void);

#endif