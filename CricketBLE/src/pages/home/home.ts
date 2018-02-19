import { Component } from '@angular/core';
import { NavController, AlertController } from 'ionic-angular';
import { BLE } from '@ionic-native/ble';


@Component({
  selector: 'page-home',
  templateUrl: 'home.html'
})
export class HomePage {

  device_id: string = "";

  pressure: number = 0;
  balls: number;
  shots: number;

  devices: any[] = [];
  statusMessage: string;
  status: boolean;

  constructor(public navCtrl: NavController, public alertCtrl: AlertController, private ble: BLE) {
    ble.isConnected(this.device_id).then(data => {
      this.status = data;
      
    })
  }

  ionViewDidEnter() {
    console.log('ionViewDidEnter');
  }

  connect() {
    this.ble.connect(this.device_id);
  }

  successfulConnection() {
    let alert = this.alertCtrl.create({
      title: 'Connected!',
      subTitle: 'Successfully connected to cannon!',
      buttons: ['OK']
    });
    alert.present();
  }
}
