var express = require('express');  // Use Express for serving files
var http = require('http');
var socketIo = require('socket.io');
var os = require('os');  // Used to get local network IP addresses

// Create an Express app
var app = express();

// Serve static files from the public directory
app.use(express.static('public'));

// Create an HTTP server
var server = http.createServer(app);

// Ensure that the root ('/') serves the index.html
app.get('/', function(req, res) {
    res.sendFile(__dirname + '/public/index.html');
});

// Create Socket.IO server with CORS enabled
const io = socketIo(server, {
  path: '/socket.io',  // Ensure the path is correct
  cors: {
    origin: "*",  // Allow all origins
    methods: ["GET", "POST"]
  }
});

// Start the server and print IP and port
server.listen(5000, function() {
    const networkInterfaces = os.networkInterfaces();
    let serverAddress = 'localhost';  // Default if no network interfaces are found

    // Loop over network interfaces to find the local network IP address
    for (const interfaceName in networkInterfaces) {
        const interfaceInfo = networkInterfaces[interfaceName];
        interfaceInfo.forEach(info => {
            if (info.family === 'IPv4' && !info.internal) {
                serverAddress = info.address;  // Local network IP address
            }
        });
    }

    console.log((new Date()) + ' Server is listening on ' + serverAddress + ':5000');
});

io.on('connection', (socket) => {
    console.log("Client connected");
	 
	socket.on('relay_control', (command) => {
        console.log('Relay control command received:', command);

        // Add logic here to handle relay control, e.g., sending the command to the ESP32
        if (command === "relay1_on") {
            // Add the code to turn on the relay
            console.log("Turning Relay 1 ON");
        } else if (command === "relay1_off") {
            // Add the code to turn off the relay
            console.log("Turning Relay 1 OFF");
        }
		socket.broadcast.emit('relay_control', command);
});
    // Handle button states
    socket.on('button_states', (data) => {
        try {
            console.log('Button states received:', data);
        } catch (error) {
            console.error('Invalid payload for button_states:', error);
            socket.emit('error', 'Invalid payload received for button_states');
        }
    });

    // Handle button pressed event
    socket.on('button_pressed', (message) => {
        try {
            console.log('Button pressed event received:', message);
        } catch (error) {
            console.error('Invalid payload for button_pressed:', error);
            socket.emit('error', 'Invalid payload received for button_pressed');
        }
    });

    socket.on('disconnect', () => {
        console.log('Client disconnected');
    });
});
