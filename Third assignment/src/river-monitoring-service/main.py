from flask import Flask, jsonify, request
import paho.mqtt.client as mqtt
import threading
import requests
import serial
import json
import sys

app = Flask(__name__)
water_channel_controller = None
mqtt_client = mqtt.Client()
dashboards = []

with open(sys.argv[0].replace("main.py", "") + "config.json") as config_file:
    config_json = json.load(config_file)
    backend_ip = config_json["backend_ip"]
    backend_port = config_json["backend_port"]
    broker_ip = config_json["broker_ip"]
    broker_port = config_json["broker_port"]
    send_topic = config_json["send_topic"]
    receive_topic = config_json["receive_topic"]
    water_channel_controller_port = config_json["water_channel_controller_port"]
    WL1 = config_json["WL1"]
    WL2 = config_json["WL2"]
    WL3 = config_json["WL3"]
    WL4 = config_json["WL4"]
    F1 = config_json["F1"]
    F2 = config_json["F2"]
    system_state = config_json["initial_system_state"]
    valve_percentage = config_json["initial_valve_percentage"]
    valve_state = config_json["initial_valve_state"]


def update_dashboards(endpoint, params):
    for dashboard in dashboards:
        try:
            requests.post(dashboard + endpoint, json=params, timeout=2)
        except:
            dashboards.remove(dashboard)


@app.route('/registerNewDashboard', methods=['POST'])
def register_new_dashboard():
    dashboard_addr = "http://" + str(request.remote_addr) + ":" + str(request.form.get('DASHBOARD_PORT')) + "/"
    if dashboard_addr in dashboards:
        return jsonify({'error': 'Dashboard già registrata!.'}), 400
    else:
        dashboards.append(dashboard_addr)
        return jsonify({'message': 'Dashboard registrata con successo.'}), 200


@app.route('/getWaterLevelThresholds', methods=['GET'])
def get_water_level_thresholds():
    return jsonify({'WL1': WL1, 'WL2': WL2, 'WL3': WL3, 'WL4': WL4})


@app.route('/setNewValvePercentageFromDashboard', methods=['POST'])
def set_new_valve_percentage_from_dashboard():
    if set_water_channel_valve_percentage(request.json.get('newPercentage')):
        return jsonify({'message': 'Percentuale valvola aggiornata con successo.'})
    else:
        return jsonify({'error': 'Non si può modificare la percentuale della valvola mentre è in modalità automatica!.'}), 400


def set_water_channel_valve_percentage(percentage):
    if percentage is None or valve_state == "MANUAL":
        return False

    global valve_percentage
    if valve_percentage != percentage:
        valve_percentage = percentage
        while water_channel_controller.in_waiting:
            continue
        water_channel_controller.write(f"V: {valve_percentage};".encode())
    return True


def set_water_level_measurement_freq(freq):
    mqtt_client.publish(send_topic, f"F: {freq}")


def check_water_channel_controller():
    while True:
        try:
            if water_channel_controller.in_waiting > 0:
                msg = water_channel_controller.read_until(b';')
                data = msg.decode().strip().split(" | ")

                global valve_percentage
                old_valve_percentage = valve_percentage
                valve_percentage = data[0].split(": ")[1]
                if old_valve_percentage != valve_percentage:
                    update_dashboards('setNewValvePercentageFromSystem', {'newPercentage': valve_percentage})

                global valve_state
                old_valve_state = valve_state
                valve_state = data[1].split(": ")[1].replace(";", "")
                if old_valve_state != valve_state:
                    params = {'newState': "false" if valve_state == "MANUAL" else "true"}
                    update_dashboards('setValveUpdatabilityState', params)
        except:
            pass


def alarm_too_low_state():
    set_water_channel_valve_percentage(0)
    set_water_level_measurement_freq(F1)


def normal_state():
    set_water_channel_valve_percentage(25)
    set_water_level_measurement_freq(F1)


def pre_alarm_too_high_state():
    set_water_channel_valve_percentage(25)
    set_water_level_measurement_freq(F2)


def alarm_too_high_state():
    set_water_channel_valve_percentage(50)
    set_water_level_measurement_freq(F2)


def alarm_too_high_critic_state():
    set_water_channel_valve_percentage(100)
    set_water_level_measurement_freq(F2)


def on_message(client, userdata, msg):
    if msg.topic == receive_topic:
        data = msg.payload.decode().strip().split(":")
        if data[0] == "WL":
            global system_state
            water_level = float(data[1])
            update_dashboards('setLatestWaterLevel', {'level': water_level})
            if water_level < WL1:
                old_system_state = system_state
                system_state = "ALARM-TOO-LOW"
                if old_system_state != system_state:
                    update_dashboards('setSystemState', {'newState': system_state})
                    alarm_too_low_state()
            elif water_level <= WL2:
                old_system_state = system_state
                system_state = "NORMAL"
                if old_system_state != system_state:
                    update_dashboards('setSystemState', {'newState': system_state})
                    normal_state()
            elif water_level <= WL3:
                old_system_state = system_state
                system_state = "PRE-ALARM-TOO-HIGH"
                if old_system_state != system_state:
                    update_dashboards('setSystemState', {'newState': system_state})
                    pre_alarm_too_high_state()
            elif water_level <= WL4:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH"
                if old_system_state != system_state:
                    update_dashboards('setSystemState', {'newState': system_state})
                    alarm_too_high_state()
            else:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH-CRITIC"
                if old_system_state != system_state:
                    update_dashboards('setSystemState', {'newState': system_state})
                    alarm_too_high_critic_state()


def flask_thread():
    app.run(host=backend_ip, port=backend_port, debug=False)


if __name__ == '__main__':
    mqtt_client.on_message = on_message
    mqtt_client.connect(broker_ip, broker_port, 60)
    mqtt_client.subscribe(receive_topic)
    mqtt_client.subscribe(send_topic)
    mqtt_client.loop_start()

    water_channel_controller = serial.Serial(port=water_channel_controller_port, baudrate=9600)

    water_controller_thread = threading.Thread(target=check_water_channel_controller)
    flask_thread = threading.Thread(target=flask_thread)

    water_controller_thread.start()
    flask_thread.start()

    water_controller_thread.join()
    flask_thread.join()

    normal_state()
