from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret'
app.config['DEBUG'] = True

socketio = SocketIO(app)

pipeline_measures_names = []
channel_measures_names = []


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/measurements')
def measurements():
    return render_template('measurements.html', pipeline_measures=pipeline_measures_names,
                           channel_measures=channel_measures_names)


@app.route('/measurements', methods=['POST'])
def measurement_post():
    req_json = request.json
    pipeline_measures = req_json.get("pipeline_measures_names")
    channel_measures = req_json.get("channel_measures_names")
    pipeline_measures_rates = req_json.get("pipeline_measures")
    if pipeline_measures:
        global pipeline_measures_names
        pipeline_measures_names = pipeline_measures
    if channel_measures:
        global channel_measures_names
        channel_measures_names = channel_measures
    if pipeline_measures_rates:
        socketio.emit("pipeline_measures_update", pipeline_measures_rates, json=True)
    return jsonify({"ok": True})


@app.route('/', methods=['POST'])
def index_post():
    req_json = request.json
    if req_json.get("c_dicts"):
        socketio.emit("update_channels", req_json.get("c_dicts"), json=True)
        return jsonify({"ok": True})

    if req_json.get("unique_channels"):
        socketio.emit("channels_map", req_json, json=True)
        return jsonify({"ok": True})

    if req_json.get("m_dicts"):
        socketio.emit("measures_update", req_json.get("m_dicts"), json=True)
        return jsonify({"ok": True})


def run():
    socketio.run(app)


if __name__ == '__main__':
    socketio.run(app)
