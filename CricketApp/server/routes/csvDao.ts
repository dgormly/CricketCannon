const json2csv = require('json2csv');
const fs = require('fs');

export class csvDao {

    headers: string[];
    data: object[];
    csv: string;

    constructor(headers: string[], data: object[]) {
      this.headers = headers;
      this.data = data;
      this.csv = this.generateCSVString();
    }

    generateCSVString(): string {
      this.csv = json2csv({ data: this.data, fields: this.data });
      return this.csv
    }
    
    writeToFile(csvString: string): void {
        console.log(csvString);
        fs.writeFile('file.csv', csvString, function(err) {
            if (err) throw err;
            console.log('file saved');
          });
    }
}

exports.csvDao = csvDao;