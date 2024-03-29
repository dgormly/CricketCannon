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
var cannonDB = require("./server/db");

var serialPort;

// Parsers
app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: false }));

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
var totalShots = 0;
var currentShot = 0;
var pressure = 0.0;
var ballNames;
var shotRecord = [""];
var clientConnected = false;

console.log("[SERVER]: Setting up sockets.");
const io = module.exports.io = socketIo(server);
var that = this;
io.on('connection', (socket) => {
  console.log('[SERVER]: The client connected'.green);

  // /** Check if cannon is already in use. */
  // if (clientConnected) {
  //   socket.emit("SERVER/AUTH", false);
  //   console.log("[SERVER]: Client already connected to server.".red);
  // } else {
  //   socket.emit("SERVER/AUTH", true);
  //   clientConnected = true;
  // }

  socket.on('PORT/GET', function () {
    SerialPort.list(function (err, ports) {
      socket.emit("PORT/GET", ports);
    });
  });


  socket.on('PORT/CLOSE', function () {
    if (serialPort != null && serialPort.isOpen) {
      serialPort.close();
      console.log('[SERVER]: Successfully closed Comm port.');
    }
  });

  // socket.on("SERVER/AUTH", function() {
  //   if (clientConnected) {
  //     socket.emit("SERVER/AUTH", false);
  //     console.log("[SERVER]: Client already connected to server.".red);
  //   } else {
  //     socket.emit("SERVER/AUTH", true);
  //     clientConnected = true;
  //   }

  // });

  /**
   * Set port to use for communication.
   */
  socket.on('PORT/SET', function (data) {
    let parser = new Readline();
    console.log("[SERVER]: Setting up comm with baud ", data.baud);
    serialPort = new SerialPort(data.port, {
      baudRate: data.baud
    }, function (err) {
      if (err) {
        socket.emit('PORT/ERROR', 'Error on connecting to port.');
        return console.log(colors.red('[SERVER]: Error on connecting to port: ', err.message));
      }
    });

    // Clean data that comes through serialport.
    serialPort.pipe(parser);
    parser.on("data", function (data) {
      let dataType = data.split(":");
      // This is needed incase a JSON string is sent.
      let header = dataType[0].trim();
      dataType.splice(0, 1);
      let payload = dataType.join(':');
      // Forward message on to any subscribers.
      io.sockets.emit(header, payload);

      // Handle CANNON results / CONVERT to JSON.
      if (header === "CANNON/RESULTS") {
        console.log("[RESULTS]: Fire results: ".cyan, payload);
        let payloadObj = JSON.parse(payload);
        record = {
          ballid: ballNames[currentShot % ballNames.length],
          pressure: payloadObj.PRESSURE,
          vin: payloadObj.VIN,
          vout: payloadObj.VOUT
        };

        shotRecord.push(record);
        // Add to database : Ball ID, Pressure
        cannonDB.addShot(record);
        currentShot++;
        console.log("Shot recorded.");

        // Stop cannon if all shots are fired ;)
        if (currentShot < totalShots) {
          serialPort.write("CANNON/FIRE:" + pressure);
        } else {
          console.log("[SERVER]: Stopping cannon".red);
          serialPort.write("CANNON/STOP:");
        }
        // Color message if DEBUG message comes through.
      } else if (dataType[0] === "CANNON/DEBUG") {
        console.log("[DEBUG]: ", payload.yellow);
      }

    });
  });


  /**
   * Reset 3pt. scale.
   */
  socket.on('SCALE/TARE', function (data) {
    serialPort.write('t');
  });


  /**
   * Save Scale data to csv.
   */
  socket.on('SCALE/SAVE', function (scaleData) {
    console.log('[SERVER]: Printing CSV:');

    var csvString = json2csv(
      {
        data: scaleData.data,
        fields: scaleData.headers
      });
    csvString = csvString.replace('[', '');

    csvString = csvString.replace(']', '');
    fs.writeFile("./Data/Scale/" + scaleData.name + ".csv", csvString, function (err) {
      if (err) throw err;
      console.log('[SERVER]: File saved.');
    });
  });


  /**
   *  Clear cannon data. THIS DOES NOT REMOVE IT FROM THE DATABASE.
   */
  socket.on('CANNON/CLEAR', function (data) {
    shotRecord = data;
  });


  /**
   * Set cannon settings on the server.
   * 
   * data holds ballNames, totalShots, pressure, and currentShot
   */
  socket.on('CANNON/SET', function (data) {
    console.log("[CANNON]: Setting cannon settings.".green);
    ballNames = data.ballNames;
    totalShots = data.totalShots;
    pressure = data.pressure;
    currentShot = data.currentShot;
    console.log("[DEBUG]: Cannon settings: ".yellow, data);
  });


  /**
   * Fire Cannon
   */
  socket.on('CANNON/FIRE', function(data) {
    console.log("[SERVER]: Firing cannon.");
    //console.log(data.pressure);
    serialPort.write("CANNON/FIRE:" + pressure);
  });


  /** Ready Drop test */
  socket.on('CANNON/DROP:READY', function(data) {
    serialPort.write("CANNON/DROP:READY");
  });

  socket.on('CANNON/SINGLE', function(data) {
    serialPort.write("CANNON/SINGLE:" + data)
  });

  /** Drop ball */
  socket.on('CANNON/DROP:FIRE', function (data) {
    serialPort.write("CANNON/DROP:FIRE");
  });

  /** Ready Drop test */
  socket.on('CANNON/DROP:DROP', function (data) {
    serialPort.write("CANNON/DROP:DROP");
  });



  /**
   * Stop Cannon.
   */
  socket.on('CANNON/STOP', function () {
    if (serialPort != null && serialPort.isOpen) {
      serialPort.write("CANNON/STOP:");
      console.log("[SERVER]: Stopping cannon.".red);
    }
  });

  /**
   * Save cannon data to CSV.
   */
  socket.on('CANNON/SAVE', function (data) {
    var csvString = json2csv(
      {
        data: shotRecord,
        fields: data.headers
      });
    csvString = csvString.replace('[', '');
    csvString = csvString.replace(']', '');
    fs.writeFile("./Data/Cannon/" + data.name + ".csv", csvString, function (err) {
      if (err) throw err;
      console.log('[SERVER]: File saved.');
    });
  });


  /**
   * Client disconnects from port and clears serial com.
   */
  socket.on('disconnect', function () {
    console.log('[SERVER]: User disconnected'.red);
    if (serialPort != null && serialPort.isOpen) {
      serialPort.close();
      //clientConnected = false;
      console.log('[SERVER]: Closed Serialport.'.green);
    }
  });
});