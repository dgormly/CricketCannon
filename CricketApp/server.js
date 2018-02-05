const express = require('express');
const bodyParser = require('body-parser');
const path = require('path');
module.exports.http = require('http');
const app = module.exports.app = express();

// API file for interacting with MongoDB
const api = require('./server/routes/api');
const socketIo = require('socket.io');

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


console.log("Setting up sockets.");
const io = module.exports.io = socketIo(server);
io.on('connection', (socket) => {
  console.log('The client connected');
  module.exports.client = socket;
  io.on('disconnect', function(){
    console.log('user disconnected');
  });
});
