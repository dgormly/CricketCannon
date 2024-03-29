import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';


import { AppComponent } from './app.component';
import { NavigationComponent } from './navigation/navigation.component';
import {
  MatButtonModule, MatButtonToggleModule, MatExpansionModule, MatExpansionPanel, MatFormFieldModule, MatInputModule, MatTableModule,
  MatToolbarModule, MatOptionModule, MatSelectModule, MatCheckboxModule, MatStepperModule, MatProgressBarModule, MatTooltipModule, MatTabsModule, MatCardModule, MatPaginatorModule
} from '@angular/material';
import { AppRoutingModule } from './/app-routing.module';
import { FireComponent } from './fire/fire.component';
import { DataComponent } from './data/data.component';
import { ScaleDataComponent } from './scaleData/scaleData.component';
import { ScaleComponent } from './scale/scale.component';
import {BrowserAnimationsModule} from '@angular/platform-browser/animations';
import {FireService} from './fire.service';
import {HttpClientModule} from '@angular/common/http';
import {DataService} from './data.service';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { NoopAnimationsModule } from '@angular/platform-browser/animations/src/module';
import { AuthGuard } from './auth-guard.service';
import { AuthService } from './auth.service';
import { LoginComponent } from './login/login.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { ChartModule } from 'angular2-chartjs';
import { SingleComponent } from './single/single.component';
import { SingleDataComponent } from './single-data/single-data.component';
//import {NgbModule} from '@ng-bootstrap/ng-bootstrap';


@NgModule({
  declarations: [
    AppComponent,
    NavigationComponent,
    FireComponent,
    DataComponent,
    ScaleComponent,
    LoginComponent,
    DashboardComponent,
    ScaleDataComponent,
    SingleComponent,
    SingleDataComponent
  ],
  imports: [
    FormsModule,
    BrowserModule,
    MatToolbarModule,
    MatButtonModule,
    AppRoutingModule,
    MatTableModule,
    MatButtonToggleModule,
    MatFormFieldModule,
    MatInputModule,
    MatExpansionModule,
    BrowserAnimationsModule,
    MatButtonModule,
    MatOptionModule,
    MatSelectModule,
    HttpClientModule,
    MatCheckboxModule,
    MatStepperModule,
    ReactiveFormsModule,
    MatProgressBarModule,
    MatTooltipModule,
    MatTabsModule,
    MatCardModule,
    ChartModule,
    MatPaginatorModule
    //NgbModule
  ],
  providers: [AuthService, AuthGuard, FireService, DataService],
  bootstrap: [AppComponent]
})
export class AppModule { }
