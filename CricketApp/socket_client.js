const ioc = require('socket.io-client');
const socket = ioc.connect('http://localhost:3000', {reconnect: true});

socket.on('connection', (socket) => {
    let token = socket.handshake.query.token;
    // ...
});
