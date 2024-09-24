const express = require('express');  
const socketIo = require('socket.io');

// Create an Express app (optional for serving files if needed)
const app = express();

// Create Socket.IO server with CORS enabled
const server = app.listen(5000, () => {
    console.log(`Server is listening on port 5000`);
});

const io = socketIo(server, {
    path: '/socket.io',
    cors: {
        origin: "*",  
        methods: ["GET", "POST"]
    }
});

io.on('connection', (socket) => {
    console.log("Client connected");

    // Handle relay control
    socket.on('relay_control', (command) => {
        console.log('Relay control command received:', command);
        if (typeof command === 'string') {
            socket.broadcast.emit('relay_control', command); // Send to all clients
        } else {
            console.error("Invalid payload type for relay_control");
            socket.emit('error', 'Invalid payload for relay_control');
        }
    });

    // Handle button states
    socket.on('button_states', (data) => {
        try {
            console.log('Button states received:', data);
        } catch (error) {
            console.error('Error in button_states:', error.message);
            socket.emit('error', 'Invalid payload received for button_states');
        }
    });

    // Handle button pressed events
    socket.on('button_pressed', (message) => {
        try {
            console.log('Button pressed event received:', message);
        } catch (error) {
            console.error('Error in button_pressed:', error.message);
            socket.emit('error', 'Invalid payload received for button_pressed');
        }
    });

    socket.on('disconnect', () => {
        console.log('Client disconnected');
    });
});
