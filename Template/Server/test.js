const io = require('socket.io-client');

// Replace with your server's URL and port
const socket = io('http://192.168.1.221:5000');

socket.on('connect', () => {
    console.log('Connected to the server');
    // Emit a ping message
    socket.emit('ping', 'Ping from my computer');
});

socket.on('pong', (msg) => {
    console.log('Pong received:', msg);
    socket.close();  // Optionally close the connection after receiving the pong
});

socket.on('disconnect', () => {
    console.log('Disconnected from the server');
});
