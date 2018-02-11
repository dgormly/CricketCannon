import { Component, OnInit } from '@angular/core';
//import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import {SocketService, Scale} from '../socket.service';

@Component({
  selector: 'app-dashboard',
  templateUrl: './dashboard.component.html',
  styleUrls: ['./dashboard.component.css']
})
export class DashboardComponent implements OnInit {

  portNames: Response[] = [];
  selectedPort: string;
  fileName: string;

  showData;
  rw1: number[] = [];
  rw2: number[] = [];
  rw3: number[] = [];
  sum: number[] = [];
  w1x: number[] = [];
  w1y: number[] = [];
  w2x: number[] = [];
  w2y: number[] = [];
  w3x: number[] = [];
  w3y: number[] = [];
  deltaX: number[] = [];
  deltaY: number[] = [];
  distX: number[] = [];
  distY: number[] = [];
  count = 0;


  hasTared: Boolean;

  chart = [];

  constructor(public socketService: SocketService) {

  }


  ngOnInit() {
    this.getPorts();
    this.hasTared = this.socketService.tareSubject.getValue();

    this.socketService.scaleDataSubject.subscribe(val => {
      let message = val.split('/');
      this.rw1.push(Number(message[1]));
      this.rw2.push(Number(message[2]));
      this.rw3.push(Number(message[3]));
      this.sum.push(Number(message[5]));
      this.w1x.push(Number(message[7]));
      this.w1y.push(Number(message[8]));
      this.w2x.push(Number(message[10]));
      this.w2y.push(Number(message[11]));
      this.w3x.push(Number(message[13]));
      this.w3y.push(Number(message[14]));
      this.deltaX.push(Number(message[16]));
      this.deltaY.push(Number(message[18]));
      this.distX.push(Number(message[20]));
      this.distY.push(Number(message[22]));
      this.count++;
    });
  }

  private sumSample(arr: number[]): number {
    let total = 0;
    let count = 0
    while(count < 10) {
      total += arr.pop();
      count++;
    }
    return total /= 10;
  }

  getPorts() {
    this.socketService.getPorts().subscribe(val => {
      return this.portNames = val.data
    });
  }


  updateSettings(): void {
    console.log('Port updated');
    this.socketService.postSettings(this.selectedPort);
  }

  tare() {
    this.socketService.tare();
  }


  displaySample() {
      var scale = new Scale();
      scale.rw1 = this.sumSample(this.rw1);
      scale.rw2 = this.sumSample(this.rw2);
      scale.rw3 = this.sumSample(this.rw3);
      scale.sum = this.sumSample(this.sum);
      scale.w1x = this.sumSample(this.w1x);
      scale.w1y = this.sumSample(this.w1y);
      scale.w2x = this.sumSample(this.w2x);
      scale.w2y = this.sumSample(this.w2y);
      scale.w3x = this.sumSample(this.w3x);
      scale.w3y = this.sumSample(this.w3y);
      scale.deltaX = this.sumSample(this.deltaX);
      scale.deltaY = this.sumSample(this.deltaY);
      scale.distX = this.sumSample(this.distX);
      scale.distY = this.sumSample(this.distY);
      this.socketService.addScale(scale);
      this.count = this.rw1.length;
  }

  saveScaleData() {
    this.socketService.saveScaleData();
    this.socketService.clearScaleData()
  }
}
