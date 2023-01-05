#include "autopool.h"

WiFiClient espClient;
PubSubClient mqtt_client(espClient);

void mqtt_callback(char *topic, byte *message, unsigned int length)
{

	int i;
	char basetopic[20];
	char payload[MQTT_MAX_PACKET_SIZE];
	strcpy(basetopic, parameters.mqtt_base_topic);
	strcat(basetopic, "/CMD/");
	String in_topic = topic;
	in_topic.replace(basetopic, "");

	String payload_buff;
	for (i = 0; i < length; i++)
	{
		payload_buff = payload_buff + String((char)message[i]);
		payload[i] = message[i];
	}
	payload[i] = 0;

	if (in_topic == "FILTER_PUMP")
	{
		printlnA("Found in topic");
	}
	else if (in_topic == "FILTER_MODE")
	{
	}
	else if (in_topic == "RESET")
	{
		ESP.restart();
	}
	else if (in_topic == "MEASURES")
	{
		measures_set_virtual(true);
		measures_json_to_measures(payload);
	}
	else if (in_topic == "PARAMETERS")
	{
		if (parameters_json_to_param(payload))
		{
			parameters_write_file();
		}
	}
	else if (in_topic == "FILTER_STATE")
	{
		filter_state_json_to_state(payload);
	}
	else if (in_topic == "FILTER_PWR_FULL")
	{
		filter_enter_power_mode(FILTER_POWER_FULL);
	}
	else if (in_topic == "FILTER_PWR_REG")
	{
		filter_enter_power_mode(FILTER_POWER_REG);
	}
	else if (in_topic == "ORP_STATE")
	{
		orp_state_json_to_state(payload);
	}
	else if (in_topic == "PH_STATE")
	{
		ph_state_json_to_state(payload);
	}
	else if (in_topic == "BEEP")
	{
		beep(10);
	}
	else if (in_topic == "GET_PARAMETERS")
	{
		mqtt_publish_parameters();
	}
	else if (in_topic == "GET_STATES")
	{
		mqtt_publish_states();
	}
	else if (in_topic == "LOG")
	{
		display_log_append(payload);
	}
	else if (in_topic == "FS")
	{
		String buf = "SPIFF Content:\r\n";
		File root = SPIFFS.open("/");
		File file = root.openNextFile();
		while (file)
		{
			buf += file.name();
			buf += "\r\n";
			file = root.openNextFile();
		}
		mqtt_publish_log((char *)buf.c_str());
	}
}

void mqtt_init(void)
{
	mqtt_client.setServer(parameters.mqtt_server, atoi(parameters.mqtt_port));
	if (wifi_is_available())
	{
		mqtt_reconnect();
	}
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

void mqtt_publish_log(const char *message)
{
	char topic[40];
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/LOG");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, message);
	}
}

void mqtt_publish_debug(const char *message)
{
	char topic[40];
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/DEBUG");
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
	if (wifi_is_available())
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
}
