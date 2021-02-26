from flask import Flask, render_template
from flask_socketio import SocketIO, send

app = Flask(__name__)
app.config['SECRET_KEY'] = 'secret'
app.config['DEBUG'] = True

socketio = SocketIO(app)


@app.route('/')
def index():
    return render_template('index.html')


@socketio.on('message')
def receive_message(message):
    print("#########", message)
    send("Hello From the BackEnd")


if __name__ == '__main__':
    socketio.run(app)
