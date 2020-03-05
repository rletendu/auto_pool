#include "mqtt.h"
#include "parameters.h"
#include "measures.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <SerialDebug.h>
#include "config.h"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void mqtt_callback(char *topic, byte *message, unsigned int length)
{

}

void mqtt_init(void)
{
	mqtt_client.setServer(parameters.mqtt_server, atoi(parameters.mqtt_port));
	mqtt_reconnect();
	mqtt_client.setCallback(mqtt_callback);
}

void mqtt_reconnect()
{
	char topic[40];
	if (!mqtt_client.connected())
	{
		if (mqtt_client.connect(MQTT_CLIENT_NAME, parameters.mqtt_user, parameters.mqtt_pass))
		{
			// Subscribe
			strcpy (topic,parameters.mqtt_base_topic);
			strcat (topic,"/CMD");
			mqtt_client.subscribe(topic);
			mqtt_publish_log("AUTOPOOL (Re)connected");
		}
		else
		{
		}
	}
}

void mqtt_publish_log(char *message)
{
	char topic[40];
	strcpy (topic,parameters.mqtt_base_topic);
	strcat (topic,"/LOG");
	if (mqtt_client.connected())
	{ 
		mqtt_client.publish(topic,message);
	}
}

void mqtt_publish_parameters()
{
	char topic[40];
	strcpy (topic,parameters.mqtt_base_topic);
	strcat (topic,"/PARAM");
	if (mqtt_client.connected())
	{ 
		mqtt_client.publish(topic,parameters_json_string);
	}
}

void mqtt_publish_measures()
{
	char topic[40];
	strcpy (topic,parameters.mqtt_base_topic);
	strcat (topic,"/MEAS");
	if (mqtt_client.connected())
	{ 
		mqtt_client.publish(topic,measures_json_string);
	}
}

void mqtt_loop()
{
	if (!mqtt_client.connected())
	{
		mqtt_reconnect();
	} else {
		mqtt_client.loop();
	}
}
