import { Component, OnInit, Input } from '@angular/core';
import {DataService} from '../data.service';
import {DataSource} from '@angular/cdk/collections';
import {Observable} from 'rxjs/Observable';
import {Shot} from '../Shot';
import {FireService} from '../fire.service';
import { MatTableDataSource } from '@angular/material';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';

@Component({
  selector: 'app-data',
  templateUrl: './data.component.html',
  styleUrls: ['./data.component.css']
})

export class DataComponent implements OnInit {

  dataSource: MatTableDataSource<any>;
  displayedColumns = ['id', 'name'];
  @Input() allData = true;


  constructor(private fireService: FireService) {
    this.dataSource = new MatTableDataSource();
    this.fireService = fireService;
  }

  ngOnInit() {
    if (this.allData == true) {
      this.fireService.shotData.subscribe(data => {
        this.dataSource.data = data;
      });
    } else {  
      this.fireService.currentShots.subscribe(data => {
        this.dataSource.data = data;
      });
    }
  }

  applyFilter(filterValue: string) {
    filterValue = filterValue.trim(); // Remove whitespace
    filterValue = filterValue.toLowerCase(); // MatTableDataSource defaults to lowercase matches
    this.dataSource.filter = filterValue;
  }
}
