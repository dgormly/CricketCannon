var sqlite3 = require('sqlite3').verbose();
var db = new sqlite3.Database('../Data/Database/CricketDB.db');

function addShot(shot) {
    db.serialize(function() {
        db.run("INSERT INTO Ball (Ballid, Pressure, Vin, Vout) VALUES ( ?, ?, ?, ?)", [shot.Ball, shot.Pressure, shot.Vin, shot.Vout]);
    });
    db.close();
}
