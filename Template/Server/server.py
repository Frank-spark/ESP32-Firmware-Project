from flask import Flask, request
import socketio
import eventlet

# Create a Socket.IO server instance
sio = socketio.Server(cors_allowed_origins="*")

# Create a Flask app
app = Flask(__name__)

# Define a Flask route for testing server status
@app.route('/')
def index():
    return "Socket.IO server is running."


# Define Socket.IO event handlers
@sio.event
def connect(sid, environ):
    print(f"Client connected: {sid}")
    sio.emit("setDelayTime", {"setDelayTime": 1000}, to=sid)

@sio.on("heartbeat")
def handle_heartbeat(sid, data):
    print(f"Heartbeat received from {sid}: {data}")
    sio.emit("pong", {"status": "alive"}, to=sid)

@sio.event
def disconnect(sid):
    print(f"Client disconnected: {sid}")

# Run the server
if __name__ == "__main__":
    # Wrap the Flask app with Socket.IO
    app = socketio.WSGIApp(sio, app)

    # Start the eventlet server
    print("Socket.IO server running on http://0.0.0.0:3000")
    eventlet.wsgi.server(eventlet.listen(("0.0.0.0", 3000)), app)
from flask import Flask
import socketio

app = Flask(__name__)
sio = socketio.Server(cors_allowed_origins="*")
app = socketio.WSGIApp(sio, app)

@sio.on('set_brightness')
def handle_set_brightness(sid, data):
    print(f"Received brightness: {data}")
    # Forward to ESP32 or other handling logic here
    sio.emit('update_brightness', {'brightness': data})

@sio.on('set_level')
def handle_set_level(sid, data):
    print(f"Received level: {data}")
    # Forward to ESP32 or other handling logic here
    sio.emit('update_level', {'level': data})

if __name__ == '__main__':
    import eventlet
    eventlet.wsgi.server(eventlet.listen(('0.0.0.0', 3000)), app)
