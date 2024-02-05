from time import sleep
import requests
from flask import Flask, jsonify, render_template, request

app = Flask(__name__)

host = '127.0.0.1'
port = 5000
core_backend_url = 'http://localhost:5001/'

measure_interval = 2 * 60       # seconds, corresponds to the time in the x-axis of the graph
data = []                       # array of water levels for each second in the last x minutes
canUpdateValve = True           # (bool) if the valve can be updated or not
valveOpeningPercentage = 25     # (int) valve opening percentage
systemState = 'NORMAL'          # (str) system state
currentWaterLevel = 0           # (float) current water level

"""
    ENDPOINTS SUMMARY

    Called from FRONTEND:
    - GET /getChartData
    - GET /getValveUpdatabilityState
    - POST /setNewValvePercentage (contains POST /setNewValvePercentageFromDashboard directed to the core backend)
    - GET /getSystemState
    - GET /getValveOpeningPercentage
    - GET /getInitialWaterThresholds

    Called from BACKEND:
    - POST /setValveUpdatabilityState
    - POST /setLatestWaterLevel
    - POST /setSystemState
    - POST /setNewValvePercentageFromSystem
"""

@app.route('/')
def index():
    """
    Render the default Dashboard page
    """
    return render_template('index.html')

@app.route('/getChartData', methods=['GET'])
def get_data():
    """
    Called by the frontend to get the water trend
    in the last x minutes.
    Returns a JSON array with the level for each second.
    """
    global data, currentWaterLevel
    data.append(currentWaterLevel)
    data = data[-measure_interval:]
    return jsonify(data)

@app.route('/getValveUpdatabilityState', methods=['GET'])
def get_valve_update_state():
    """
    Called by the frontend to know if the valve position
    can be updated.
    If not, the Send button is disabled to prevent requests.
    """
    global canUpdateValve
    return jsonify({'canUpdateValve': canUpdateValve})

@app.route('/setValveUpdatabilityState', methods=['POST'])
def set_valve_update_state():
    """
    Called by the core backend to set new valve state.
    It is used by the core backend to disable the update status
    of the valve when it cannot be changed and restore it when it can.
    """
    global canUpdateValve
    new_state = request.json.get('newState')
    if new_state is not None:
        if new_state == 'true':
            canUpdateValve = True
        else:
            canUpdateValve = False
        return jsonify({'message': 'Stato valvola aggiornato con successo.'})
    else:
        return jsonify({'error': 'Il parametro new_state deve essere un booleano valido.'}), 400

@app.route('/setNewValvePercentage', methods=['POST'])
def set_new_percentage():
    """
    Called by the frontend to set the new valve opening percentage.
    It receives the new valve opening level and sends it to
    the core backend.
    """
    global valveOpeningPercentage
    newPercentage = request.json.get('newPercentage')

    if newPercentage is not None:
        try:
            valveOpeningPercentage = int(newPercentage)
            params = {'newPercentage': valveOpeningPercentage}
            response = requests.post(core_backend_url + 'setNewValvePercentageFromDashboard', json=params)

            if response.status_code == 200:
                return jsonify({'message': 'Percentuale impostata correttamente'})
            else:
                return jsonify({'message': 'Errore nella chiamata al core backend'})
        except ValueError:
            return jsonify({'message': 'Percentuale non valida'})
    else:
        return jsonify({'message': 'Percentuale non impostata'})

@app.route('/setLatestWaterLevel', methods=['POST'])
def set_latest_water_level():
    """
    Called by the core backend to set the last registered water level.
    It saves it in a variable so that if the new update does not arrive
    in time, in the graph no changes will be shown.
    """
    global currentWaterLevel
    newWaterLevel = request.json.get('level')

    if newWaterLevel is not None:
        try:
            currentWaterLevel = float(newWaterLevel)
            return jsonify({'message': 'Livello impostato correttamente'})
        except ValueError:
            return jsonify({'message': 'Livello non valido'})
    else:
        return jsonify({'message': 'Livello non impostato'})

@app.route('/getSystemState', methods=['GET'])
def get_system_state():
    """
    Called from the frontend to obtain the current system state.
    """
    global systemState
    return jsonify({'systemState': systemState})

@app.route('/setSystemState', methods=['POST'])
def set_system_state():
    """
    Called from the backend core to set the new system state.
    """
    global systemState
    new_state = request.json.get('newState')
    if new_state is not None:
        systemState = new_state
        return jsonify({'message': 'Stato sistema aggiornato con successo.'})
    else:
        return jsonify({'error': 'Il parametro new_state deve essere un booleano valido.'}), 400

@app.route('/getValveOpeningPercentage', methods=['GET'])
def get_valve_opening_percentage():
    """
    Called from the frontend to obtain the valve opening level.
    """
    global valveOpeningPercentage
    return jsonify({'valveOpeningPercentage': valveOpeningPercentage})

@app.route('/setNewValvePercentageFromSystem', methods=['POST'])
def set_new_valve_percentage_from_system():
    """
    Called from the backend core to set the new valve opening level.
    """
    global valveOpeningPercentage
    new_percentage = request.json.get('newPercentage')
    if new_percentage is not None:
        valveOpeningPercentage = int(new_percentage)
        return jsonify({'message': 'Percentuale valvola aggiornata con successo.'})
    else:
        return jsonify({'error': 'Il parametro new_percentage deve essere un numero valido.'}), 400

@app.route('/getInitialWaterThresholds', methods=['GET'])
def get_initial_water_thresholds():
    """
    Called from the frontend to obtain the initial water thresholds from the backend.
    """
    response = requests.get(core_backend_url + 'getWaterLevelThresholds')
    if response.status_code == 200:
        return jsonify(response.json())
    else:
        return jsonify({'error': 'Errore nella chiamata al core backend'}), 400

if __name__ == '__main__':
    while True:
        try:
            response = requests.post(core_backend_url + 'registerNewDashboard', data={ "DASHBOARD_PORT": port })
            if response.status_code == 200:
                break
        except:
            sleep(0.1)
    app.run(host=host, port=port, debug=False)