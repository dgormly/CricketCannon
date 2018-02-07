import { Injectable } from '@angular/core';
import {Observable} from 'rxjs/Observable';
import {Shot, Scale } from './Shot';
import {Response} from './Response';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import {tap} from 'rxjs/operators';
import {SHOTS} from './mock-shots';
import { Subject } from 'rxjs/Subject';
import * as socketIo from 'socket.io-client';


@Injectable()
export class FireService {

  private serialPortUrl = '/api/ports';
  private ports: any;
  shots = new BehaviorSubject<Shot[]>(SHOTS);
  private firingSubject = new BehaviorSubject<boolean>(false);
  firingMessage = this.firingSubject.asObservable();
  shotData = this.shots.asObservable();
  private cShots = new BehaviorSubject<Shot[]>([]);
  currentShots = this.cShots.asObservable();
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

  getPorts(): Observable<Response> {
    return this.http.get<Response>(this.serialPortUrl)
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

  fireCannon(pressureValue: number, ballName: string): Observable<Object> {
    return this.http.post<Shot>('/api/fire',
    {
       pressure: pressureValue
    }, this.httpOptions)
    .pipe(tap(data => {
      console.log(data);

    }));
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


  clearShots(): void {
    console.log('Clearing shot data.');
    this.cShots.next([]);
  }


  private addShot(ballName: string, shot: Shot): void {
    console.log('Shot added.');
    shot.name = ballName;
    this.shots.next(this.shots.getValue().concat(shot));
    this.cShots.next(this.cShots.getValue().concat(shot));
  }

  addScale(scale: Scale): void {
    console.log('Scale added.');
    this.cScale.next(this.cScale.getValue().concat(scale));
  }

  clearScale(): void {
    this.cScale.next(this.cScale.getValue().concat([]));
  }


  tare(): void {
    this.socket.emit('TARE');
  }
}
