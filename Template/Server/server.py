from flask import Flask, request
import socketio
import eventlet

# Create a Socket.IO server instance
sio = socketio.Server(cors_allowed_origins="*", ping_timeout=120, ping_interval=30)




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
    sio.emit("2", {}, to=sid)  # Respond with a pong


@sio.on("message")
def handle_message(sid, data):
    print(f"Message from {sid}: {data}")



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

