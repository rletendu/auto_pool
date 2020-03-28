
#ifndef __STATE_H__
#define __STATE_H__

#include <Arduino.h>
#include "config.h"
#include "filter_control.h"
#include "orp_control.h"
#include "ph_control.h"

struct StateStructure
{
	enum cl_mode_t cl_mode;
	enum ph_minus_mode_t ph_minus_mode;
	enum ph_plus_mode_t ph_plus_mode;
	enum filter_mode_t filter_mode;

	enum ph_control_state_t ph_control_state;
	//enum filter_control_state_t filter_control_state;
	enum orp_control_state_t orp_control_state;

};

extern struct StateStructure state;
extern char state_json_string[STATE_JSON_MESSAGE_LEN];



#endif