from flask import Flask, jsonify, request
import paho.mqtt.client as mqtt
import requests
import serial
import glob
import sys
import sched
import time

app = Flask(__name__)
host = '127.0.0.1'
port = 5001
frontend_url = 'http://localhost:5000/'
water_channel_controller = None
mqtt_client = mqtt.Client()
scheduler = sched.scheduler(time.time, time.sleep)
water_channel_controller_polling_freq = 60

WL1 = 100
WL2 = 200
WL3 = 300
WL4 = 400
F1 = 50
F2 = 100
system_state = "NORMAL"
valve_percentage = None
valve_state = "AUTOMATIC"
broker_address = "broker.mqtt-dashboard.com"
send_topic = "system-state"
receive_topic = "water-level-topic"


@app.route('/setNewValvePercentageFromDashboard', methods=['POST'])
def set_new_valve_percentage_from_dashboard():
    if set_water_channel_valve_percentage(request.json.get('newPercentage')):
        return jsonify({'message': 'Percentuale valvola aggiornata con successo.'})
    else:
        return jsonify({'error': 'Non si può modificare la percentuale della valvola mentre è in modalità automatica!.'}), 400


def set_water_channel_valve_percentage(percentage):
    if percentage is None or valve_state is "MANUAL":
        return False

    global valve_percentage
    if valve_percentage is not percentage:
        valve_percentage = percentage
        while water_channel_controller.in_waiting:
            continue
        water_channel_controller.write(f"V: {valve_percentage}".encode())
    return True


def set_water_level_measurement_freq(freq):
    mqtt_client.publish(send_topic, f"F: {freq}")


def serial_ports():
    if sys.platform.startswith('win'):
        ports = ['COM%s' % (i + 1) for i in range(256)]
    elif sys.platform.startswith('linux') or sys.platform.startswith('cygwin'):
        # this excludes your current terminal "/dev/tty"
        ports = glob.glob('/dev/tty[A-Za-z]*')
    elif sys.platform.startswith('darwin'):
        ports = glob.glob('/dev/tty.*')
    else:
        raise EnvironmentError('Unsupported platform')

    result = []
    for port in ports:
        try:
            s = serial.Serial(port)
            s.close()
            result.append(port)
        except (OSError, serial.SerialException):
            pass
    return result


def check_water_channel_controller():
    scheduler.enter(water_channel_controller_polling_freq, 1, check_water_channel_controller)
    try:
        if water_channel_controller.in_waiting > 0:
            # Per evitare letture casuali ogni msg è V: 50 | S: MANUAL;
            msg = water_channel_controller.read_until(b';')
            data = msg.decode().strip().split(" | ")

            global valve_percentage
            old_valve_percentage = valve_percentage
            valve_percentage = data[0].split(": ")[1]
            if old_valve_percentage is not valve_percentage:
                params = {'newPercentage': valve_percentage}
                requests.post(frontend_url + 'setNewValvePercentageFromSystem', json=params)

            global valve_state
            old_valve_state = valve_state
            valve_state = data[1].split(": ")[1]
            if old_valve_state is not valve_state:
                params = {'newState': valve_state}
                requests.post(frontend_url + 'setValveUpdatabilityState', json=params)
    except IndexError:
        pass


def on_message(client, userdata, msg):
    if msg.topic is receive_topic:
        data = msg.payload.decode().strip().split(": ")
        if data[0] is "WL":
            global system_state
            water_level = float(data[1])
            params = {'level': water_level}
            requests.post(frontend_url + 'setLatestWaterLevel', json=params)
            if water_level < WL1:
                old_system_state = system_state
                system_state = "ALARM-TOO-LOW"
                if old_system_state is not system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(0)
            elif water_level <= WL2:
                old_system_state = system_state
                system_state = "NORMAL"
                if old_system_state is not system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(25)
                    set_water_level_measurement_freq(F1)
            elif water_level <= WL3:
                old_system_state = system_state
                system_state = "PRE-ALARM-TOO-HIGH"
                if old_system_state is not system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_level_measurement_freq(F2)
            elif water_level <= WL4:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH"
                if old_system_state is not system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(50)
                    set_water_level_measurement_freq(F2)
            else:
                old_system_state = system_state
                system_state = "ALARM-TOO-HIGH-CRITIC"
                if old_system_state is not system_state:
                    requests.post(frontend_url + 'setSystemState', json={'newState': system_state})
                    set_water_channel_valve_percentage(100)
                    set_water_level_measurement_freq(F2)


if __name__ == '__main__':
    water_channel_controller = serial.Serial(port=serial_ports()[0], baudrate=9600)
    scheduler.enter(water_channel_controller_polling_freq, 1, check_water_channel_controller)
    scheduler.run()

    mqtt_client.on_message = on_message
    mqtt_client.connect(broker_address, 1833, 60)
    mqtt_client.subscribe(receive_topic)
    mqtt_client.subscribe(send_topic)
    mqtt_client.loop_start()

    app.run(host=host, port=port, debug=False)
