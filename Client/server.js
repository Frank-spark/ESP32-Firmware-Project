const express = require('express');
const http = require('http');
const socketIo = require('socket.io');
const fs = require('fs');
const path = require('path');

// Create express app
const app = express();

// Create HTTP server
const server = http.createServer(app);

// Create Socket.IO server
const io = socketIo(server);

// Serve the firmware file for updates
app.get('/firmware.bin', (req, res) => {
  const firmwarePath = path.join(__dirname, 'firmware.bin');
  if (fs.existsSync(firmwarePath)) {
    res.sendFile(firmwarePath);
  } else {
    res.status(404).send('Firmware not found');
  }
});

// Handle WebSocket connections
io.on('connection', (socket) => {
  console.log('ESP32 client connected');

  // Handle incoming events from ESP32
  socket.on('event', (data) => {
    console.log('Received event:', data);
    // Send a response back to ESP32
    socket.emit('response', 'Acknowledged');
  });

  // Handle client disconnect
  socket.on('disconnect', () => {
    console.log('ESP32 client disconnected');
  });
});

// Start the server on port 3000
server.listen(3000, () => {
  console.log('JS server listening on port 3000');
});
