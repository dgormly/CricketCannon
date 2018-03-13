import { Component, OnInit } from '@angular/core';
import { FireService } from '../fire.service';

@Component({
  selector: 'app-single',
  templateUrl: './single.component.html',
  styleUrls: ['./single.component.css']
})
export class SingleComponent implements OnInit {

  private dropPressure: number = 0;

  constructor(public fireService: FireService) { }

  ngOnInit() {
  }

}
