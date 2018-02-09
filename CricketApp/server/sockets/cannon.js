// Cannon IO handling;
exports.socket = (io) => {
    var cannonIO = io.of('/CANNON');

    cannonIO.on('connection', function(data) {
        console.log('Cannon client connected.'); 
    });
 
    cannonIO.on('PORTS', function(data) {
        console.log('Get ports');
    });
 
    cannonIO.on('disconnect', function(data) {
        console.log('Cannon client disconnected.');
    });
}