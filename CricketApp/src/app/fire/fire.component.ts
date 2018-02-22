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
  fileName: string = "";  

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
      if (that.fireService.dashCurrentShot >= that.fireService.totalShots) {
        this.toggleOff();
        this.fireService.stopCannon();
      }
    }); 
  }


  reset(): void {
    this.fireService.totalShots = 0;
    this.fireService.dashCurrentShot = 0;
    this.fireService.ballName = "";
    this.fireService.stopCannon();
  }


  beginFiring(): void {
    this.toggled = true;
    this.fire();
  }

  fire(): void {
    console.log("Firing!");
    if (!this.toggled) {
      return;
    }

    this.totalShots = this.balls * this.shots;
    var payload = {
      ballNames: this.ballNames,
      totalShots: this.totalShots,
      pressure: this.pressure,
      currentShot: this.currentShotNum
    }

    this.fireService.fireCannon(payload);
  }

  toggleOff() {
    this.toggled = false;
    this.fireService.stopCannon();
    console.log("Paused firing session.");
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
