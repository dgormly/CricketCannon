const serverFile = require('../../server');
const express = require('express');
const http = serverFile.http;
const SerialPort = require('serialport');
const router = express.Router();
const MongoClient = require('mongodb').MongoClient;
const ObjectID = require('mongodb').ObjectID;
const json2csv = require('json2csv');
const fs = require('fs');
const io = serverFile.io;

var port;
var saveDir = "./data/shots";

// Connect
const connection = (closure) =>
{
  return MongoClient.connect('mongodb://localhost:27017/cricket', function (err, client) {
    if (err) throw err;O
    var db = client.db('cricket');
    closure(db);
  });
}

// Error handling
const sendError = (err, res) => {
  response.status = 501;
  response.message = typeof err == 'object' ? err.message : err;
  res.status(501).json(response);
};

// Response handling
let response = {
  status: 200,
  data: [],
  message: null
};

// Get users
router.get('/shots', (req, res) => {
  connection((db) => {
  db.collection('shots')
    .find()
    .toArray()
    .then((shots) => {
    response.data = shots;
  res.json(response);
  }).catch((err) => {
    sendError(err, res);
  });
  });
});

// Get serial ports.
router.get('/ports', (req, res) => {
  console.log('Ports connection');
  SerialPort.list(function (err, ports) {
    response.data = ports;
    res.json(response);
  }).catch((err) => {
    sendError(err, res);
  });
});

// Set cannon settings.
router.post('/settings', (req, res) => {
  res.send({
    status: "OK"}
  );
  console.log('Post received');
  console.log(req.body.port);
  port = new SerialPort(req.body.port, {
    baudRate: 9600
  });

  port.on('data', function(data) {
    console.log("Data received.");
    var message = data.toString();
    console.log(message);
    var arr = message.split("/");
    if (arr[0] == "3pt") {
      serverFile.scale.emit("DATA", "Hello World");
    } else {
    console.log(arr[0]);
    serverFile.client.emit(arr[0], {
       value: arr[1]
    });
  }
  });
});

// Fire Cannon, save data and return results
router.post('/fire', (req, res) => {
  let databuffer = "";

  console.log('Firing cannon.');
  let message = "FIRE/" + req.body.pressure + "/" + req.body.pressure + "\n";

  // Write data to serial port.
  port.write(message, function(err) {
    console.log(message);
    if (err) {
      res.send("ERROR");
      return console.log("Error occured writing to port, check if the comport is open.");
    }
    res.send({
      status: "OK"
    });
  });
});


router.post('/export', (req, res) => {
  console.log('Printing CSV:');
  console.log(req.body.fileName);
  console.log(req.body.data);

  var csvString = json2csv(
    {
      data: req.body.data,
      fields: req.body.fields
  });
  fs.writeFile(req.body.fileName, csvString, function(err) {
    if (err) throw err;
    console.log('file saved.');
  });
  res.send({
    status: 'OK'
  });
});

module.exports = router;
