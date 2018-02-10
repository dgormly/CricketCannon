import {Component, Input, OnInit, Output, ViewChild} from '@angular/core';
import {FireService} from '../fire.service';
import { FormGroup, Validators, FormBuilder } from '@angular/forms';
import * as async from 'async';
import { nextTick } from 'async';


@Component({
  selector: 'app-fire',
  templateUrl: './fire.component.html',
  styleUrls: ['./fire.component.css']
})
export class FireComponent implements OnInit {

  @ViewChild('stepper') stepper;

  toggled = false;
  selectedPort: string;
  checked = false;

  balls: number = 1;
  shots: number = 1;
  currentShotNum = 0;
  pressure = 0;
  ballNames: string[] = ["ball 0"];
  hasFired: boolean = false;

  firstFormGroup: FormGroup;
  secondFormGroup: FormGroup;
  thirdFormGroup: FormGroup;

  constructor(private fireService: FireService, private _formBuilder: FormBuilder) {
  }

  ngOnInit() {
    this.firstFormGroup = this._formBuilder.group({
      firstCtrl: ['', Validators.required]
    });
    this.secondFormGroup = this._formBuilder.group({
      secondCtrl: ['', Validators.required]
    });
    this.thirdFormGroup = this._formBuilder.group({
      thirdCtrl: ['', Validators.required]
    });
  }


  fire(pressure: number): void {
    // var count = 0;
    // this.toggled = !this.toggled;
    // console.log('Firing!');
    // console.log(this.balls);
    // this.fireService.isFiring(this.toggled);

    // async.eachSeries([...Array(this.shots * this.balls - this.currentShotNum)].keys(), (key, next) => {
    //   if (!this.toggled) return;
    //   this.fireService.fireCannon( this.pressure, this.ballNames[key % this.ballNames.length]).subscribe(() => {
    //     console.log(key);
    //     this.currentShotNum++;
    //     setTimeout(function() {
    //       next();
    //     }, 1000)
    //   })   /* <---- critical piece.  This is how the forEach knows to continue to
    //                        the next loop.  Must be called inside search's callback so that
    //                        it   doesn't loop prematurely.*/
    // }, (err) => {
    //   if (err) throw err;
    //   console.log('Session Finished');
    //   this.toggleOff();
    //   this.changeStep(4);
    // });
  }

  toggleOff() {
    this.toggled = false;
    this.fireService.isFiring(this.toggled);
  }

  sleep(ms){
    return new Promise(resolve=>{
        setTimeout(resolve,ms)
    })
  }

  setBallNum(numBrands: number) {
    console.log('number: ', this.ballNames);
    this.ballNames = Array(numBrands).fill("name").map((x,i)=>"ball " + i);
    console.log(this.ballNames);
  }

  /**
   * Changes the step to the index specified
   * @param {number} index The index of the step
   */
  changeStep(index: number) {
    this.stepper.selectedIndex = index;
  }

  trackByFn(index: any, item: any) {
    return index;
 }
}
