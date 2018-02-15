const express = require("express");
const bodyParser = require("body-parser");
const path = require("path");
const http = require("http");
const app = module.exports.app = express();
const SerialPort = require("serialport");
const Readline = SerialPort.parsers.Readline;
const socketIo = require("socket.io");
const json2csv = require("json2csv");
const fs = require("fs");
const logger = require("./server/console_logger.js");

//const sqlite3 = require("sqlite3");

// let db = new sqlite3.Database('./Data/database.db', sqlite3.OPEN_READWRITE, (err) => {
//   if (err) {
//     console.error(err.message);
//   }
//   console.log("Connected to the database.")
// });

var serialPort;

// Parsers
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false}));

// Angular DIST output folder
app.use(express.static(path.join(__dirname, 'dist')));

// API location
// Send all other requests to the Angular app
app.get('*', (req, res) => {
  logger.log("connection detected");
  res.sendFile(path.join(__dirname, './dist/index.html'));
});

logger.log("Starting server up.");
//Set Port
const port = '5000';
app.set('port', port);
logger.log("SUCCESS", "Successfully setup port");

const server = module.exports.server = module.exports.http.createServer(app);

server.listen(port, () => {
  logger.log("NORMAL",`Running on localhost:${port}`)
});


// Socket setup
logger.log("NORMAL", "Setting up sockets.");
const io = module.exports.io = socketIo(server);
var that = this;
io.on('connection', (socket) => {
  logger.log("SUCCESS", 'The client connected');
  module.exports.client = socket;

  socket.on('PORT/GET', function() {
    SerialPort.list(function (err, ports) {
      socket.emit("PORT/GET", ports);
    });
  });

  socket.on('PORT/SET', function(data) {
    let parser = new Readline();
    logger.log("NORMAL", "Setting up scale comm");
      serialPort = new SerialPort(data.port, {
        baudRate: data.baud
      }, function(err) {
        if (err) {
          socket.emit('PORT/ERROR', 'Error on connecting to port.');
          return logger.log("ERROR",'Error on connecting to port: ', err.message);
        }
      });
      
      serialPort.pipe(parser);
      parser.on("data", function(data) {
        let dataType = data.split(":");
//        logger.log("DEBUG",dataType[0].trim());
        io.sockets.emit(dataType[0].trim(), dataType[1]);

        if (dataType[0] === "CANNON/RESULTS") {
          logger.log("RESULTS","Fire results.");
        }

      });
  });


  socket.on('SCALE/TARE', function(data) {
    serialPort.write('t');;
  });

  socket.on('SCALE/SAVE', function(scaleData) {
    logger.log("NORMAL",'Printing CSV:');
  
    var csvString = json2csv(
      {
        data: scaleData.data,
        fields: scaleData.headers
    });
    csvString = csvString.replace('[','');
    
    csvString = csvString.replace(']','');
    fs.writeFile("./Data/Scale/" + scaleData.name + ".csv", csvString, function(err) {
      if (err) throw err;
      logger.log("SUCCESS",'file saved.');
    });
  });

  socket.on('CANNON/FIRE', function(data) {
    logger.log("NORMAL","Firing cannon.");
    //logger.log(data.pressure);
    serialPort.write("CANNON/FIRE:" + data.pressure);
  });

  socket.on('CANNON/STOP', function() {
    if (serialPort) {
    serialPort.write("CANNON/STOP:");
    }
  });

  socket.on('disconnect', function(){
    logger.log("ERROR", 'user disconnected');
    serialPort = null;
  });
});
