var WebSocketServer = require('websocket').server;
var http = require('http');
var fs = require('fs');  // File system module to serve HTML files

// Create an HTTP server to listen on port 5000
var server = http.createServer(function(request, response) {
    console.log((new Date()) + ' Received request for ' + request.url);

    // Serve the index.html file for root request
    if (request.url === "/") {
        fs.readFile('public/index.html', function(err, data) {
            if (err) {
                response.writeHead(500);
                response.end('Error loading index.html');
            } else {
                response.writeHead(200, { 'Content-Type': 'text/html' });
                response.end(data);
            }
        });
    } else {
        response.writeHead(404); // Respond with 404 for other URLs
        response.end();
    }
});
server.listen(5000, function() {
    console.log((new Date()) + ' Server is listening on port 5000');
});

// Create WebSocket server
var wsServer = new WebSocketServer({
    httpServer: server,
    autoAcceptConnections: false // Security: Disable auto-accepting connections
});

// Store all connections in an array
var clients = [];

// Handle WebSocket connection requests
wsServer.on('request', function(request) {
    console.log(request);  // Log the connection request for debugging

    // Check if the origin is allowed
    if (!originIsAllowed(request.origin)) {
        request.reject();
        console.log((new Date()) + ' Connection from origin ' + request.origin + ' rejected.');
        return;
    }

    // Accept the connection
    var connection = request.accept(null, request.origin);
    clients.push(connection);  // Add new client to the list
    console.log((new Date()) + ' Connection accepted.');

    // Handle messages from the client
    connection.on('message', function(message) {
        if (message.type === 'utf8') {
            console.log( message.utf8Data);

            // Broadcast message to all clients (including ESP32)
            clients.forEach(function(client) {
                if (client !== connection) {  // Don't send to the sender
                    client.sendUTF(message.utf8Data);
                }
            });
        }
    });

    // Handle connection closure
    connection.on('close', function(reasonCode, description) {
        console.log((new Date()) + ' Peer ' + connection.remoteAddress + ' disconnected.');
        
        // Remove client from the list
        clients = clients.filter(function(client) {
            return client !== connection;
        });
    });
});

// Function to check if the origin is allowed
function originIsAllowed(origin) {
    // In production, you should filter origins here. For now, allow all.
    return true;
}

