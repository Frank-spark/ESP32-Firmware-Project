import tkinter as tk
from tkinter import ttk, scrolledtext
import socketio

# Socket.IO client setup
sio = socketio.Client()

# Main GUI Class
class SocketIOApp:
    def __init__(self, root):
        self.root = root
        self.root.title("Socket.IO Control Panel")
        self.root.geometry("500x500")
        self.root.configure(bg="#f4f4f4")

        # Initialize state
        self.is_connected = False

        # GUI Elements
        self.create_controls()
        self.create_sliders()
        self.create_serial_area()

        # Socket.IO Listeners
        self.setup_socketio_listeners()

    def create_controls(self):
        """Create Connect, Disconnect, and Close buttons."""
        button_frame = ttk.Frame(self.root)
        button_frame.pack(pady=10)

        self.connect_btn = ttk.Button(button_frame, text="Connect", command=self.connect_to_server)
        self.connect_btn.pack(side="left", padx=5)

        self.disconnect_btn = ttk.Button(button_frame, text="Disconnect", command=self.disconnect_from_server)
        self.disconnect_btn.pack(side="left", padx=5)

        self.close_btn = ttk.Button(button_frame, text="Close", command=self.close_app)
        self.close_btn.pack(side="left", padx=5)

    def create_sliders(self):
        """Create sliders for brightness and float level."""
        # Brightness Slider
        brightness_label = ttk.Label(self.root, text="Brightness", font=("Arial", 12), background="#f4f4f4")
        brightness_label.pack(pady=(10, 5))

        self.brightness_slider = ttk.Scale(self.root, from_=0, to=255, orient="horizontal", command=self.send_brightness)
        self.brightness_slider.pack(fill="x", padx=20)

        # Float Level Slider
        float_label = ttk.Label(self.root, text="Float Level", font=("Arial", 12), background="#f4f4f4")
        float_label.pack(pady=(10, 5))

        self.float_slider = ttk.Scale(self.root, from_=0, to=100, orient="horizontal", command=self.send_float_level)
        self.float_slider.pack(fill="x", padx=20)

    def create_serial_area(self):
        """Create a scrolling text area for serial traffic."""
        serial_label = ttk.Label(self.root, text="Socket.IO Traffic", font=("Arial", 12), background="#f4f4f4")
        serial_label.pack(pady=(10, 5))

        self.serial_area = scrolledtext.ScrolledText(self.root, height=10, font=("Courier", 10), bg="#ffffff", wrap="word")
        self.serial_area.pack(fill="both", padx=20, pady=10, expand=True)

    def connect_to_server(self):
        """Connect to the Socket.IO server."""
        if not self.is_connected:
            try:
                sio.connect("http://192.168.1.221:5000")  # Update with your server IP and port
                self.is_connected = True
                self.print_to_serial("Connected to Socket.IO Server")
            except Exception as e:
                self.print_to_serial(f"Connection Error: {e}")

    def disconnect_from_server(self):
        """Disconnect from the Socket.IO server."""
        if self.is_connected:
            sio.disconnect()
            self.is_connected = False
            self.print_to_serial("Disconnected from Socket.IO Server")

    def close_app(self):
        """Close the GUI application."""
        if self.is_connected:
            sio.disconnect()
        self.root.destroy()

    def send_brightness(self, value):
        """Send brightness value to the server."""
        if self.is_connected:
            brightness = int(float(value))
            sio.emit("setBrightness", {"brightness": brightness})
            self.print_to_serial(f"Sent: setBrightness -> {brightness}")

    def send_float_level(self, value):
        """Send float level value to the server."""
        if self.is_connected:
            float_level = int(float(value))
            sio.emit("setFloatLevel", {"level": float_level})
            self.print_to_serial(f"Sent: setFloatLevel -> {float_level}")

    def print_to_serial(self, message):
        """Display messages in the serial area."""
        self.serial_area.insert(tk.END, message + "\n")
        self.serial_area.see(tk.END)

    def setup_socketio_listeners(self):
        """Set up Socket.IO listeners for events."""

        @sio.on("connect")
        def on_connect():
            self.print_to_serial("Server: Connected")

        @sio.on("disconnect")
        def on_disconnect():
            self.print_to_serial("Server: Disconnected")

        @sio.on("pong")
        def on_pong(data):
            self.print_to_serial(f"Received: pong -> {data}")

        @sio.event
        def message(data):
            self.print_to_serial(f"Message: {data}")

# Main Function
if __name__ == "__main__":
    root = tk.Tk()
    app = SocketIOApp(root)
    root.mainloop()
