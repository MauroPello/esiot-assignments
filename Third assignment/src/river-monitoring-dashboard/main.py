from flask import Flask, jsonify, render_template, request
import random

app = Flask(__name__)

host = '127.0.0.1'
port = 5000
measure_interval = 5 * 60 # seconds

data = []
currentWaterLevel = 0
canUpdateValve = True

# Default Dashboard page
@app.route('/')
def index():
    return render_template('index.html')

# Called by the frontend to get the water trend in the last x minutes
@app.route('/getChartData')
def get_data():
    global data, currentWaterLevel
    data.append(currentWaterLevel)
    data = data[-measure_interval:]
    return jsonify(data)

# Called by the frontend to know if the valve position can be updated
@app.route('/getValveUpdateState', methods=['GET'])
def get_valve_update_state():
    global canUpdateValve
    return jsonify({'canUpdateValve': canUpdateValve})

# Called by the core backend to set new valve state
@app.route('/setValveUpdateState', methods=['POST'])
def set_valve_update_state():
    global canUpdateValve
    new_state = request.json.get('newState')    
    if new_state is not None and isinstance(new_state, bool):
        canUpdateValve = new_state
        return jsonify({'message': 'Stato valvola aggiornato con successo.'})
    else:
        return jsonify({'error': 'Il parametro new_state deve essere un booleano valido.'}), 400

# Called by the frontend to set the new valve opening percentage
@app.route('/setNewPercentage', methods=['POST'])
def get_new_percentage():
    if request.method == 'POST':
        data = request.json
        percentuale = data.get('percentuale')
        print(f"Dato ricevuto dal client: {percentuale}")
        return jsonify({'message': 'Dato ricevuto correttamente dal server'})
    else:
        return jsonify({'message': 'Metodo non consentito'})

# Called by the core backend to set the last registered water level
@app.route('/setNewLevel', methods=['GET'])
def get_new_level():
    global currentWaterLevel
    newWaterLevel = request.args.get('level')
    
    if newWaterLevel is not None:
        try:
            currentWaterLevel = int(newWaterLevel)
            return jsonify({'message': 'Livello impostato correttamente'})
        except ValueError:
            return jsonify({'message': 'Livello non valido'})
    else:
        return jsonify({'message': 'Livello non impostato'})

if __name__ == '__main__':
    app.run(host=host, port=port, debug=True)
