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

  constructor(private fireService: FireService) {
      fireService.firingMessage.subscribe(toggled => {
        this.toggled = toggled;
        console.log(this.toggled);
      });
   }

  ngOnInit() {
  }

}
