import { Injectable } from '@angular/core';
import {Observable} from 'rxjs/Observable';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import {tap} from 'rxjs/operators';
import { Subject } from 'rxjs/Subject';
import * as socketIo from 'socket.io-client';


@Injectable()
export class SocketService {

  private serialPortUrl = '/api/ports';
  private ports: any;
  private firingSubject = new BehaviorSubject<boolean>(false);
  firingMessage = this.firingSubject.asObservable();
  private cScale = new BehaviorSubject<Scale[]>([]);
  scaleData = this.cScale.asObservable();
  private socket;
  currentPressure;
  scaleDataSubject = new BehaviorSubject<String>("");
  tareSubject = new BehaviorSubject<Boolean>(false);

  private httpOptions = {
    headers: new HttpHeaders({ 'Content-Type': 'application/json'})
  };

  constructor(private http: HttpClient) {
  }

  connectSocket(): void {
    var that = this;
    this.socket = socketIo('http://localhost:5000');
    this.socket.on('RESULT', function(data) {
      console.log("Client rec data:");
      var message = data.toString();
      console.log(message);
      //this.currentPressure = data;
    });


    this.socket.on('SCALE', function(data) {
      let message = data.toString();
      that.scaleDataSubject.next(message);
    });

    this.socket.on('PORTS', function(data) {
      console.log(data.toString());
      this.ports = data;
    });


    this.socket.on('SPORTS', function(data) {
      if (data.toString() === "OK") {
        console.log("Successfully connected scale comms");
      } else {
        console.log("Failed to connect to scale comms");
      }
    });

    this.socket.on('TARE', function(data) {
      console.log("Tare complete!");
      that.tareSubject.next(true);
    })
  }

  setScalePort(portName: string) {
    this.socket.emit('SPORTS', portName);
  }

  getPorts(): Observable<any> {
    return this.http.get<any>(this.serialPortUrl)
      .pipe(
        tap(ports => {
        console.log(ports);
        this.ports = ports;
      }));
  }

  postSettings(portName: string): void {
    console.log("performing post.");
    this.http.post('/api/settings',
    {
      port: portName
    }, this.httpOptions).subscribe();
  }


  isFiring(data: boolean): void{
    this.firingSubject.next(data);
  }

  saveFile(filePath: string): void {
    this.http.post<object>('/api/export',
    {
      "data": [{
        "id": "hello",
        "name": "world"
      }],
      "headers": ["id", "name"],
      "fileName": filePath
    }, this.httpOptions).subscribe();
  }


  addScale(scale: Scale): void {
    console.log('Scale added.');
    this.cScale.next(this.cScale.getValue().concat(scale));
  }

  clearScaleData(): void {
    this.cScale.next([]);
  }

  saveScaleData(): void {
    this.socket.emit('SAVESCALE', {
      name: "test",
      headers:  ['rw1', 'rw2', "rw3", "sum", "w1x", "w1y", "w2x", "w2y", "w3x", "w3y", "deltaX", "deltaY", "distX", "distY"],
      data: this.cScale.getValue()
    });
  }


  tare(): void {
    this.socket.emit('TARE');
  }
}

export class Scale {
  rw1: number;
  rw2: number;
  rw3: number;
  sum: number;
  w1x: number;
  w1y: number;
  w2x: number;
  w2y: number;
  w3x: number;
  w3y: number;
  deltaX: number;
  deltaY: number;
  distX: number;
  distY: number;
}