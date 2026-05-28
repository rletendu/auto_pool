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
	else if (in_topic == "FILTER_MODE_AUTO")
	{
		filter_enter_mode(FILTER_AUTO);
	}
	else if (in_topic == "FILTER_MODE_OFF")
	{
		filter_enter_mode(FILTER_OFF);
	}
	else if (in_topic == "FILTER_MODE_ON")
	{
		filter_enter_mode(FILTER_ON);
	}
	else if (in_topic == "ORP_MODE_AUTO")
	{
		orp_enter_mode(ORP_AUTO);
	}
	else if (in_topic == "ORP_MODE_OFF")
	{
		orp_enter_mode(ORP_OFF);
	}
	else if (in_topic == "ORP_MODE_ON")
	{
		orp_enter_mode(ORP_ON);
	}
	else if (in_topic == "RST_DAILY_ML_ORP")
	{
		measures.daily_ml_orp = 0;
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
		bool prev_ha = parameters.ha_discovery_enabled;
		if (parameters_json_to_param(payload))
		{
			parameters_write_file();
			if (prev_ha && !parameters.ha_discovery_enabled)
			{
				mqtt_discovery_clear_all();
			}
			else if (!prev_ha && parameters.ha_discovery_enabled)
			{
				mqtt_discovery_publish_all();
			}
		}
	}
	else if (in_topic == "SET/FILTER_MODE")
	{
		if (strcmp(payload, "AUTO") == 0) filter_enter_mode(FILTER_AUTO);
		else if (strcmp(payload, "ON") == 0) filter_enter_mode(FILTER_ON);
		else if (strcmp(payload, "OFF") == 0) filter_enter_mode(FILTER_OFF);
	}
	else if (in_topic == "SET/FILTER_POWER")
	{
		if (strcmp(payload, "FULL") == 0) filter_enter_power_mode(FILTER_POWER_FULL);
		else if (strcmp(payload, "REG") == 0) filter_enter_power_mode(FILTER_POWER_REG);
	}
	else if (in_topic == "SET/ORP_MODE")
	{
		if (strcmp(payload, "AUTO") == 0) orp_enter_mode(ORP_AUTO);
		else if (strcmp(payload, "ON") == 0) orp_enter_mode(ORP_ON);
		else if (strcmp(payload, "OFF") == 0) orp_enter_mode(ORP_OFF);
	}
	else if (in_topic == "SET/PH_MINUS_MODE")
	{
		if (strcmp(payload, "AUTO") == 0) ph_minus_enter_mode(PH_MINUS_AUTO);
		else if (strcmp(payload, "ON") == 0) ph_minus_enter_mode(PH_MINUS_ON);
		else if (strcmp(payload, "OFF") == 0) ph_minus_enter_mode(PH_MINUS_OFF);
	}
	else if (in_topic == "SET/PH_PLUS_MODE")
	{
		if (strcmp(payload, "AUTO") == 0) ph_plus_enter_mode(PH_PLUS_AUTO);
		else if (strcmp(payload, "ON") == 0) ph_plus_enter_mode(PH_PLUS_ON);
		else if (strcmp(payload, "OFF") == 0) ph_plus_enter_mode(PH_PLUS_OFF);
	}
	else if (in_topic.startsWith("SET/"))
	{
		bool changed = true;
		float v = atof(payload);
		if      (in_topic == "SET/TARGET_PH")            parameters.target_ph = v;
		else if (in_topic == "SET/DELTA_PH")             parameters.delta_ph = v;
		else if (in_topic == "SET/TARGET_ORP")           parameters.target_orp = v;
		else if (in_topic == "SET/DELTA_ORP")            parameters.delta_orp = v;
		else if (in_topic == "SET/FLOW_CL")              parameters.flow_cl = v;
		else if (in_topic == "SET/FLOW_PH_MINUS")        parameters.flow_ph_minus = v;
		else if (in_topic == "SET/FLOW_PH_PLUS")         parameters.flow_ph_plus = v;
		else if (in_topic == "SET/CL_MAX_DAY")           parameters.cl_max_day = v;
		else if (in_topic == "SET/PHM_MAX_DAY")          parameters.phm_max_day = v;
		else if (in_topic == "SET/PRESSURE_WARNING")     parameters.pressure_warning = v;
		else if (in_topic == "SET/PH_OFFSET")            parameters.ph_offset = v;
		else if (in_topic == "SET/ORP_OFFSET")           parameters.orp_offset = v;
		else if (in_topic == "SET/PERIODIC_FILTER_TIME") parameters.periodic_filter_time = v;
		else changed = false;
		if (changed)
		{
			parameters_write_file();
			mqtt_publish_parameters();
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

void mqtt_publish_availability(const char *status)
{
	char topic[40];
	strcpy(topic, parameters.mqtt_base_topic);
	strcat(topic, "/AVAIL");
	if (mqtt_client.connected())
	{
		mqtt_client.publish(topic, status, true);
	}
}

void mqtt_reconnect()
{
	char topic[40];
	char will_topic[40];
	if (!mqtt_client.connected())
	{
		strcpy(will_topic, parameters.mqtt_base_topic);
		strcat(will_topic, "/AVAIL");
		if (mqtt_client.connect(MQTT_CLIENT_NAME, parameters.mqtt_user, parameters.mqtt_pass,
								will_topic, 0, true, "offline"))
		{
			// Subscribe
			strcpy(topic, parameters.mqtt_base_topic);
			strcat(topic, "/CMD/#");
			mqtt_client.subscribe(topic);
			mqtt_publish_availability("online");
			mqtt_publish_log("AUTOPOOL (Re)connected");
			if (parameters.ha_discovery_enabled)
			{
				mqtt_discovery_publish_all();
			}
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
