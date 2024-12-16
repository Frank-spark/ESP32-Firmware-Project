const express = require('express');
const http = require('http');
const { Server } = require('socket.io'); // Import Socket.IO

const app = express();
const server = http.createServer(app);

// Initialize Socket.IO
const io = new Server(server, {
    cors: {
        origin: '*', // Allow all origins for development/testing
        methods: ['GET', 'POST'], // Specify allowed HTTP methods
    },
});

// Socket.IO event handlers
io.on('connection', (socket) => {
    console.log(`A client connected: ${socket.id}`);

    // Handle "ping" event
    socket.on('ping', (message) => {
        console.log(`Ping received: ${message}`);
        socket.emit('pong', 'Pong from Server');
    });

    // Handle disconnection
    socket.on('disconnect', () => {
        console.log(`Client disconnected: ${socket.id}`);
    });
});

// Start the server
const PORT = 5000;
server.listen(PORT, () => {
    console.log(`Server is running on http://192.168.1.221:${PORT}`);
});
