import { Component, OnInit, Input } from '@angular/core';
import {DataService} from '../data.service';
import {DataSource} from '@angular/cdk/collections';
import {Observable} from 'rxjs/Observable';
import {Shot, Scale} from '../Shot';
import {FireService} from '../fire.service';
import { MatTableDataSource } from '@angular/material';
import { BehaviorSubject } from 'rxjs/BehaviorSubject';
import { SelectionModel } from '@angular/cdk/collections';

@Component({
  selector: 'app-single-data',
  templateUrl: './single-data.component.html',
  styleUrls: ['./single-data.component.css']
})

export class SingleDataComponent implements OnInit {

  dataSource: MatTableDataSource<Shot>;
  displayedColumns = ['select','No.', 'ball','rw1', 'rw2', "rw3", "sum"];

  selection = new SelectionModel<Scale>(true, []);

  constructor(private fireService: FireService) {
    this.dataSource = new MatTableDataSource();
    this.fireService = fireService;
  }

  ngOnInit() {
      this.fireService.scaleData.subscribe(data => {
        //this.dataSource.data = data;
      });
  }

    /** Whether the number of selected elements matches the total number of rows. */
    isAllSelected() {
      const numSelected = this.selection.selected.length;
      const numRows = this.dataSource.data.length;
      return numSelected === numRows;
    }

      /** Selects all rows if they are not all selected; otherwise clear selection. */
  masterToggle() {
    this.isAllSelected() ?
        this.selection.clear() :
        this.fireService.cScale.value.forEach(row => this.selection.select(row));
  }

  removeSelectedRows() {
    this.selection.selected.forEach(item => {
      let index: number = this.fireService.cScale.value.findIndex(d => d === item);
      console.log(this.fireService.cScale.value.findIndex(d => d === item));
      let temp = this.fireService.cScale.value;
      temp.splice(index, 1);
      this.fireService.cScale.next(temp);
    });
    this.selection = new SelectionModel<Scale>(true, []);
  }

  applyFilter(filterValue: string) {
    filterValue = filterValue.trim(); // Remove whitespace
    filterValue = filterValue.toLowerCase(); // MatTableDataSource defaults to lowercase matches
    this.dataSource.filter = filterValue;
  }
}
