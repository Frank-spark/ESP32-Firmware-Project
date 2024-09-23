import tkinter as tk
import socketio

# Initialize SocketIO client
sio = socketio.Client(logger=True, engineio_logger=True)


# Connect to the SocketIO server with correct namespace
def connect_to_server():
    try:
        # Connect to the default namespace ("/")
        sio.connect('http://192.168.1.221:5000')
        print("Connected to the server.")
    except Exception as e:
        print(f"Connection error: {e}")

# Disconnect from the server
def disconnect_from_server():
    sio.disconnect()
    print("Disconnected from the server.")

# Ensure we are connected before emitting events
def emit_if_connected(event, data):
    if sio.connected:
        sio.emit(event, data)
    else:
        print("Not connected to the server.")

# Define button actions for sending relay control commands
def relay_on(relay_number):
    command = f"relay{relay_number}_on"
    print(f"Sending: {command}")
    sio.emit('relay_control', command)

# Function to send the relay OFF command
def relay_off(relay_number):
    command = f"relay{relay_number}_off"
    print(f"Sending: {command}")
    sio.emit('relay_control', command)

# Create the GUI window
root = tk.Tk()
root.title("Relay Control Panel")

# Connect and disconnect buttons
connect_button = tk.Button(root, text="Connect to Server", command=connect_to_server)
connect_button.pack(pady=5)

disconnect_button = tk.Button(root, text="Disconnect from Server", command=disconnect_from_server)
disconnect_button.pack(pady=5)

# Create relay control buttons
for i in range(1, 9):
    on_button = tk.Button(root, text=f"Relay {i} ON", command=lambda i=i: relay_on(i))
    on_button.pack(pady=2)
    
    off_button = tk.Button(root, text=f"Relay {i} OFF", command=lambda i=i: relay_off(i))
    off_button.pack(pady=2)

# Run the GUI loop
root.mainloop()

