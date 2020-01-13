#include <Arduino.h>

#define PARAM_MAX_STR 50

struct Parameters
{
	uint16_t mqtt_port;
	char mqtt_user[PARAM_MAX_STR];
	char mqtt_pass[PARAM_MAX_STR];
	char humidity_topic[PARAM_MAX_STR];
};


extern char *mqtt_server;
extern char *mqtt_port;
extern char *mqtt_user;
extern char *mqtt_pass;
/* [50] = "xxx.cloudmqtt.com";
char mqtt_port[50]  =       "12345";
char mqtt_user[50]   =      "mqtt_user";
char mqtt_pass[50]   =      "mqtt_pass";
char humidity_topic[50] =   "sensor/humidity";
*/