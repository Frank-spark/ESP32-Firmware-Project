var http = require('http');
var socketIo = require('socket.io');
var os = require('os');  // Used to get local network IP addresses

// Create an HTTP server
var server = http.createServer();

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

// Handle WebSocket connections
io.on('connection', (socket) => {
    console.log("Client connected");

     // Send a custom pong event
    socket.on('ping', (message) => {
        console.log('Ping received:', message);
        socket.emit('hello', 'Pong response from server');  // Respond with a custom pong

    });
	socket.on('pong_received', (message) => {
        console.log('Pong received:', message);

    });
    // Handle client disconnection
    socket.on('disconnect', () => {
        console.log('Client disconnected');
    });
});
