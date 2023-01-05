#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>

void mqtt_init(void);
void mqtt_loop(void);
void mqtt_reconnect();
void mqtt_publish_debug(const char *message);
void mqtt_publish_log(const char *message);
void mqtt_publish_parameters(void);
void mqtt_publish_measures(void);
void mqtt_publish_states();
void mqtt_publish_filter_state();
void mqtt_publish_ph_state();
void mqtt_publish_orp_state();

#endif