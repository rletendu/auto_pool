#include <Arduino.h>

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
	char base_topic[PARAM_MAX_STR];
	float target_ph;
	float delta_ph;
	float target_redox;
	float delta_redox;
	float flow_cl;
	float flow_ph_minus;
	float flow_ph_plus;
	uint32_t timer_prog;
	enum filter_auto_mode_t filter_auto_mode;
	float pressure_warning;
};

extern struct ParametersStruture parameters;

bool parameters_read_json(void);
void parameters_write_json(void);
void parameters_format(void);
