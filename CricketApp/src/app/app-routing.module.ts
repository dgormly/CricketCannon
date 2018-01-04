import { NgModule } from '@angular/core';
import { RouterModule, Routes} from '@angular/router';
import { FireComponent } from './fire/fire.component';

const routes: Routes = [
  {path: '', redirectTo: '/Fire', pathMatch: 'full'},
  {path: 'Fire', component: FireComponent }
];

@NgModule({
  imports: [ RouterModule.forRoot(routes)],
  exports: [ RouterModule ]
})

export class AppRoutingModule { }
