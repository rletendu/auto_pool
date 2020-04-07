#include "mqtt.h"
#include "parameters.h"
#include "measures.h"
#include <PubSubClient.h>
#include <WiFi.h>
#include <SerialDebug.h>
#include "config.h"
#include "state.h"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void mqtt_callback(char *topic, byte *message, unsigned int length)
{
	char basetopic[20];
	strcpy(basetopic, parameters.mqtt_base_topic);
	strcat(basetopic, "/CMD/");
	String in_topic = topic;
	in_topic.replace(basetopic,"");
	printA("IN topic:");
	printlnA(in_topic);
	String payload_buff;
	for (int i = 0; i < length; i++)
	{
		payload_buff = payload_buff + String((char)message[i]);
	}

	if (in_topic=="filter_pump") {
		printlnA("Found in topic");	
	}

	printA("Topic :")
	printlnA(topic); // Print out messages.
	printA("Payload :")
	printlnA(payload_buff); // Print out messages.
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
			strcpy(topic, parameters.mqtt_base_topic);
			strcat(topic, "/CMD/#");
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
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/LOG");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, message);
	}
}

void mqtt_publish_parameters()
{
	char topic[40];
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/PARAM");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, parameters_json_string);
	}
}

void mqtt_publish_measures()
{
	char topic[40];
	measures_to_json_string();
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/MEAS");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, measures_json_string);
	}
}

void mqtt_publish_states()
{
	mqtt_publish_filter_state();
	mqtt_publish_ph_state();
	mqtt_publish_orp_state();
}

void mqtt_publish_filter_state()
{
	char topic[40];
	filter_state_to_json_string();
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/STATE_FILTER");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, state_filter_json_string);
	}
}

void mqtt_publish_ph_state()
{
	char topic[40];
	ph_state_to_json_string();
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/STATE_PH");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, state_ph_json_string);
	}
}

void mqtt_publish_orp_state()
{
	char topic[40];
	orp_state_to_json_string();
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/STATE_ORP");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, state_orp_json_string);
	}
}




void mqtt_loop()
{
	if (!mqtt_client.connected())
	{
		mqtt_reconnect();
	}
	else
	{
		mqtt_client.loop();
	}
}

