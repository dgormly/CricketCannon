const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
const http = require('http');
const app = express();
const socketIo = require('socket.io');
const json2csv = require('json2csv');
const SerialPort = require('serialport');
const router = express.Router();

// API file for interacting with MongoDB
//const api = require('./server/routes/api');

// Parsers
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false}));

// Angular DIST output folder
app.use(express.static(path.join(__dirname, 'scale')));

// API location
//app.use('/api', api);

// Send all other requests to the Angular app
app.get('*', (req, res) => {
  console.log("connection detected");
  res.sendFile(path.join(__dirname, './scale/index.html'));
});

console.log("Starting 3pt scale server up.");
//Set Port
const port = '5000';
app.set('port', port);
console.log("Successfully setup web port");

const server = module.exports.server = module.exports.http.createServer(app);

server.listen(port, () => {
  console.log(`Running on localhost:${port}`)
});

// Ports// Get serial ports.
router.get('/ports', (req, res) => {
  console.log('Ports connection');
  SerialPort.list(function (err, ports) {
    response.data = ports;
    res.json(response);
  }).catch((err) => {
    sendError(err, res);
  });
});


router.post('/settings', (req, res) => {
  res.send({
    status: "OK"
  });
  console.log('Com post received');
  console.log(req.body.port);
  port = new SerialPort(req.body.port, {
    baudRate: 9600
  });

  port.on('data', function(data) {
    console.log("Data received.");
    var message = data.toString();
    console.log(message);
  });
});



console.log("Setting up sockets.");
const io = socketIo(server);
io.on('connection', (socket) => {
  console.log('The client connected');

  io.on('disconnect', function(){
    console.log('user disconnected');
  });
});
