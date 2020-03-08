#ifndef __MQTT_H__
#define __MQTT_H__

#include <Arduino.h>

void mqtt_init(void);
void mqtt_loop(void);
void mqtt_reconnect();
void mqtt_publish_log(char *message);
void mqtt_publish_parameters(void);
void mqtt_publish_measures(void);

#endif