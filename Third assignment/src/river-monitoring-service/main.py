from flask import Flask, jsonify, request
import paho.mqtt.client as mqtt
import threading
import requests
import serial
import json

app = Flask(__name__)
water_channel_controller = None
mqtt_client = mqtt.Client()

with open("config.json") as config_file:
    config_json = json.load(config_file)
    backend_ip = config_json["backend_ip"]
    backend_port = config_json["backend_port"]
    frontend_url = config_json["frontend_url"]
    broker_ip = config_json["broker_ip"]
    broker_port = config_json["broker_port"]
    send_topic = config_json["send_topic"]
    receive_topic = config_json["receive_topic"]
    water_channel_controller_port = config_json["water_channel_controller_port"]
    frontend_url = config_json["frontend_url"]
    WL1 = config_json["WL1"]
    WL2 = config_json["WL2"]
    WL3 = config_json["WL3"]
    WL4 = config_json["WL4"]
    F1 = config_json["F1"]
    F2 = config_json["F2"]
    system_state = config_json["initial_system_state"]
    valve_percentage = config_json["initial_valve_percentage"]
    valve_state = config_json["initial_valve_state"]


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
                    params = {'newPercentage': valve_percentage}
                    requests.post(frontend_url + 'setNewValvePercentageFromSystem', json=params)

                global valve_state
                old_valve_state = valve_state
                valve_state = data[1].split(": ")[1].replace(";", "")
                if old_valve_state != valve_state:
                    params = {'newState': "false" if valve_state == "MANUAL" else "true"}
                    requests.post(frontend_url + 'setValveUpdatabilityState', json=params)
        except IndexError:
            pass


def on_message(client, userdata, msg):
    if msg.topic == receive_topic:
        data = msg.payload.decode().strip().split(":")
        if data[0] == "WL":
            global system_state
            water_level = float(data[1])
            params = {'level': water_level}
            requests.post(frontend_url + 'setLatestWaterLevel', json=params)
            if water_level < WL1:
                old_system_state = system_state
                system_state = "ALARM-TOO-LOW"
                if old_system_state != system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(0)
            elif water_level <= WL2:
                old_system_state = system_state
                system_state = "NORMAL"
                if old_system_state != system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(25)
                    set_water_level_measurement_freq(F1)
            elif water_level <= WL3:
                old_system_state = system_state
                system_state = "PRE-ALARM-TOO-HIGH"
                if old_system_state != system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(25)
                    set_water_level_measurement_freq(F2)
            elif water_level <= WL4:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH"
                if old_system_state != system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(50)
                    set_water_level_measurement_freq(F2)
            else:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH-CRITIC"
                if old_system_state != system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(100)
                    set_water_level_measurement_freq(F2)


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

