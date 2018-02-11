//server.js
var app = require('express')();
var http = require('http').Server(app);
var io = require('socket.io')(http);

// Handle namespaces.
const cannon = require('./server/sockets/cannon');
cannon.socket(io);
io.on('connection', (socket) => {
    
    console.log("Client connected");
});


http.listen(3000, function () {
    console.log('listening on *:3000');
  });

