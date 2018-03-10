var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('./Data/Database/CricketDB.db');

// db.serialize(function() {
//     db.run("INSERT INTO Ball (Brand, Type) VALUES ( ?, ?)", ["Daniel", "Gormly"]);

//     db.each("SELECT * FROM Ball", function(err, row) {
//         console.log(row);
//     });
// });

exports.addShot = function(record) {
    db.serialize(function() {
        db.run("INSERT INTO Shot (Ballid, Pressure, Vin, Vout) VALUES ( ?, ?, ?, ?)", [record.ballid, record.pressure, record.vin, record.vout]);
    });
    //db.close();
}
