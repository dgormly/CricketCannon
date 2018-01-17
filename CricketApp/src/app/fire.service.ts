import { Injectable } from '@angular/core';
import {Observable} from 'rxjs/Observable';
import {Shot} from './Shot';
import {Response} from './Response';
import {HttpClient, HttpHeaders} from '@angular/common/http';
import {BehaviorSubject} from 'rxjs/BehaviorSubject';
import {tap} from 'rxjs/operators';
import {SHOTS} from './mock-shots';
import { Subject } from 'rxjs/Subject';

@Injectable()
export class FireService {

  private serialPortUrl = '/api/ports';
  private ports: Response;
  shots = new BehaviorSubject<Shot[]>(SHOTS);
  private firingSubject = new BehaviorSubject<boolean>(false);
  firingMessage = this.firingSubject.asObservable();
  shotData = this.shots.asObservable();
  private cShots = new BehaviorSubject<Shot[]>([]);
  currentShots = this.cShots.asObservable();

  private httpOptions = {
    headers: new HttpHeaders({ 'Content-Type': 'application/json'})
  };

  constructor(private http: HttpClient) { }

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

  fireCannon(ballName: string): Observable<Shot> {
    return this.http.get<Shot>('/api/fire')
    .pipe(tap(data => {
      this.addShot(ballName, data)
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
}
