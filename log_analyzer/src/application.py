from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, send, emit
from src.config_reader import ConfigReader

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret'
app.config['DEBUG'] = True

socketio = SocketIO(app)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/stats')
def stats():
    return render_template('stats.html')


@app.route('/settings')
def settings():
    cr = ConfigReader()
    cr.read()
    return render_template('settings.html', checkers=cr.get_checkers())


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
