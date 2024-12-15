import tkinter as tk
from socketio import Client
import socketio

# Create a Socket.IO client instance
sio = socketio.Client()

# Event: Connected to the server
@sio.on('connect')
def on_connect():
    print('Connected to the server.')

# Event: Disconnected from the server
@sio.on('disconnect')
def on_disconnect():
    print('Disconnected from the server.')

# Event: Handle delay time acknowledgment
@sio.on('delay_ack')
def on_delay_ack(data):
    print(f"Server acknowledged delay time: {data}")

# Function: Update delay time from slider
def update_delay(value):
    try:
        delay_value = int(value)
        sio.emit('setDelayTime', {'delay': delay_value})
        print(f"Sent new delay time: {delay_value}")
    except ValueError:
        print("Invalid value from slider.")

# Connect to the server
sio.connect('http://192.168.1.221:3000')  # Replace with your server's IP and port

# Tkinter GUI
root = tk.Tk()
root.title("ESP32 Delay Controller")

# GUI slider label
slider_label = tk.Label(root, text="Set Delay Time (ms):")
slider_label.pack()

# GUI slider for delay time
slider = tk.Scale(root, from_=1000, to=50000, orient=tk.HORIZONTAL, command=update_delay)
slider.pack()

# Run GUI event loop
root.mainloop()
