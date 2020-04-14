import paho.mqtt.client as mqtt
import json

SERVER = "192.168.1.39"
PORT = 1883
USER = "admin"
PASS = ""

"""
MEASURES = {"system_temperature": 23.9, "system_humidity": 22.5, "water_temperature": 23.3125, "pump_pressure": 0,
            "ph": 0, "orp": -1.44648, "level_cl": false, "level_ph_minus": false, "level_ph_plus": false, "level_water": true}
PARAM = {"mqtt_server": "192.168.1.39", "mqtt_port": "1883", "mqtt_user": "admin", "mqtt_pass": "pulsar", "base_topic": "autopool", "target_ph": 7, "delta_ph": 0.5,
         "target_orp": 650, "delta_orp": 50, "flow_cl": 20.2, "flow_ph_minus": 21.5, "flow_ph_plus": 19.5, "filter_auto_mode": 0, "timer_prog": 2686976, "pressure_warning": 2.2}
"""

# The callback for when the client receives a CONNACK response from the server.


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("autopool/#")

# The callback for when a PUBLISH message is received from the server.


def on_message(client, userdata, msg):
    #print(msg.topic+" "+str(msg.payload))
    #print(msg.topic)
    if str(msg.topic) == "autopool/MEAS":
        print("Measure msg in")
        print(str(msg.payload))
        d = json.loads(msg.payload)
        print("Dict:",d)
        print("Sys Temp:",d['system_temperature'])
        print("Level Cl:",d['level_cl'])
    else:
        print(msg.topic, msg.payload)

PASS=input()
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message

client.username_pw_set(USER, PASS)
client.connect(SERVER, PORT, 60)

# Blocking call that processes network traffic, dispatches callbacks and
# handles reconnecting.
# Other loop*() functions are available that give a threaded interface and a
# manual interface.
client.loop_forever()
