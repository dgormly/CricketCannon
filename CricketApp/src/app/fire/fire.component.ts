import {Component, Input, OnInit, Output, ViewChild, ChangeDetectorRef} from '@angular/core';
import { FireService } from '../fire.service';
import { FormGroup, Validators, FormBuilder } from '@angular/forms';


@Component({
  selector: 'app-fire',
  templateUrl: './fire.component.html',
  styleUrls: ['./fire.component.css']
})
export class FireComponent implements OnInit {

  @ViewChild('stepper') stepper;

  toggled = false;
  checked = false;

  balls: number = 0;
  shots: number = 0;
  currentShotNum = 0;
  totalShots = 0;
  pressure: number = 0;
  ballNames: string[] = ["ball ID"];

  firstFormGroup: FormGroup;
  secondFormGroup: FormGroup;
  thirdFormGroup: FormGroup;

  constructor(private fireService: FireService, private _formBuilder: FormBuilder, private cdr: ChangeDetectorRef) {

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

    var that = this;
    this.fireService.cannonResults$.subscribe( data => {
      that.currentShotNum++;
      if (that.currentShotNum < that.totalShots) {
        that.fire(that.pressure);
      } else {
        this.toggleOff();
      }
    });
  
  }

  beginFiring(pressure: number): void {
    this.toggled = true;
    this.fire(pressure);
  }

  fire(pressure: number): void {
    console.log("Firing!");
    if (!this.toggled) {
      return;
    }

    this.totalShots = this.balls * this.shots;
    this.fireService.fireCannon(pressure, this.ballNames[this.currentShotNum % this.balls]);
  }

  toggleOff() {
    this.toggled = false;
    this.fireService.stopCannon();
    console.log("Finished firing session.");
  }

  sleep(ms){
    return new Promise(resolve=>{
        setTimeout(resolve,ms)
    })
  }

  setBallNum(numBrands: number) {
    console.log('number: ', this.ballNames);
    this.ballNames = Array(numBrands).fill("name").map((x,i)=>"ball ID");
    console.log(this.ballNames);
    this.cdr.detectChanges();
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
