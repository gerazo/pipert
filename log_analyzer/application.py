from flask import Flask, render_template, request, jsonify
from flask_socketio import SocketIO, send, emit

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret'
app.config['DEBUG'] = True

socketio = SocketIO(app)


@app.route('/')
def index():
    return render_template('index.html')


@app.route('/', methods=['POST'])
def get_packet():
    channels = request.json
    # print(channels)
    socketio.emit("update_channels", channels, json=True)
    return jsonify({"ok": True})


def run():
    socketio.run(app)

# @socketio.on('message')
# def receive_message(message):
#     print("#########", message)
#     send("Hello From the BackEnd")


if __name__ == '__main__':
    socketio.run(app)
