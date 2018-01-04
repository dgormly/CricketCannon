import { Component, OnInit } from '@angular/core';
import {DataService} from '../data.service';
import {DataSource} from '@angular/cdk/collections';
import {Observable} from 'rxjs/Observable';
import { SHOTS } from '../mock-shots';
import {Shot} from '../Shot';
import {FireService} from '../fire.service';

@Component({
  selector: 'app-data',
  templateUrl: './data.component.html',
  styleUrls: ['./data.component.css']
})

export class DataComponent implements OnInit {

  dataSource: ShotDataSource;
  displayedColumns = ['id', 'name'];

  constructor(private fireService: FireService) {
    this.dataSource = new ShotDataSource(this.fireService);
  }

  ngOnInit() {
  }

}


export class ShotDataSource extends DataSource<any> {

  constructor(private fireService: FireService) {
    super();
  }

  connect(): Observable<Shot[]> {
    return this.fireService.shotData;
  }

  disconnect(): void {}

}
