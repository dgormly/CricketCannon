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
const colors = require("colors");

const sqlite3 = require("sqlite3");
module.exports.db = require("./server/db");

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
  console.log("[SERVER]: Connection detected".green);
  res.sendFile(path.join(__dirname, './dist/index.html'));
});

console.log("[SERVER]: Starting server up.");
//Set Port
const port = '5000';
app.set('port', port);
console.log("[SERVER]: Successfully setup port".green);

const server = http.createServer(app);

server.listen(port, () => {
  console.log(`[SERVER]: Running on localhost:${port}`)
});


// Socket setup
console.log("[SERVER]: Setting up sockets.");
const io = module.exports.io = socketIo(server);
var that = this;
io.on('connection', (socket) => {
  console.log('[SERVER]: The client connected'.green);
  module.exports.client = socket;

  socket.on('PORT/GET', function() {
    SerialPort.list(function (err, ports) {
      socket.emit("PORT/GET", ports);
    });
  });


  var currentBallId = -1;
  socket.on('PORT/SET', function(data) {
    let parser = new Readline();
    console.log("[SERVER]: Setting up scale comm");
      serialPort = new SerialPort(data.port, {
        baudRate: data.baud
      }, function(err) {
        if (err) {
          socket.emit('PORT/ERROR', 'Error on connecting to port.');
          return console.log(colors.red('[SERVER]: Error on connecting to port: ', err.message));
        }
      });
      

      serialPort.pipe(parser);
      parser.on("data", function(data) {
        let dataType = data.split(":");
//        console.log("DEBUG",dataType[0].trim());
        io.sockets.emit(dataType[0].trim(), dataType[1]);

        if (dataType[0] === "CANNON/RESULTS") {
          console.log("[RESULTS]: Fire results.".cyan);
          dataType[1].Ballid = currentBallId;
          db.addShot(dataType[1]);
          console.log("[DATABASE]: Shot data added to the database.".blue);
        } else if (dataType[0] === "CANNON/DEBUG") {
          console.log("[DEBUG]: ", dataType[1].yellow);
        }

      });
  });


  socket.on('SCALE/TARE', function(data) {
    serialPort.write('t');;
  });

  socket.on('SCALE/SAVE', function(scaleData) {
    console.log('[SERVER]: Printing CSV:');
  
    var csvString = json2csv(
      {
        data: scaleData.data,
        fields: scaleData.headers
    });
    csvString = csvString.replace('[','');
    
    csvString = csvString.replace(']','');
    fs.writeFile("./Data/Scale/" + scaleData.name + ".csv", csvString, function(err) {
      if (err) throw err;
      console.log('[SERVER]: File saved.');
    });
  });

  socket.on('CANNON/FIRE', function(data) {
    currentBallId = data.Ballid;
    console.log("Firing cannon with ball: ", data.Ballid);
    //console.log(data.pressure);
    serialPort.write("CANNON/FIRE:" + data.pressure);
  });

  socket.on('CANNON/STOP', function() {
    if (serialPort) {
    serialPort.write("CANNON/STOP:");
    }
  });

  socket.on('disconnect', function(){
    console.log('[SERVER]: User disconnected'.red);
    serialPort = null;
  });
});
