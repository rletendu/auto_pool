#include "display_logger.h"
#include "mqtt.h"


void log_append(char *msg){
	mqtt_publish_log(msg);
	display_log_append(msg);
}