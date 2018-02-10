import { Injectable } from '@angular/core';
import {Observable} from 'rxjs/Observable';
import {Shot, Scale } from './Shot';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import {tap} from 'rxjs/operators';
import {SHOTS} from './mock-shots';
import { Subject } from 'rxjs/Subject';
import * as socketIo from 'socket.io-client';


@Injectable()
export class FireService {

  ports: any = [];
  selectedPort: any = "";

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

  constructor(private http: HttpClient) {
    var that = this;
    this.socket = socketIo('http://localhost:5000');

    this.socket.on('SCALE/DATA', function(data) {
      let message = data.toString();
      that.scaleDataSubject.next(message);
    });

    this.socket.on('PORT/GET', function(data) {
      that.ports = data;
      console.log('Ports setup');
    });


    this.socket.on('PORT/SELECT', function(data) {
      if (data.toString() === "OK") {
        console.log("Successfully connected scale comms");
      } else {
        console.log("Failed to connect to scale comms");
      }
    });

    this.socket.on('SCALE/TARE', function(data) {
      console.log("Tare complete!");
      that.tareSubject.next(true);
    });

    this.socket.on('SCALE/ERROR', function(data) {
      console.log(data);
    });
    this.getPorts();
  }


  setPort(portName: string) {
    this.socket.emit('PORT/SELECT', portName);
  }

  getPorts() {
    this.socket.emit('PORT/GET');
  }


  fireCannon(pressureValue: number, ballName: string): Observable<Object> {
    return null;
  }

  isFiring(data: boolean): void{
    this.firingSubject.next(data);
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


  clearScaleData(): void {
    this.cScale.next([]);
  }

  saveScaleData(fileName: string): void {
    this.socket.emit('SCALE/SAVE', {
      name: fileName,
      headers:  ['ball', 'rw1', 'rw2', "rw3", "sum", "w1x", "w1y", "w2x", "w2y", "w3x", "w3y", "deltaX", "deltaY", "distX", "distY"],
      data: this.cScale.getValue()
    });
  }

  saveCannonData(fileName: string): void {
    this.socket.emit('CANNON/SAVE', {
      name: fileName,
      headers:  ['ball', 'rw1', 'rw2', "rw3", "sum", "w1x", "w1y", "w2x", "w2y", "w3x", "w3y", "deltaX", "deltaY", "distX", "distY"],
      data: this.cScale.getValue()
    });
  }

  tare(): void {
    this.socket.emit('SCALE/TARE');
  }
}
