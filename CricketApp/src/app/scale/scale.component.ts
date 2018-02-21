import { Component, OnInit, ViewChild } from '@angular/core';
//import { NgbModule } from '@ng-bootstrap/ng-bootstrap';
import { FireService } from '../fire.service';
import { Scale } from './../Shot';

@Component({
    selector: 'app-scale',
    templateUrl: './scale.component.html',
    styleUrls: ['./scale.component.css']
})
export class ScaleComponent implements OnInit {

    fileName: string = "";
    ballName: string = "";

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
    sampleNo: number = 0;
    count = 0;

    type = 'scatter';
    data: {
        datasets: [{
            label: 'Scatter Dataset',
            data: [{
                x: -0.5,
                y: 0
            }, {
                    x: 0,
                    y: 0.7
                }, {
                    x: 0.1,
                    y: 0.8
                }]
        }]
    }
    options: {
        scales: {
            xAxes: [{
                type: 'linear',
                position: 'bottom'
            }]
        }
    }


    hasTared: Boolean;

    constructor(private fireService: FireService) {
    }


    ngOnInit() {
        this.hasTared = this.fireService.tareSubject.getValue();

        this.fireService.scaleDataSubject.subscribe(val => {
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
        while (count < 10) {
            total += arr.pop();
            count++;
        }
        return total /= 10;
    }

    tare() {
        this.fireService.tare();
    }


    displaySample(ballName: string) {
        var scale = new Scale();
        scale.ball = ballName;
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
        scale.sampleNo = this.sampleNo;

        this.fireService.addScale(scale);
        this.sampleNo++;
        this.count = this.rw1.length;
    }

    saveScaleData() {
        this.fireService.saveScaleData(this.fileName);
        this.fireService.clearScaleData();
        this.sampleNo = 0;
    }
}
