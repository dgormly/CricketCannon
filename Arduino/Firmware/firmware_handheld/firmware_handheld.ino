
#include "U8glib.h"
#include "SPI.h"

U8GLIB_ST7920_128X64_4X u8g(15, 16, 14);

//int VbatArray[10];
float vbatVal;
#define NUM_READS 50
#define MEDIAN_SIZE 20

char floatbuf[8];

String inputString = "";

bool needSendSP = false;

int PostRegSwitch = 18;
int BotVentSwitch = 19;
int ButterflySwitch = 20;
int ElbowVentSwitch = 21;
int VacSwitch = 5;
int vBat = 10;
int spkPin = 6;

float PressureSP = 1.999;
float PresureMeas = 1.999;

float p1;
float p2;
float p3;
float pv;

float v1;
float v2;
float vbat;
unsigned long vbatWarnTime;
unsigned long stateupdate;
int vbatWarnInterval = 4000;
bool batLow = false;

int WifiRSSI;

int enc_up = 3;
int enc_down = 2;
int enc_velocity;
float enc_last_puls;

int fireBtn = 8;
int sftBtn = 7;
int fireLed = 9;
bool fireLedState = false;

boolean A_set = false;
boolean B_set = false;
boolean encoderDisable = false;
float last_encoderPos = 0;
float encoderPos = 0;

int debounceTime = 50;

unsigned long lastBtnChk;
unsigned long lastPacket;
unsigned long lastDraw;

bool RdyFire = true;
bool PinChange = false;

byte PinState;
byte PinStateOld;

void spkBeep(int len) {
  digitalWrite(spkPin, HIGH);
  delay(len);
  digitalWrite(spkPin, LOW);
}

float readVbat() {
  vbatVal = 0;
  for (int i = 0; i < 10; i++) {
    vbatVal = vbatVal + analogRead(vBat);
  }
  vbatVal = vbatVal / 10.0;
  return vbatVal;
}

void ReadPins() {

  PinStateOld = PinState;

  if (!digitalRead(PostRegSwitch)) {
    bitSet(PinState, 0);
  } else {
    bitClear(PinState, 0);
  }

  if (!digitalRead(BotVentSwitch)) {
    bitSet(PinState, 1);
  } else {
    bitClear(PinState, 1);
  }

  if (!digitalRead(ButterflySwitch)) {
    bitSet(PinState, 2);
  } else {
    bitClear(PinState, 2);
  }

  if (!digitalRead(ElbowVentSwitch)) {
    bitSet(PinState, 3);
  } else {
    bitClear(PinState, 3);
  }

  if (digitalRead(VacSwitch)) {
    bitSet(PinState, 4);
  } else {
    bitClear(PinState, 4);
  }

  if (!digitalRead(sftBtn)) {
    bitSet(PinState, 5);
    if (RdyFire) {
      digitalWrite(fireLed, !fireLedState);
      fireLedState = !fireLedState;
      //delay(250);
    }
  } else {
    bitClear(PinState, 5);
    digitalWrite(fireLed, LOW);

  }

  if (!digitalRead(fireBtn)) {
    bitSet(PinState, 6);
  } else {
    bitClear(PinState, 6);
  }

  if (!digitalRead(fireBtn) and !digitalRead(sftBtn)) {
    bitSet(PinState, 7);
    encoderPos=0;
  } else {
    bitClear(PinState, 7);
  }
  lastBtnChk = millis();
}

void mapEncoder() {
  if (encoderPos > 145) {
    encoderPos = 145;
  }

  if (encoderPos < 0) {
    encoderPos = 0;
  }
}


void sendSP() {
  //  Serial.print("SP");
  //  Serial.println(encoderPos, 2);

  Serial1.print("SP");
  Serial1.println(encoderPos, 2);
  last_encoderPos = encoderPos;
}

void sendPinStates() {
  int i;
  //Serial.print("SS");
  Serial1.print("SS");
  for (i = 0; i < 8; i++) {
    if (bitRead(PinState, i)) {
      //  Serial.print("1");
      Serial1.print("1");
    } else {
      //Serial.print("0");
      Serial1.print("0");
    }
  }
  //Serial.println();
  Serial1.println();
}

float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

bool checkPinChange() {
  if (PinState == PinStateOld) {

    PinChange = false;
    return false;

  } else {
    spkBeep(25);
    PinChange = true;
    return true;

  }
}

void ProcessSerial(char inChar0, char inChar1) {
  if (inChar0 == 'R' and inChar1 == 'S') {
    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    WifiRSSI = atof(floatbuf);
    if (WifiRSSI < 0) {
      lastPacket = millis();
      WifiRSSI = 100 - (map(WifiRSSI, -100, -50, 100, 0));
      WifiRSSI = WifiRSSI + 10;;
      if (WifiRSSI > 100) {
        WifiRSSI = 100;
      }
    }
  }

  if (inChar0 == 'P' and inChar1 == 'V') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    pv = atof(floatbuf);
  }

  if (inChar0 == 'P' and inChar1 == '1') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    p1 = atof(floatbuf);
  }

  if (inChar0 == 'P' and inChar1 == '2') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    p2 = atof(floatbuf);
  }

  if (inChar0 == 'P' and inChar1 == '3') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    p3 = atof(floatbuf);
  }

  if (inChar0 == 'V' and inChar1 == '1') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    v1 = atof(floatbuf);
  }

  if (inChar0 == 'V' and inChar1 == '2') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    v2 = atof(floatbuf);
  }


  inputString = "";
}

void serialEventRun() {
  //  if (Serial.available()) {
  //    char inChar0 = (char)Serial.read();
  //    char inChar1 = (char)Serial.read();
  //
  //    while (Serial.available()) {
  //      char inChar = (char)Serial.read();
  //
  //      if (inChar == '\n') {
  //        ProcessSerial(inChar0, inChar1);
  //      } else {
  //        inputString += inChar;
  //      }
  //
  //    }
  //
  //  }
  //if (Serial1.available()) {
  // lastPacket = millis();
  //char inChar0 = (char)Serial1.read();
  //char inChar1 = (char)Serial1.read();

  while (Serial1.available()) {
    //lastPacket = millis();
    char inChar = (char)Serial1.read();
    inputString += inChar;

    if (inChar == '\n' or inChar == '\r') {
      char inChar0 = inputString.charAt(0);
      char inChar1 = inputString.charAt(1);
      //Serial.print(inChar0);
      //Serial.print(inChar1);
      //Serial.println(inputString);
      inputString.remove(0, 2);
      ProcessSerial(inChar0, inChar1);
      inputString = "";
    }

  }
  drawMainScreen();

}

void setup() {

  Serial.begin(115200);
  Serial1.begin(115200);
  Serial.setTimeout(5);
  Serial1.setTimeout(5);
  inputString.reserve(200);
  SPI.setClockDivider(2);
  //Serial.println("Booting...");
  //Serial.println("Crimsafe Cannon Controller V1");
  pinMode(PostRegSwitch, INPUT_PULLUP);
  pinMode(BotVentSwitch, INPUT_PULLUP);
  pinMode(ButterflySwitch, INPUT_PULLUP);
  pinMode(ElbowVentSwitch, INPUT_PULLUP);
  pinMode(VacSwitch, INPUT_PULLUP);

  pinMode(fireBtn, INPUT_PULLUP);
  pinMode(fireLed, OUTPUT);
  pinMode(sftBtn, INPUT_PULLUP);
  pinMode(vBat, INPUT);
  pinMode(spkPin, OUTPUT);

  spkBeep(50);
  //delay(100);
  //spkBeep(50);
  digitalWrite(fireLed, HIGH);
  delay(200);
  digitalWrite(fireLed, LOW);
  delay(200);


  pinMode(enc_up, INPUT_PULLUP);
  pinMode(enc_down, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(enc_up), doEncoderA, CHANGE);
  attachInterrupt(digitalPinToInterrupt(enc_down), doEncoderB, CHANGE);
  analogReference(INTERNAL);


  u8g.firstPage();
  do {
    DrawSplash();
  } while ( u8g.nextPage() );

  delay(4000);

  u8g.firstPage();
  do {
    DrawVersion();
  } while ( u8g.nextPage() );

  //spk_beep(50);

  //spk_beep(50);
  delay(1000);
  sendPinStates();
  sendPinStates();

}

void drawMainScreen() {

  u8g.firstPage();
  do {
    drawMain();
  } while ( u8g.nextPage() );


}

void loop() {
  //spkBeep(50);
  //if (millis() - lastBtnChk > debounceTime) {
  if (millis() - stateupdate > 200) {
    sendPinStates;
    sendSP();
    stateupdate = millis();

  }
  ReadPins();
  if (checkPinChange()) {
    sendPinStates();
  }
  //}
  //Serial.println(encoderPos);
  if (batLow) {
    if (millis() - vbatWarnTime > vbatWarnInterval) {
      spkBeep(30);
      delay(75);
      spkBeep(30);
      vbatWarnTime = millis();
      //digitalWrite(fireLed, HIGH);
      //delay(100);
      //digitalWrite(fireLed, LOW);

    }
  }

}


