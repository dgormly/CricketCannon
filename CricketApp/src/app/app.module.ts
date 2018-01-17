import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';


import { AppComponent } from './app.component';
import { NavigationComponent } from './navigation/navigation.component';
import {
  MatButtonModule, MatButtonToggleModule, MatExpansionModule, MatExpansionPanel, MatFormFieldModule, MatInputModule, MatTableModule,
  MatToolbarModule, MatOptionModule, MatSelectModule, MatCheckboxModule, MatStepperModule, MatProgressBarModule, MatTooltipModule, MatTabsModule, MatCardModule
} from '@angular/material';
import { AppRoutingModule } from './/app-routing.module';
import { FireComponent } from './fire/fire.component';
import { DataComponent } from './data/data.component';
import { DataNavComponent } from './data-nav/data-nav.component';
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


@NgModule({
  declarations: [
    AppComponent,
    NavigationComponent,
    FireComponent,
    DataComponent,
    DataNavComponent,
    LoginComponent,
    DashboardComponent
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
    MatCardModule
  ],
  providers: [AuthService, AuthGuard, FireService, DataService],
  bootstrap: [AppComponent]
})
export class AppModule { }
