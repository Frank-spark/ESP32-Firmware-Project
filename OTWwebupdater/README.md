


# WT32-ETH01 Web Updater with Socket.IO Client using Ethernet

This project demonstrates how to set up the WT32-ETH01 (ESP32) module to use Ethernet for hosting a web-based firmware updater and simultaneously act as a Socket.IO client for real-time communication with a server.

The web updater code was heavily based on the **ESP32 Over-the-Air (OTA) Programming** tutorial from Random Nerd Tutorials. You can find the original tutorial here:  
[ESP32 Over-the-Air (OTA) Programming](https://randomnerdtutorials.com/esp32-over-the-air-ota-programming/).

## Requirements

### Hardware:
- **WT32-ETH01 v1.4**
- Ethernet cable
- Power supply (USB or 5V)

### Software:
- [Arduino IDE](https://www.arduino.cc/en/software)
- ESP32 board support package for Arduino ([Installation Guide](https://github.com/espressif/arduino-esp32/blob/master/docs/arduino-ide/boards_manager.md))

## Pin Configuration

The WT32-ETH01 requires the following pin configuration for Ethernet:

- **MDC Pin**: GPIO23
- **MDIO Pin**: GPIO18
- **PHY Power Pin**: GPIO16
- **PHY Address**: 1

## Dependencies

You will need the following libraries for this project. They can be installed through the Arduino IDE or downloaded manually.

1. **WebSockets Client by Markus Sattler**:
   - Provides WebSocket communication support.
   - [GitHub Repo](https://github.com/Links2004/arduinoWebSockets)
   - To install:
     - Open Arduino IDE.
     - Go to **Sketch** -> **Include Library** -> **Manage Libraries**.
     - Search for `WebSockets by Markus Sattler` and install it.

2. **Socket.IO Client Library**:
   - Provides Socket.IO client functionality for ESP32.
   - [GitHub Repo](https://github.com/timum-viw/socket.io-client)
   - To install:
     - Download the library from the repository.
     - In Arduino IDE, go to **Sketch** -> **Include Library** -> **Add .ZIP Library** and select the downloaded ZIP file.

## Installation

1. **Install ESP32 Board Package**:
   - Open the Arduino IDE.
   - Go to **File** -> **Preferences**.
   - Add this URL in the "Additional Boards Manager URLs": `https://dl.espressif.com/dl/package_esp32_index.json`
   - Go to **Tools** -> **Board** -> **Boards Manager**.
   - Search for "ESP32" and install the package by Espressif Systems.

2. **Install Required Libraries**:
   - Install the **WebSockets** and **Socket.IO Client** libraries as mentioned in the dependencies section.

3. **Clone or download this repository**:
   - Clone this project or download the ZIP and extract it.

4. **Open the Project in Arduino IDE**:
   - Open `OTWwebupdater.ino` in the Arduino IDE.

5. **Configure the Static IP (Optional)**:
   - Modify the IP addresses in the sketch if needed:
     - `local_IP`: Set this to the desired static IP for your device (default is `192.168.0.50`).
     - `gateway`: Set this to your network gateway (default is `192.168.0.1`).
     - `subnet`: Set the subnet mask (default is `255.255.255.0`).
     - `primaryDNS` and `secondaryDNS`: Set DNS addresses (default is Google's DNS).

6. **Upload the Sketch**:
   - Connect the WT32-ETH01 to your computer using a USB-to-serial converter.
   - Select your board and the correct COM port in **Tools** -> **Port**.
   - Upload the sketch to your device.

7. **Connect Ethernet**:
   - Connect the WT32-ETH01 to your router or network switch using an Ethernet cable.

## Usage

### Web Updater:
1. **Access the Web Interface**:
   - Once the device is connected to the network, open a web browser and navigate to the static IP address assigned in the sketch (default: `http://192.168.0.50`).

2. **Login Page**:
   - Use the following credentials to log in:
     - **Username**: `admin`
     - **Password**: `admin`

3. **Firmware Update**:
   - After logging in, you will see an upload form for firmware updates.
   - Choose a binary file (`.bin`) and upload it to update the device firmware.
   - The device will automatically reboot after the update.

### Socket.IO Client:
The device will also connect to your Socket.IO server and listen for events. You can monitor the events via the serial output:

```cpp
void socketIOEvent(socketIOmessageType_t type, uint8_t * payload, size_t length) {
  switch (type) {
    case sIOtype_CONNECT:
      Serial.println("[Socket.IO] Connected to server");
      break;
    case sIOtype_EVENT:
      Serial.printf("[Socket.IO] Event received: %s\n", payload);
      break;
    case sIOtype_DISCONNECT:
      Serial.println("[Socket.IO] Disconnected from server");
      break;
    default:
      Serial.println("[Socket.IO] Unknown event");
  }
}
```

You will need to replace `"your-server-address"` in the code with your Socket.IO server's IP or hostname.

## Socket.IO Server Setup

Ensure you have a working Socket.IO server that the WT32-ETH01 can connect to. The ESP32 will attempt to connect to the server at the configured IP and port. You can modify this in the code:

```cpp
socketIO.begin("your-server-address", 3000);  // Replace with your Socket.IO server address
```

## Troubleshooting

### Web Updater:
- **No IP Address Assigned**: Ensure that your router or network supports DHCP or that you've correctly set up a static IP.
- **Cannot Access Web Interface**: Double-check the static IP configuration and confirm that the Ethernet cable is properly connected.

### Socket.IO:
- **Socket.IO Connection Issues**: Ensure that your Socket.IO server is running and accessible at the specified IP and port.
- **Empty Payloads**: If the payloads appear empty, check that the server is sending events correctly, and log payloads in the event handler.

## License

This project is licensed under the MIT License.
```


## Added Credit

 This web updater code is based on the Random Nerd Tutorials guide on **ESP32 Over-the-Air (OTA) Programming** with a link to the tutorial.

