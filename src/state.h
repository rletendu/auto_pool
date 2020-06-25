
#ifndef __STATE_H__
#define __STATE_H__

#include <Arduino.h>
#include "config.h"

enum pump_state_t
{
	PUMP_OFF = 0,
	PUMP_ON = 1,
};

enum orp_mode_t
{
	ORP_OFF = 0,
	ORP_ON = 1,
	ORP_AUTO = 2,
};

enum filter_mode_t
{
	FILTER_OFF = 0,
	FILTER_ON = 1,
	FILTER_AUTO = 2,
};

enum filter_power_t
{
	FILTER_POWER_FULL = 0,
	FILTER_POWER_REG = 1,
};

enum filter_control_state_t
{
	FILTER_IDLE = 0,
	FILTER_AUTO_ACTIVE_WARM_UP,
	FILTER_AUTO_ACTIVE,
	FILTER_AUTO_ACTIVE_EXTENDED,
};

enum orp_control_state_t
{
	ORP_IDLE = 0,
	ORP_INJECTION_ON,
	ORP_INJECTION_OFF,
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
	PH_MINUS_INJECTION_ON,
	PH_PLUS_INJECTION_ON,
	PH_MINUS_INJECTION_OFF,
	PH_PLUS_INJECTION_OFF
};

struct StateStructure
{
	enum filter_mode_t filter_mode;
	enum filter_power_t filter_power;
	enum pump_state_t filter_pump;
	enum filter_control_state_t filter_control_state;

	enum orp_mode_t orp_mode;
	enum orp_control_state_t orp_control_state;
	enum pump_state_t orp_pump;

	enum ph_minus_mode_t ph_minus_mode;
	enum ph_plus_mode_t ph_plus_mode;
	enum pump_state_t ph_minus_pump;
	enum pump_state_t ph_plus_pump;
	enum ph_control_state_t ph_control_state;
};

enum correction_need_t
{
	NO_CORRECTION = 0,
	STEP1_CORRECTION = 1,
	STEP2_CORRECTION = 2,
	STEP3_CORRECTION = 3,
	STEP4_CORRECTION = 4,
};

extern struct StateStructure state;
extern char state_filter_json_string[STATE_FILTER_CTRL_JSON_MESSAGE_LEN];
extern char state_ph_json_string[STATE_PH_CTRL_JSON_MESSAGE_LEN];
extern char state_orp_json_string[STATE_ORP_CTRL_JSON_MESSAGE_LEN];

void filter_state_to_json_string(void);
void orp_state_to_json_string(void);
void ph_state_to_json_string(void);

bool filter_state_json_to_state(char *json_str);
bool orp_state_json_to_state(char *json_str);
bool ph_state_json_to_state(char *json_str);

#endif