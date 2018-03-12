
void DrawSplash(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_fub11r);
  u8g.drawStr(0, 20, "UQ Composites");
  u8g.drawStr(0, 35, "    & Crimsafe");
  u8g.setFont(u8g_font_6x10);
  u8g.drawStr(20, 52, " www.uq.edu.au");
  u8g.drawStr(20, 62, "crimsafe.com.au");


}

void DrawVersion(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_fub11r);
  u8g.drawStr(25, 15, "  Cannon");
  u8g.drawStr(25, 32, "Controller");
  u8g.drawStr(20, 60, "Version 1.0");
}

void drawMain(void) {
  mapEncoder();
  if (abs(encoderPos - last_encoderPos) > 0.005) {
    sendSP();
    spkBeep(5);
  }

  if (bitRead(PinState, 5)) {
    spkBeep(50);
    //digitalWrite(fireLed, !fireLedState);
  }
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_7x13);

  if (millis() - lastPacket < 750) {

    u8g.setPrintPos(0, 10);
    u8g.print("SP:" + String(encoderPos));
    u8g.setPrintPos(0, 22);
    u8g.print("P1:" + String(p1));
    u8g.setPrintPos(66, 10);
    u8g.print("PV:" + String(pv));

    u8g.setPrintPos(0, 34);
    u8g.print("P2:" + String(p2));
    u8g.setPrintPos(66, 22);
    u8g.print("V1:" + String(v1));

    u8g.setPrintPos(66, 34);
    u8g.print("V2:" + String(v2));

    u8g.setPrintPos(0, 46);
    u8g.print("P3:" + String(p3));

    u8g.setPrintPos(65, 52);
    u8g.print("Sig:" + String(WifiRSSI) + "%");

  } else {

    u8g.setPrintPos(0, 10);
    u8g.print("SP:" + String(encoderPos));
    u8g.setPrintPos(0, 22);
    u8g.print("P1:----");
    u8g.setPrintPos(66, 10);
    u8g.print("PV:----");

    u8g.setPrintPos(0, 34);
    u8g.print("P2:----");
    u8g.setPrintPos(66, 22);
    u8g.print("V1:----");

    u8g.setPrintPos(66, 34);
    u8g.print("V2:----");

    u8g.setPrintPos(0, 46);
    u8g.print("P3:----");

    u8g.setPrintPos(65, 52);
    u8g.print("Sig:---");

  }




  u8g.setPrintPos(2, 63);
  u8g.print(String(bitRead(PinState, 0)) + String(bitRead(PinState, 1)) + String(bitRead(PinState, 2)) + String(bitRead(PinState, 3)) + String(bitRead(PinState, 4)) + String(bitRead(PinState, 5)) + String(bitRead(PinState, 6)) + String(bitRead(PinState, 7)));
  //int raw = analogRead(vBat);
  float val = mapfloat(ReadADC(vBat), 0, 1023, 0, 25.6);
  //Serial.println(val,5);
  if (val < 3.9) {
  batLow=true;
    u8g.print(" Bat:LOW");
  } else {
    u8g.print(" Bat:" + String(val) + "v");
    batLow=false;
    //u8g.drawStr(25, 32, "Controller");
    //u8g.drawStr(20, 60, "Version 1.0");
  }
}

void draw(void) {
  // graphic commands to redraw the complete screen should be placed here
  u8g.setFont(u8g_font_unifont);
  u8g.setPrintPos(0, 20);
  // call procedure from base class, http://arduino.cc/en/Serial/Print
  u8g.print("Hello World!");
}
