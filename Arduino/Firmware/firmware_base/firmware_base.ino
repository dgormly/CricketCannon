//int triggerPin = 1;
//int elbowValve = 2;
//int lightGreen = 3;
//int lightRed = 4;
//
//int pinGate1 = 2;
//int pinGate2 = 3;
//int pinGate3 = 7;
//int pinGate4 = 8;
//int pinGateOn = 9;
//
//unsigned long gTime1 = 0;
//unsigned long gTime2 = 0;
//unsigned long gTime3 = 0;
//unsigned long gTime4 = 0;
//

#define HWSERIAL Serial1

#define RES  16
// Read Bits

#define AVG  4
// hardware averages 1, 2, 4, 8, 16, 32

#define NUM_READS 150
#define MEDIAN_SIZE 40


int pinPressure1 = 17; //A4
int pinPressure2 = 18; //A5
int pinPressure3 = 19; //A6
int pinPressure4 = 20; //A7
int pinPressure5 = 21; //A8

float pressure1 = 0.0;
float pressure2 = 0.0;
float pressure3 = 0.0;
float pressure4 = 0.0;
float pressure5 = 0.0;

float DAC;

String inputString = "";
char floatbuf[8];

//
//float pSetPoint = 0.0;
//
//float gateDistance1 = 0.0;
//float gateDistance2 = 0.0;
//
//float velocity1 = 0.0;
//float velocity2 = 0.0;
//
//boolean safetyOne = false;
//boolean safetyTwo = false;
//boolean showTime = false;



float mapfloat(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}

void ProcessSerial(char inChar0, char inChar1) {
  if (inChar0 == 'S' and inChar1 == 'P') {

    inputString.toCharArray(floatbuf, sizeof(floatbuf));
    DAC = atof(floatbuf);
    Serial.println("Revc SP");
  }
}

void serialEvent1() {
  while (Serial1.available() > 0) {
    char inChar = (char)Serial1.read();
    inputString += inChar;
    Serial.println("Hello");
    if (inChar == '\n' or inChar == '\r') {
      char inChar0 = inputString.charAt(0);
      char inChar1 = inputString.charAt(1);
      //Serial.print(inChar0);
      //Serial.print(inChar1);
      Serial.println(inputString);
      inputString.remove(0, 2);
      ProcessSerial(inChar0, inChar1);
      inputString = "";
    }
  }
}

float ReadADC(int sensorpin) {  // read multiple values and sort them to take the mode ///adapted from :http://www.elcojacobs.com/eleminating-noise-from-sensor-readings-on-arduino-with-digital-filtering/

  int sortedValues[NUM_READS];

  for (int i = 0; i < NUM_READS; i++) { ///generate an array NUM_READS in size and fill it with ADC readings sorted from largest-smallest
    int value = analogRead(sensorpin);
    int j;
    if (value < sortedValues[0] || i == 0) {
      j = 0; //insert at first position
    }
    else {
      for (j = 1; j < i; j++) {
        if (sortedValues[j - 1] <= value && sortedValues[j] >= value) {
          // j is insert position
          break;
        }
      }
    }
    for (int k = i; k > j; k--) {
      // move all values higher than current reading up one position
      sortedValues[k] = sortedValues[k - 1];
    }
    sortedValues[j] = value; //insert current reading
  }




  //return scaled mode of MEDIAN_SIZE values
  float returnval = 0;
  for (int i = NUM_READS / 2 - (MEDIAN_SIZE / 2); i < (NUM_READS / 2 + (MEDIAN_SIZE / 2)); i++) {

    returnval += sortedValues[i];
  }
  returnval = returnval / MEDIAN_SIZE;
  //return returnval*1100/1023;
  return returnval;
}

////////////////////////////////////////////////////////////////
//boolean fire(int pulseLength){
//
// if(safetyOne and safetyTwo and showTime){
//
//  digitalWrite(triggerPin,HIGH);
//  delay(pulseLength);
//  digitalWrite(triggerPin,LOW);
//  Serial.println("CANNON FIRED");
//  delay(1500);
//  calcVelocity1();
//  calcVelocity2();
//  showTime == false;
//  printVelocity();
//
//}else{
//
//  Serial.println("CANNON NOT SAFE TO FIRE");
//
//  }
//}
////////////////////////////////////////////////////////////////
void readPressures() {

  pressure1 = mapfloat(ReadADC(pinPressure1), 6553, 65536, 0, 200);
  pressure2 = mapfloat(ReadADC(pinPressure2), 6553, 65536, 0, 200);
  pressure3 = mapfloat(ReadADC(pinPressure3), 6553, 65536, 0, 200);
  pressure4 = mapfloat(ReadADC(pinPressure4), 6553, 65536, 0, 200);
  pressure5 = mapfloat(ReadADC(pinPressure5), 6553, 65536, 0, 200);

  //  Serial.println("P1" + String(pressure1));
  //  Serial.println("P2" + String(pressure2));
  //  Serial.println("P3" + String(pressure3));
  //  Serial.println("P4" + String(pressure4));
  //  Serial.println("P5" + String(pressure5));

  Serial1.println("P1" + String(pressure1));
  Serial1.println("P2" + String(pressure2));
  Serial1.println("P3" + String(pressure3));
  Serial1.println("V1" + String(pressure4));
  Serial1.println("V2" + String(pressure5));


}
///////////////////////////////////////////////////////////////


//void calcVelocity1() {
//  velocity1 = (gateDistance1 / (gTime2 - gTime1)) * 1000;
//}
//
//void calcVelocity2() {
//  velocity2  = (gateDistance2 / (gTime4 - gTime3)) * 1000;
//}
///////////////////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);
  HWSERIAL.begin(115200);
  //Serial.setTimeout(1);
  //Serial1.setTimeout(1);
  inputString.reserve(200);

  //analogReadResolution(RES);
  //analogReadAveraging(AVG);

  //pinMode(pinGate1, INPUT_PULLUP);
  //pinMode(pinGate2, INPUT_PULLUP);
  //pinMode(pinGate3,INPUT_PULLUP);
  //pinMode(pinGate4,INPUT_PULLUP);



  //attachInterrupt(digitalPinToInterrupt(pinGate1), ISRgate1, FALLING);
  //attachInterrupt(digitalPinToInterrupt(pinGate2), ISRgate2, FALLING);
  //attachInterrupt(digitalPinToInterrupt(pinGate3), ISRgate3, FALLING);
  //attachInterrupt(digitalPinToInterrupt(pinGate4), ISRgate4, FALLING);


  //pinMode(triggerPin,OUTPUT);
  //pinMode(lightGreen,OUTPUT);
  //pinMode(lightRed,OUTPUT);
  delay(500);
  Serial.println("Booting...");
  Serial.println("Teensy 3.1...");
  Serial1.println("P197");
  delay(500);

  pinMode(pressure1, INPUT);
  pinMode(pressure2, INPUT);
  pinMode(pressure3, INPUT);
  pinMode(pressure4, INPUT);
  pinMode(pressure5, INPUT);
}


void loop() {
  int incomingByte;
  //delay(200);
  //readPressures();
  //Serial.println(Serial1.available());
  //Serial.println("Hello");

  //  if (Serial1.available()) {
  //    Serial.println("Hello");
  //    //CheckfSerial();
 }


//void printVelocity() {
//  Serial.println("V1=" + String(velocity1));
//  //Serial.println("V2="+String(velocity2));
//}
//
//void ISRgate1() {
//  //if(showTime){
//  gTime1 = micros();
//  //}
//}
//
//
//void ISRgate2() {
//  //if(showTime){
//  gTime2 = micros();
//  //}
//  calcVelocity1();
//  printVelocity();
//}

//
//void ISRgate3(){
//  if(showTime){
//  gTime3 = micros();
//  }
//}
//
//
//void ISRgate4(){
//  if(showTime){
//  gTime4 = micros();
//  }
//}

