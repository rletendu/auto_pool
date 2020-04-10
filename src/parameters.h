#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

#include <Arduino.h>
#include "config.h"

#define PARAM_MAX_STR 50

enum filter_auto_mode_t
{
	AUTO_TIMER_PROG = 0,
	AUTO_TIMER_FCT_T = 1,
};

struct ParametersStruture
{
	char mqtt_server[PARAM_MAX_STR];
	char mqtt_port[PARAM_MAX_STR];
	char mqtt_user[PARAM_MAX_STR];
	char mqtt_pass[PARAM_MAX_STR];
	char mqtt_base_topic[PARAM_MAX_STR];
	float target_ph;
	float delta_ph;
	float target_orp;
	float delta_orp;
	float flow_cl;
	float flow_ph_minus;
	float flow_ph_plus;
	uint32_t timer_prog;
	enum filter_auto_mode_t filter_auto_mode;
	float pressure_warning;
};

extern struct ParametersStruture parameters;
extern char parameters_json_string[PARAMETERS_JSON_MESSAGE_LEN];

bool parameters_read_file(void);
void parameters_write_file(void);
void parameters_format(void);
bool parameters_json_to_param(char *json_str);

#endif