import { Injectable } from '@angular/core';
import {Observable} from 'rxjs/Observable';
import {Shot, Scale } from './Shot';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import {tap} from 'rxjs/operators';
import { Subject } from 'rxjs/Subject';
import * as socketIo from 'socket.io-client';
import { AuthService } from './auth.service';


@Injectable()
export class FireService {

  ports: any = [];
  selectedPort: any;
  private cannonResults = new BehaviorSubject<string>("");
  cannonResults$ = this.cannonResults.asObservable();

  shots = new BehaviorSubject<Shot[]>([]);
  private firingSubject = new BehaviorSubject<boolean>(false);
  private firingMessage = this.firingSubject.asObservable();
  shotData = this.shots.asObservable();
  private cShots = new BehaviorSubject<Shot[]>([]);
  currentShots = this.cShots.asObservable();
  cScale = new BehaviorSubject<Scale[]>([]);
  scaleData = this.cScale.asObservable();
  socket;

  dashCurrentShot = 0;
  totalShots = 0;
  ballNames: string[] = ["ball ID"];


  private currentPressure = new BehaviorSubject<number>(0);
  currentPressure$ = this.currentPressure.asObservable();

  scaleDataSubject = new BehaviorSubject<String>("");
  tareSubject = new BehaviorSubject<Boolean>(false);

  constructor(private authService: AuthService) {
    var that = this;
    this.socket = socketIo();

    this.socket.on('SCALE/DATA', function(data) {
      let message = data.toString();
      that.scaleDataSubject.next(message);
    });


    this.socket.on('PORT/GET', function(data) {
      that.ports = data;
      console.log('Ports setup');
    });

    this.socket.on('CANNON/PRESSURE', function(data) {
      that.currentPressure.next(data);
    });


    // this.socket.on('SERVER/AUTH', function(data) {
    //   if (data == true) {
    //     that.authService.isLoggedIn = true;
    //   } else {
    //     that.authService.isLoggedIn = false;
    //   }
    // });


    this.socket.on('PORT/SET', function(data) {
      if (data.toString() === "OK") {
        console.log("Sng buiuccessfully connected scale comms");
      } else {
        console.log("Failed to connect to scale comms");
      }
    });

    this.socket.on('SCALE/TARE', function(data) {
      console.log("Tare complete!");
      that.tareSubject.next(true);
    });

    this.socket.on('CANNON/RESULTS', function(data) {
      console.log("Cannon results - client");
      // get data values to add shot.
      let payload = JSON.parse(data);
      console.log(payload);
      that.addShot(payload);
      that.dashCurrentShot++;
    });

    this.socket.on('SCALE/ERROR', function(data) {
      console.log(data);
    });

    this.socket.on('PORT/ERROR', function(data) {
      this.selectedPort = "";
    });

    this.getPorts();
  }


  setPort(portName: string, baud: number): void {
    this.socket.emit('PORT/SET', {
      port: portName,
      baud: baud
    });
  }

  closeComm() {
    this.socket.emit('PORT/CLOSE');
  }

  getPorts() {
    this.socket.emit('PORT/GET');
  }

  setCannon(payload: any): void {
    this.socket.emit('CANNON/SET', payload);
  }

  fireCannon(payload: any): void {
    this.setCannon(payload);
    this.socket.emit('CANNON/FIRE', payload.pressure);
  }

  stopCannon(): void {
    this.socket.emit('CANNON/STOP');
  }

  readyDrop(): void {
    this.socket.emit('CANNON/DROP:READY');
  }

  fireDrop(): void {
    this.socket.emit('CANNON/DROP:FIRE');
  }

  // No camera trigger
  dropDrop(): void {
    this.socket.emit('CANNON/DROP:DROP');
  }

  fireSingle(psi): void {
    this.socket.emit('CANNON/SINGLE', psi);
  }


  clearShots(): void {
    console.log('Clearing shot data.');
    this.cShots.next([]);
    this.socket.emit('CANNON/CLEAR', []);
  }


  private addShot(shotData): void {
    let shot = new Shot();
    shot.shotPressure = shotData.PRESSURE;
    shot.ballid = this.ballNames[this.dashCurrentShot % this.ballNames.length];
    shot.vin = shotData.VIN;
    shot.vout = shotData.VOUT;
    console.log('Shot added. ', shot);
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
      headers:  ['ballid', 'pressure', 'vin', 'vout']
    });
  }

  tare(): void {
    this.socket.emit('SCALE/TARE');
  }
}
