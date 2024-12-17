const express = require('express');
const http = require('http');
const { Server } = require('socket.io');

const app = express();
const server = http.createServer(app);
const io = new Server(server);

// Handle Socket.IO connections
io.on('connection', (socket) => {
    console.log(`Client connected: ${socket.id}`);

    // Generic message handler: forwards any JSON event and data
    socket.onAny((eventName, data) => {
        console.log(`Received Event: ${eventName}, Data: ${JSON.stringify(data)}`);

        // Forward the event and data to all connected clients (except the sender)
        socket.broadcast.emit(eventName, data);

        // Optionally forward back to the sender as well
        // io.emit(eventName, data);  // Uncomment this if you want it sent to all clients, including the sender
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
