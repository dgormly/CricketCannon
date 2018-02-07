import { Component, OnInit } from '@angular/core';
//import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import {FireService} from '../fire.service';
import { Scale } from './../Shot';

@Component({
  selector: 'app-scale',
  templateUrl: './scale.component.html',
  styleUrls: ['./scale.component.css']
})
export class ScaleComponent implements OnInit {

  portNames: Response[] = [];
  selectedPort: string;

  showData;
  count: number = 0;
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


  hasTared: Boolean;

  sampleData: any;
  sample: boolean = false;
  sampleCount: number = 0;

  constructor(private fireService: FireService) {

  }


  ngOnInit() {
    this.getPorts();
    this.sample = false;
    this.showData = false;
    this.hasTared = this.fireService.tareSubject.getValue();

    this.fireService.scaleDataSubject.subscribe(val => {
      this.showData = true;
      let message = val.split('/');
      if (this.sample == false) {
      this.rw1.push(Number(message[1]));
      this.rw2.push(Number(message[2]));
      this.rw3.push(Number(message[3]));
      this.sum.push(Number(message[5]));
      this.w1x.push(Number(message[7]));
      this.w1y.push(Number(message[8]));
      this.w2x.push(Number(message[10]));
      this.w2y.push(Number(message[11]));
      this.w1x.push(Number(message[13]));
      this.w1y.push(Number(message[14]));
      this.deltaX.push(Number(message[16]));
      this.deltaY.push(Number(message[18]));
      this.distX.push(Number(message[20]));
      this.distY.push(Number(message[22]));
      this.count++;
      }
    });
  }

  private sumSample(arr: number[]): number {
    let total = 0;
    for(var i = 0; i < 10; i++) {
      total +=arr.pop()
    }
    return total /= 10;
  }

  getPorts() {
    this.fireService.getPorts().subscribe(val => {
      return this.portNames = val.data
    });
  }


  updateSettings(): void {
    console.log('Port updated');
    this.fireService.postSettings(this.selectedPort);
  }

  tare() {
    this.fireService.tare();
  }

  clearData() {
    this.rw1 = [];
    this.rw2 = [];
    this.rw3 = [];
    this.sum = [];
    this.w1x = [];
    this.w1y = [];
    this.w2x = [];
    this.w2y = [];
    this.w3x = [];
    this.w3y = [];
    this.deltaX = [];
    this.deltaY = [];
    this.distX = [];
    this.distY = [];
    this.count = 0;
  }

  displaySample() {
      var scale = new Scale();
      this.rw1 = scale.rw1 = [this.sumSample(this.rw1)];
      this.rw2 = scale.rw2 = [this.sumSample(this.rw2)];
      this.rw3 = scale.rw3 = [this.sumSample(this.rw3)];
      this.sum = scale.sum = [this.sumSample(this.sum)];
      this.w1x = scale.w1x = [this.sumSample(this.w1x)];
      this.w1y = scale.w1y = [this.sumSample(this.w1y)];
      this.w2x = scale.w2x = [this.sumSample(this.w2x)];
      this.w2y = scale.w2y = [this.sumSample(this.w2y)];
      this.w3x = scale.w3x = [this.sumSample(this.w1x)];
      this.w3y = scale.w3y = [this.sumSample(this.w1y)];
      this.deltaX = scale.deltaX = [this.sumSample(this.deltaX)];
      this.deltaY = scale.deltaY = [this.sumSample(this.deltaY)];
      this.distX = scale.distX = [this.sumSample(this.distX)];
      this.distY = scale.distY = [this.sumSample(this.distY)];
      this.fireService.addScale(scale);
      this.clearData();
  }
}
