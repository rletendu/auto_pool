
#ifndef __STATE_H__
#define __STATE_H__

#include <Arduino.h>
#include "config.h"
//#include "filter_control.h"
//#include "orp_control.h"
//#include "ph_control.h"


enum pump_state_t
{
    PUMP_OFF = 0,
    PUMP_ON = 1,
};

enum cl_mode_t
{
    CL_OFF = 0,
    CL_ON = 1,
	CL_AUTO = 2,
};


enum filter_mode_t
{
    FILTER_OFF = 0,
    FILTER_ON = 1,
    FILTER_AUTO = 2,
};

enum filter_control_state_t
{
	FILTER_IDLE = 0,
};

enum orp_control_state_t
{
    ORP_IDLE = 0,
};


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



struct StateStructure
{
	enum cl_mode_t cl_mode;
	enum ph_minus_mode_t ph_minus_mode;
	enum ph_plus_mode_t ph_plus_mode;
	enum filter_mode_t filter_mode;

	enum pump_state_t cl_pump;
	enum pump_state_t ph_minus_pump;
	enum pump_state_t ph_plus_pump;
	enum pump_state_t filter_pump;

	enum ph_control_state_t ph_control_state;
	enum filter_control_state_t filter_control_state;
	enum orp_control_state_t orp_control_state;

};

extern struct StateStructure state;
extern char state_json_string[STATE_JSON_MESSAGE_LEN];



#endif