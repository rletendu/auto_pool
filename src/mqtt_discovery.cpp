#include "autopool.h"
#include "mqtt_discovery.h"
#include <ArduinoJson.h>
#include <PubSubClient.h>

extern PubSubClient mqtt_client;

static char node_id[24];
static bool node_id_built = false;

static const char *MFR = "AutoPool";
static const char *MDL = "ESP32 Pool Controller";

static void build_node_id(void)
{
	if (node_id_built) return;
	String mac = WiFi.macAddress();
	mac.replace(":", "");
	String tail = mac.substring(mac.length() - 6);
	tail.toLowerCase();
	snprintf(node_id, sizeof(node_id), "autopool_%s", tail.c_str());
	node_id_built = true;
}

static char dev_sw[16];

static void add_device_block(JsonObject &cfg)
{
	if (dev_sw[0] == 0)
	{
		snprintf(dev_sw, sizeof(dev_sw), "%.1f", AUTOPOOL_VER);
	}
	JsonObject &dev = cfg.createNestedObject("dev");
	JsonArray &ids = dev.createNestedArray("ids");
	ids.add(node_id);
	dev["name"] = "AutoPool";
	dev["mf"] = MFR;
	dev["mdl"] = MDL;
	dev["sw"] = dev_sw;
}

static void avail_topic(char *out, size_t n)
{
	snprintf(out, n, "%s/AVAIL", parameters.mqtt_base_topic);
}

static void disc_topic(char *out, size_t n, const char *component, const char *object)
{
	snprintf(out, n, "%s/%s/%s/%s/config",
			 parameters.ha_discovery_prefix, component, node_id, object);
}

static void publish_cfg(const char *component, const char *object, JsonObject &cfg)
{
	char topic[128];
	char payload[768];
	char avty[64];
	avail_topic(avty, sizeof(avty));
	cfg["avty_t"] = avty;
	char uid[48];
	snprintf(uid, sizeof(uid), "%s_%s", node_id, object);
	cfg["uniq_id"] = uid;
	char oid[48];
	snprintf(oid, sizeof(oid), "autopool_%s", object);
	cfg["obj_id"] = oid;
	add_device_block(cfg);
	disc_topic(topic, sizeof(topic), component, object);
	cfg.printTo(payload, sizeof(payload));
	mqtt_client.publish(topic, payload, true);
}

static void clear_cfg(const char *component, const char *object)
{
	char topic[128];
	disc_topic(topic, sizeof(topic), component, object);
	mqtt_client.publish(topic, "", true);
}

// ---------- entity builders ----------

static void publish_sensor(const char *object, const char *name,
						   const char *state_suffix, const char *json_field,
						   const char *unit, const char *dev_cla,
						   const char *ent_cat, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48];
	snprintf(st, sizeof(st), "%s/%s", parameters.mqtt_base_topic, state_suffix);
	cfg["name"] = name;
	cfg["stat_t"] = st;
	char tpl[64];
	snprintf(tpl, sizeof(tpl), "{{ value_json.%s }}", json_field);
	cfg["val_tpl"] = tpl;
	if (unit) cfg["unit_of_meas"] = unit;
	if (dev_cla) cfg["dev_cla"] = dev_cla;
	if (ent_cat) cfg["ent_cat"] = ent_cat;
	if (icon) cfg["ic"] = icon;
	publish_cfg("sensor", object, cfg);
}

static void publish_sensor_tpl(const char *object, const char *name,
							   const char *state_suffix, const char *value_template,
							   const char *ent_cat, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48];
	snprintf(st, sizeof(st), "%s/%s", parameters.mqtt_base_topic, state_suffix);
	cfg["name"] = name;
	cfg["stat_t"] = st;
	cfg["val_tpl"] = value_template;
	if (ent_cat) cfg["ent_cat"] = ent_cat;
	if (icon) cfg["ic"] = icon;
	publish_cfg("sensor", object, cfg);
}

static void publish_bsensor(const char *object, const char *name,
							const char *state_suffix, const char *json_field,
							const char *dev_cla, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48];
	snprintf(st, sizeof(st), "%s/%s", parameters.mqtt_base_topic, state_suffix);
	cfg["name"] = name;
	cfg["stat_t"] = st;
	char tpl[80];
	snprintf(tpl, sizeof(tpl), "{{ 'ON' if value_json.%s else 'OFF' }}", json_field);
	cfg["val_tpl"] = tpl;
	cfg["pl_on"] = "ON";
	cfg["pl_off"] = "OFF";
	if (dev_cla) cfg["dev_cla"] = dev_cla;
	if (icon) cfg["ic"] = icon;
	publish_cfg("binary_sensor", object, cfg);
}

static void publish_select_modes(const char *object, const char *name,
								 const char *state_suffix, const char *json_field,
								 const char *cmd_suffix, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48], cmd[48], tpl[96];
	snprintf(st, sizeof(st), "%s/%s", parameters.mqtt_base_topic, state_suffix);
	snprintf(cmd, sizeof(cmd), "%s/CMD/%s", parameters.mqtt_base_topic, cmd_suffix);
	snprintf(tpl, sizeof(tpl),
			 "{{ {0:'OFF',1:'ON',2:'AUTO'}.get(value_json.%s|int) }}", json_field);
	cfg["name"] = name;
	cfg["stat_t"] = st;
	cfg["cmd_t"] = cmd;
	cfg["val_tpl"] = tpl;
	JsonArray &ops = cfg.createNestedArray("ops");
	ops.add("OFF");
	ops.add("ON");
	ops.add("AUTO");
	if (icon) cfg["ic"] = icon;
	publish_cfg("select", object, cfg);
}

static void publish_select_filter_power(void)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48], cmd[48];
	snprintf(st, sizeof(st), "%s/STATE_FILTER", parameters.mqtt_base_topic);
	snprintf(cmd, sizeof(cmd), "%s/CMD/SET/FILTER_POWER", parameters.mqtt_base_topic);
	cfg["name"] = "Filter Power";
	cfg["stat_t"] = st;
	cfg["cmd_t"] = cmd;
	cfg["val_tpl"] = "{{ {0:'FULL',1:'REG'}.get(value_json.filter_power|int) }}";
	JsonArray &ops = cfg.createNestedArray("ops");
	ops.add("FULL");
	ops.add("REG");
	cfg["ic"] = "mdi:speedometer";
	publish_cfg("select", "filter_power", cfg);
}

static void publish_number(const char *object, const char *name,
						   const char *json_field, const char *cmd_suffix,
						   float vmin, float vmax, float step,
						   const char *unit, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char st[48], cmd[48], tpl[64];
	snprintf(st, sizeof(st), "%s/PARAM", parameters.mqtt_base_topic);
	snprintf(cmd, sizeof(cmd), "%s/CMD/%s", parameters.mqtt_base_topic, cmd_suffix);
	snprintf(tpl, sizeof(tpl), "{{ value_json.%s }}", json_field);
	cfg["name"] = name;
	cfg["stat_t"] = st;
	cfg["cmd_t"] = cmd;
	cfg["val_tpl"] = tpl;
	cfg["min"] = vmin;
	cfg["max"] = vmax;
	cfg["step"] = step;
	cfg["mode"] = "box";
	if (unit) cfg["unit_of_meas"] = unit;
	if (icon) cfg["ic"] = icon;
	cfg["ent_cat"] = "config";
	publish_cfg("number", object, cfg);
}

static void publish_button(const char *object, const char *name,
						   const char *cmd_suffix, const char *icon)
{
	StaticJsonBuffer<512> buf;
	JsonObject &cfg = buf.createObject();
	char cmd[48];
	snprintf(cmd, sizeof(cmd), "%s/CMD/%s", parameters.mqtt_base_topic, cmd_suffix);
	cfg["name"] = name;
	cfg["cmd_t"] = cmd;
	cfg["pl_prs"] = "1";
	if (icon) cfg["ic"] = icon;
	cfg["ent_cat"] = "config";
	publish_cfg("button", object, cfg);
}

// ---------- entity table for clear_all ----------

struct entity_ref { const char *component; const char *object; };

static const entity_ref ENTITIES[] = {
	// sensors (MEAS)
	{"sensor", "water_temperature"},
	{"sensor", "system_temperature"},
	{"sensor", "system_humidity"},
	{"sensor", "pump_pressure"},
	{"sensor", "ph"},
	{"sensor", "orp"},
	{"sensor", "day_max_water_temperature"},
	{"sensor", "daily_ml_orp"},
	{"sensor", "daily_ml_ph_minus"},
	{"sensor", "daily_ml_ph_plus"},
	{"sensor", "daily_filter_min"},
	{"sensor", "total_filter_min"},
	{"sensor", "boot_count"},
	// diagnostic state sensors
	{"sensor", "filter_control_state"},
	{"sensor", "orp_control_state"},
	{"sensor", "ph_control_state"},
	// binary sensors
	{"binary_sensor", "level_water"},
	{"binary_sensor", "level_cl"},
	{"binary_sensor", "level_ph_minus"},
	{"binary_sensor", "level_ph_plus"},
	{"binary_sensor", "filter_pump"},
	{"binary_sensor", "orp_pump"},
	{"binary_sensor", "ph_minus_pump"},
	{"binary_sensor", "ph_plus_pump"},
	{"binary_sensor", "pressure_warning"},
	// filter timing diagnostic sensors
	{"sensor", "filter_time_pump_on"},
	{"sensor", "filter_time_pump_off"},
	// selects
	{"select", "filter_mode"},
	{"select", "filter_power"},
	{"select", "orp_mode"},
	{"select", "ph_minus_mode"},
	{"select", "ph_plus_mode"},
	// numbers
	{"number", "target_ph"},
	{"number", "delta_ph"},
	{"number", "target_orp"},
	{"number", "delta_orp"},
	{"number", "flow_cl"},
	{"number", "flow_ph_minus"},
	{"number", "flow_ph_plus"},
	{"number", "cl_max_day"},
	{"number", "phm_max_day"},
	{"number", "pressure_warning"},
	{"number", "ph_offset"},
	{"number", "orp_offset"},
	{"number", "periodic_filter_time"},
	// buttons
	{"button", "reset"},
	{"button", "rst_daily_ml_orp"},
};

// ---------- public API ----------

void mqtt_discovery_publish_all(void)
{
	if (!mqtt_client.connected()) return;
	build_node_id();
	printlnA("MQTT HA discovery: publishing");

	// Sensors from MEAS
	publish_sensor("water_temperature", "Water Temperature", "MEAS", "water_temperature", "°C", "temperature", NULL, NULL);
	publish_sensor("system_temperature", "System Temperature", "MEAS", "system_temperature", "°C", "temperature", "diagnostic", NULL);
	publish_sensor("system_humidity", "System Humidity", "MEAS", "system_humidity", "%", "humidity", "diagnostic", NULL);
	publish_sensor("pump_pressure", "Pump Pressure", "MEAS", "pump_pressure", "bar", "pressure", NULL, NULL);
	publish_sensor("ph", "pH", "MEAS", "ph", "pH", NULL, NULL, "mdi:flask");
	publish_sensor("orp", "ORP", "MEAS", "orp", "mV", "voltage", NULL, "mdi:flash");
	publish_sensor("day_max_water_temperature", "Day Max Water Temp", "MEAS", "day_max_water_temperature", "°C", "temperature", NULL, NULL);
	publish_sensor("daily_ml_orp", "Daily Chlorine Injected", "MEAS", "daily_ml_orp", "mL", NULL, NULL, "mdi:water-pump");
	publish_sensor("daily_ml_ph_minus", "Daily pH- Injected", "MEAS", "daily_ml_ph_minus", "mL", NULL, NULL, "mdi:water-pump");
	publish_sensor("daily_ml_ph_plus", "Daily pH+ Injected", "MEAS", "daily_ml_ph_plus", "mL", NULL, NULL, "mdi:water-pump");
	publish_sensor("daily_filter_min", "Daily Filter Time", "MEAS", "daily_filter_min", "min", "duration", NULL, "mdi:timer-sand");
	publish_sensor("total_filter_min", "Total Filter Time", "MEAS", "total_filter_min", "min", "duration", "diagnostic", "mdi:timer");
	publish_sensor("boot_count", "Boot Count", "MEAS", "boot_count", NULL, NULL, "diagnostic", "mdi:counter");

	// Diagnostic state sensors
	publish_sensor_tpl("filter_control_state", "Filter Control State", "STATE_FILTER",
		"{{ {0:'IDLE',1:'WARM_UP',2:'ACTIVE',3:'EXTENDED',4:'PERIODIC'}.get(value_json.filter_control_state|int) }}",
		"diagnostic", "mdi:state-machine");
	publish_sensor_tpl("orp_control_state", "ORP Control State", "STATE_ORP",
		"{{ {0:'IDLE',1:'INJECTION_ON',2:'INJECTION_OFF'}.get(value_json.orp_control_state|int) }}",
		"diagnostic", "mdi:state-machine");
	publish_sensor_tpl("ph_control_state", "pH Control State", "STATE_PH",
		"{{ {0:'IDLE',1:'MINUS_INJ_ON',2:'PLUS_INJ_ON',3:'MINUS_INJ_OFF',4:'PLUS_INJ_OFF'}.get(value_json.ph_control_state|int) }}",
		"diagnostic", "mdi:state-machine");

	// Binary sensors (levels - inverted: tank empty = problem when value is true? in measures, level_water is true when OK)
	publish_bsensor("level_water", "Water Level OK", "MEAS", "level_water", NULL, "mdi:waves");
	publish_bsensor("level_cl", "Chlorine Tank OK", "MEAS", "level_cl", NULL, "mdi:cup-water");
	publish_bsensor("level_ph_minus", "pH- Tank OK", "MEAS", "level_ph_minus", NULL, "mdi:cup-water");
	publish_bsensor("level_ph_plus", "pH+ Tank OK", "MEAS", "level_ph_plus", NULL, "mdi:cup-water");

	// Pump on/off binary sensors
	publish_bsensor("filter_pump", "Filter Pump", "STATE_FILTER", "filter_pump", "running", "mdi:pump");
	publish_bsensor("orp_pump", "Chlorine Pump", "STATE_ORP", "orp_pump", "running", "mdi:pump");
	publish_bsensor("ph_minus_pump", "pH- Pump", "STATE_PH", "ph_minus_pump", "running", "mdi:pump");
	publish_bsensor("ph_plus_pump", "pH+ Pump", "STATE_PH", "ph_plus_pump", "running", "mdi:pump");

	// Pressure warning (computed in firmware against parameters.pressure_warning)
	publish_bsensor("pressure_warning", "Pressure Warning", "MEAS", "pressure_warning_active", "problem", "mdi:alert");

	// Filter pump on/off timestamps (millis since boot) - diagnostic
	publish_sensor("filter_time_pump_on", "Filter Pump-On Timestamp", "STATE_FILTER", "filter_time_pump_on", "ms", NULL, "diagnostic", "mdi:timer-play");
	publish_sensor("filter_time_pump_off", "Filter Pump-Off Timestamp", "STATE_FILTER", "filter_time_pump_off", "ms", NULL, "diagnostic", "mdi:timer-pause");

	// Selects
	publish_select_modes("filter_mode", "Filter Mode", "STATE_FILTER", "filter_mode", "SET/FILTER_MODE", "mdi:filter");
	publish_select_filter_power();
	publish_select_modes("orp_mode", "ORP Mode", "STATE_ORP", "orp_mode", "SET/ORP_MODE", "mdi:flash");
	publish_select_modes("ph_minus_mode", "pH- Mode", "STATE_PH", "ph_minus_mode", "SET/PH_MINUS_MODE", "mdi:flask-minus");
	publish_select_modes("ph_plus_mode", "pH+ Mode", "STATE_PH", "ph_plus_mode", "SET/PH_PLUS_MODE", "mdi:flask-plus");

	// Numbers (setpoints)
	publish_number("target_ph", "Target pH", "target_ph", "SET/TARGET_PH", 6.5, 8.0, 0.05, "pH", "mdi:flask");
	publish_number("delta_ph", "pH Delta", "delta_ph", "SET/DELTA_PH", 0.0, 1.0, 0.05, "pH", "mdi:delta");
	publish_number("target_orp", "Target ORP", "target_orp", "SET/TARGET_ORP", 400, 900, 10, "mV", "mdi:flash");
	publish_number("delta_orp", "ORP Delta", "delta_orp", "SET/DELTA_ORP", 0, 200, 10, "mV", "mdi:delta");
	publish_number("flow_cl", "Chlorine Pump Flow", "flow_cl", "SET/FLOW_CL", 1, 100, 0.1, "mL/min", "mdi:water-pump");
	publish_number("flow_ph_minus", "pH- Pump Flow", "flow_ph_minus", "SET/FLOW_PH_MINUS", 1, 100, 0.1, "mL/min", "mdi:water-pump");
	publish_number("flow_ph_plus", "pH+ Pump Flow", "flow_ph_plus", "SET/FLOW_PH_PLUS", 1, 100, 0.1, "mL/min", "mdi:water-pump");
	publish_number("cl_max_day", "Daily Chlorine Cap", "cl_max_day", "SET/CL_MAX_DAY", 0, 5000, 50, "mL", "mdi:cup-water");
	publish_number("phm_max_day", "Daily pH- Cap", "phm_max_day", "SET/PHM_MAX_DAY", 0, 5000, 50, "mL", "mdi:cup-water");
	publish_number("pressure_warning", "Pressure Warning", "pressure_warning", "SET/PRESSURE_WARNING", 0, 3, 0.1, "bar", "mdi:gauge");
	publish_number("ph_offset", "pH Calibration Offset", "ph_offset", "SET/PH_OFFSET", -2, 2, 0.05, "pH", "mdi:tune");
	publish_number("orp_offset", "ORP Calibration Offset", "orp_offset", "SET/ORP_OFFSET", -200, 200, 10, "mV", "mdi:tune");
	publish_number("periodic_filter_time", "Periodic Filter Time", "periodic_filter_time", "SET/PERIODIC_FILTER_TIME", 0, 600, 5, "min", "mdi:timer");

	// Buttons
	publish_button("reset", "Reset Controller", "RESET", "mdi:restart");
	publish_button("rst_daily_ml_orp", "Reset Daily Chlorine Counter", "RST_DAILY_ML_ORP", "mdi:counter");
}

void mqtt_discovery_clear_all(void)
{
	if (!mqtt_client.connected()) return;
	build_node_id();
	printlnA("MQTT HA discovery: clearing");
	const size_t n = sizeof(ENTITIES) / sizeof(ENTITIES[0]);
	for (size_t i = 0; i < n; ++i)
	{
		clear_cfg(ENTITIES[i].component, ENTITIES[i].object);
	}
}
