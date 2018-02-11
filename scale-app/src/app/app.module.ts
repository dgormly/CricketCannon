import { BrowserModule } from '@angular/platform-browser';
import { NgModule } from '@angular/core';


import { AppComponent } from './app.component';
import { DashboardComponent } from './dashboard/dashboard.component';
import { BrowserAnimationsModule } from '@angular/platform-browser/animations';
import {
  MatButtonModule, MatButtonToggleModule, MatExpansionModule, MatExpansionPanel, MatFormFieldModule, MatInputModule, MatTableModule,
  MatToolbarModule, MatOptionModule, MatSelectModule, MatCheckboxModule, MatStepperModule, MatProgressBarModule, MatTooltipModule, MatTabsModule, MatCardModule
} from '@angular/material';
import {HttpClientModule} from '@angular/common/http';
import { FormsModule, ReactiveFormsModule } from '@angular/forms';
import { ChartsModule } from 'ng2-charts/ng2-charts';
import { DataComponent } from './data/data.component';


@NgModule({
  declarations: [
    AppComponent,
    DashboardComponent,
    DataComponent
  ],
  imports: [
    FormsModule,
    BrowserModule,
    MatToolbarModule,
    MatButtonModule,
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
    ChartsModule

  ],
  providers: [],
  bootstrap: [AppComponent]
})
export class AppModule { }
