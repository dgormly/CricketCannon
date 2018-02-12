const express = require("express");
const bodyParser = require("body-parser");
const path = require("path");
module.exports.http = require("http");
const app = module.exports.app = express();
const SerialPort = require("serialport");
const Readline = SerialPort.parsers.Readline;
// API file for interacting with MongoDB
const socketIo = require("socket.io");
const json2csv = require("json2csv");
const fs = require("fs");

var serialPort;

// Parsers
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false}));

// Angular DIST output folder
app.use(express.static(path.join(__dirname, 'dist')));

// API location
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
var that = this;
io.on('connection', (socket) => {
  console.log('The client connected');
  module.exports.client = socket;

  socket.on('PORT/GET', function() {
    SerialPort.list(function (err, ports) {
      socket.emit("PORT/GET", ports);
    });
  });

  socket.on('PORT/SET', function(data) {
    let parser = new Readline();
    console.log("Setting up scale comm");
      serialPort = new SerialPort(data.port,{
        baudRate: data.baud
      }, function(err) {
        if (err) {
          that.io.emit('PORT/ERROR', 'Error on connecting to port.');
          return console.error('Error on connecting to port: ', err.message);
        }
      });
      
      serialPort.pipe(parser);
      parser.on("data", function(data) {
        let dataType = data.split(":");
        io.emit(dataType[0], dataType[1]);

        if (dataType[0] === "CANNON/RESULTS") {
          console.log("Fire results.");
        }

      });
  });


  socket.on('SCALE/TARE', function(data) {
    serialPort.write('t');;
  });

  socket.on('SCALE/SAVE', function(scaleData) {
    console.log('Printing CSV:');
  
    var csvString = json2csv(
      {
        data: scaleData.data,
        fields: scaleData.headers
    });
    csvString = csvString.replace('[','');
    
    csvString = csvString.replace(']','');
    fs.writeFile("./Data/Scale/" + scaleData.name + ".csv", csvString, function(err) {
      if (err) throw err;
      console.log('file saved.');
    });
  });

  socket.on('CANNON/FIRE', function(data) {
    console.log("Firing cannon.");
    serialPort.write("CANNON/FIRE/" + data.pressure);
  });

  socket.on('disconnect', function(){
    console.log('user disconnected');
    serialPort = null;
  });
});
