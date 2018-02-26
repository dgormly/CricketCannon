import { Component }   from '@angular/core';
import { Router }      from '@angular/router';
import { AuthService } from '../auth.service';
import { FireService } from '../fire.service';
import * as socketIo from 'socket.io-client';

@Component({
  selector: 'app-login',
  templateUrl: './login.component.html',
  styleUrls: ['./login.component.css']
})

export class LoginComponent {
  message: string;

  constructor(private fireService: FireService, public authService: AuthService, public router: Router) {
    this.setMessage();
  }

  setMessage() {
    this.message = 'Logged ' + (this.authService.isLoggedIn ? 'in' : 'out');
  }

  login() {
    this.message = 'Trying to log in ...';
    this.fireService.socket.emit("SERVER/AUTH");
      this.authService.login().subscribe(() => {
        this.setMessage();
        if (this.authService.isLoggedIn) {
          // Get the redirect URL from our auth service
          // If no redirect has been set, use the default
          let redirect = this.authService.redirectUrl ? this.authService.redirectUrl : '/Dashboard';
  
          // Redirect the user
          this.router.navigate([redirect]);
        }
      });  
  }

  logout() {
    this.authService.logout();
    this.setMessage();
  }
}