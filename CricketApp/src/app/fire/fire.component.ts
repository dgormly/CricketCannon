import {Component, Input, OnInit, Output} from '@angular/core';
import {FireService} from '../fire.service';
import {Response} from '../Response';
import { FormGroup, Validators, FormBuilder } from '@angular/forms';

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

  fire(): void {
    this.toggled = !this.toggled;
    console.log(this.toggled);
    this.fireService.isFiring(this.toggled);
    this.fireService.fireCannon();
  }

  saveToCsv(location: string): void {
    this.fireService.saveFile(location);
  }

  toggleOff() {
    this.toggled = false;
    this.fireService.isFiring(this.toggled);
  }
}
