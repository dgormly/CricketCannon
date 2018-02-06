const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
module.exports.http = require('http');
const app = module.exports.app = express();
const SerialPort = require('serialport');

// API file for interacting with MongoDB
const api = require('./server/routes/api');
const socketIo = require('socket.io');

var scalePort;

// Parsers
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false}));

// Angular DIST output folder
app.use(express.static(path.join(__dirname, 'dist')));

// API location
app.use('/api', api);

// Send all other requests to the Angular app
app.get('*', (req, res) => {
  console.log("connection detected");
  res.sendFile(path.join(__dirname, './dist/index.html'));
});

console.log("Starting server up.");
//Set Port
const port = '5000';
app.set('port', port);
console.log("Successfully setup port");

const server = module.exports.server = module.exports.http.createServer(app);

server.listen(port, () => {
  console.log(`Running on localhost:${port}`)
});


// Socket setup
console.log("Setting up sockets.");
const io = module.exports.io = socketIo(server);
io.on('connection', (socket) => {
  console.log('The client connected');
  module.exports.client = socket;

  socket.on('PORT', function() {
    console.log("Connected with port")
  });

  socket.on('disconnect', function(){
    console.log('user disconnected');
  });
});

// 3pt scale socket
var scale = io.of('/3pt');
module.exports.scale = scale;
scale.on('connection', function(socket) {
  console.log(console.log("Java client connected."));

  socket.on("PORT", function(data) {
    port = new SerialPort(data, {
      baudRate: 115200
    });
    console.log("New com set");
    socket.emit("SUCCESS", "OK");
  });

  socket.on('disconnect', function(){
    console.log('3pt disconnected');
  });
});



// Testing
scalePort = new SerialPort("COM10", {
  baudRate: 115200
});

scalePort.on("readable", function(data) {
  console.error(scalePort.read().toString());
})
