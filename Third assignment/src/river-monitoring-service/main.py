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
mqtt_client = None
scheduler = sched.scheduler(time.time, time.sleep)
water_channel_controller_polling_freq = 60

water_level = None
systemState = 'NORMAL'          # (str) system state
broker_address = "broker.mqtt-dashboard.com"
send_topic = "water-level-topic"
receive_topic = "system-state"

"""
    Called by the backend to the frontend:
    - POST /setLatestWaterLevel
    - POST /setSystemState
"""

@app.route('/setNewValvePercentageFromDashboard', methods=['POST'])
def set_new_valve_percentage_from_dashboard():
    newPercentage = request.json.get('newPercentage')
    if newPercentage is not None:
        write_message_to_water_channel_controller(f"V: {newPercentage}")
        return jsonify({'message': 'Percentuale valvola aggiornata con successo.'})
    else:
        return jsonify({'error': 'Non si può modificare la percentuale della valvola mentre è in modalità automatica!.'}), 400


def write_message_to_water_channel_controller(msg):
    while water_channel_controller.in_waiting:
        continue
    water_channel_controller.write(msg.encode())


def serial_ports():
    """ Lists serial port names

        :raises EnvironmentError:
            On unsupported or unknown platforms
        :returns:
            A list of the serial ports available on the system
    """
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
            msg = water_channel_controller.read_until(b';')
            print(msg.decode)
            data = msg.decode().strip().split(": ")
            if data[0] is "V":
                params = {'newPercentage': data[1]}
                requests.post(frontend_url + 'setNewValvePercentageFromSystem', json=params)
            elif data[0] is "S":
                params = {'newState': data[1]}
                requests.post(frontend_url + 'setValveUpdatabilityState', json=params)
    except IndexError:
        pass


def on_message(client, userdata, msg):
    if msg.topic is send_topic:
        data = msg.payload.decode().strip().split(": ")
        if data[0] is "WL":
            global water_level
            water_level = data[1]


if __name__ == '__main__':
    water_channel_controller = serial.Serial(port=serial_ports()[0], baudrate=9600)
    scheduler.enter(water_channel_controller_polling_freq, 1, check_water_channel_controller)
    scheduler.run()

    mqtt_client = mqtt.Client()
    mqtt_client.on_message = on_message
    mqtt_client.connect(broker_address, 1833, 60)
    mqtt_client.subscribe(receive_topic)
    mqtt_client.subscribe(send_topic)
    mqtt_client.loop_start()

    app.run(host=host, port=port, debug=False)
