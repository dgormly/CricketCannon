import {Component, Input, OnInit, Output} from '@angular/core';
import {FireService} from '../fire.service';
import {Response} from '../Response';
import { FormGroup, Validators, FormBuilder } from '@angular/forms';
import * as async from 'async';
import { nextTick } from 'async';


@Component({
  selector: 'app-fire',
  templateUrl: './fire.component.html',
  styleUrls: ['./fire.component.css']
})
export class FireComponent implements OnInit {

  toggled = false;
  portNames: Response[] = [];
  selectedPort: string;
  checked = false;
  isLinear = true;

  balls: string;
  shots: string;

  constructor(private fireService: FireService, private _formBuilder: FormBuilder) { 
    this.fireService.isFiring(this.toggled);
  }

  ngOnInit() {
    this.fireService.getPorts().subscribe(val => {
      this.portNames = val.data
    });
  }

  updateSettings(): void {
    console.log('button pressed');
    this.fireService.postSettings(this.selectedPort);
  }

  clear(): void {
    this.fireService.clearShots();
  }

  fire(): void {
    var count = 0;
    this.toggled = !this.toggled;
    console.log('Firing!');
    console.log(this.balls);
    this.fireService.isFiring(this.toggled);

    async.eachSeries([...Array(this.shots)].keys(), (key, next) => { 
      this.fireService.fireCannon().subscribe(() => {
        console.log(key);
        next()  
      })   /* <---- critical piece.  This is how the forEach knows to continue to
                           the next loop.  Must be called inside search's callback so that
                           it doesn't loop prematurely.*/
    }, (err) => {
      if (err) throw err;
      console.log('Session Finished');
      this.toggleOff();
    });
  } 


  saveToCsv(location: string): void {
    this.fireService.saveFile(location);
  }

  toggleOff() {
    this.toggled = false;
    this.fireService.isFiring(this.toggled);
  }
}
