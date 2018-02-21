import { Component, OnInit } from '@angular/core';
import { FireService } from '../fire.service';

@Component({
  selector: 'app-navigation',
  templateUrl: './navigation.component.html',
  styleUrls: ['./navigation.component.css']
})
export class NavigationComponent implements OnInit {

  title = 'Cricket Cannon';
  toggled: boolean;
  pressureValue;

  constructor(private fireService: FireService) {

   }

  ngOnInit() {
     this.fireService.currentPressure$.subscribe(data => {
       this.pressureValue = data;
     })
  }

}
