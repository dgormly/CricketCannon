import { Component, OnInit, Input } from '@angular/core';
import {DataService} from '../data.service';
import {DataSource} from '@angular/cdk/collections';
import {Observable} from 'rxjs/Observable';
import {Shot, Scale} from '../Shot';
import {FireService} from '../fire.service';
import { MatTableDataSource } from '@angular/material';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';

@Component({
  selector: 'app-ScaleData',
  templateUrl: './ScaleData.component.html',
  styleUrls: ['./ScaleData.component.css']
})

export class ScaleDataComponent implements OnInit {

  dataSource: MatTableDataSource<Scale>;
  displayedColumns = ['rw1', 'rw2', "rw3", "sum", "w1x", "w1y", "w2x", "w2y", "w3x", "w3y", "deltaX", "deltaY", "distX", "distY"];


  constructor(private fireService: FireService) {
    this.dataSource = new MatTableDataSource();
    this.fireService = fireService;
  }

  ngOnInit() {
      this.fireService.scaleData.subscribe(data => {
        this.dataSource.data = data;
      });
  }

  applyFilter(filterValue: string) {
    filterValue = filterValue.trim(); // Remove whitespace
    filterValue = filterValue.toLowerCase(); // MatTableDataSource defaults to lowercase matches
    this.dataSource.filter = filterValue;
  }
}
