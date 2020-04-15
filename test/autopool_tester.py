#!/usr/bin/env python
# -*- coding: utf-8 -*-

import paho.mqtt.client as mqtt
import json
import keyboard

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


measures_count = 0
parameters_count = 0


def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))

    # Subscribing in on_connect() means that if we lose the connection and
    # reconnect then subscriptions will be renewed.
    client.subscribe("autopool/#")
    client.publish("autopool/CMD/BEEP")
    client.publish("autopool/CMD/GET_PARAMETERS")
    client.publish("autopool/CMD/GET_STATES")

# The callback for when a PUBLISH message is received from the server.


def on_message(client, userdata, msg):
    #print(msg.topic+" "+str(msg.payload))
    # print(msg.topic)
    if str(msg.topic) == "autopool/MEAS":
        print("Measure msg in")
        print(str(msg.payload))
        d = json.loads(msg.payload)
        print("Dict:", d)
        print("Sys Temp:", d['system_temperature'])
        print("Level Cl:", d['level_cl'])
    else:
        print(msg.topic, msg.payload)


def on_measures(client, userdata, msg):
    print(msg.topic)
    global measures_count
    measures = json.loads(msg.payload)
    print("\tMeasure index:", measures["index"])
    if measures_count:
        pass
        #print(measures)
    else:
        print("Enter Virtual measures mode")
        measures["level_cl"] = True
        measures["level_ph_minus"] = True
        measures["orp"] = 50.1
        #measures["orp"] = 750.3
        measures["ph"] = 6.5
        client.publish("autopool/CMD/MEASURES", json.dumps(measures))
    measures_count += 1


def on_parameters(client, userdata, msg):
    global parameters_count
    print(msg.topic, str(msg.payload))
    parameters = json.loads(msg.payload)
    print("Parameters", parameters)
    if parameters_count:
        pass
    else:
        parameters["timer_prog"] = 16776960
        client.publish("autopool/CMD/PARAMETERS", json.dumps(parameters))
        client.publish("autopool/CMD/GET_PARAMETERS")
    parameters_count +=1

if __name__ == '__main__':
    measures_count = 0
    if not len(PASS):
        PASS = input("Enter MQTT password: ")
    client = mqtt.Client()
    client.on_connect = on_connect
    client.on_message = on_message

    client.message_callback_add("autopool/MEAS", on_measures)
    client.message_callback_add("autopool/PARAM", on_parameters)

    client.username_pw_set(USER, PASS)
    client.connect(SERVER, PORT, 60)
    client.publish("autopool/CMD/GET_PARAMETERS")
    # client.loop_forever()

    client.loop_start()
    cont = True
    while cont:
        c = keyboard.read_key()
        if c == "q":
            cont = False
        elif c == 's':
            client.publish("autopool/CMD/GET_STATES")
        elif c == 'b':
            client.publish("autopool/CMD/BEEP")
        elif c == 'r':
            client.publish("autopool/CMD/RESET")
        pass
