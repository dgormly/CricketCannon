import { NgModule } from '@angular/core';
import { RouterModule, Routes} from '@angular/router';
import { FireComponent } from './fire/fire.component';
import { AuthGuard } from './auth-guard.service';
import { LoginComponent } from './login/login.component';
import { DashboardComponent } from './dashboard/dashboard.component';

const routes: Routes = [
  {path: '', redirectTo: '/Dashboard', pathMatch: 'full'},
  {
    path: 'Dashboard',
    component: DashboardComponent,
    canActivate: [AuthGuard],
  },
  {path: 'login', component: LoginComponent}
];

@NgModule({
  imports: [ RouterModule.forRoot(routes)],
  exports: [ RouterModule ]
})

export class AppRoutingModule { }
