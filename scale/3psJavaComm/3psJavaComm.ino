//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library for two or more HX711 modules
// Settling time (number of samples) and data filtering can be adjusted in the HX711_ADC.h file

// Modified by William DEER to allow auto zero and known weight auto weighing.  # NOT YET IMPLEMENTED
// This is to reduce effects of thermal drift on loadcells by minimizing waiting time for user input.

#include <HX711_ADC.h>
#include <EEPROM.h>       // The sensor values are stored in EEPROM now. 

struct CalibrationData {
  float sensor1;
  float sensor2;
  float sensor3;
  word  eepromWrites;
  
};

//HX711 constructor (dout pin, sck pin)
HX711_ADC LoadCell_1( 4, 5);          //  Loadcell 1 on board
HX711_ADC LoadCell_2( 8, 9);          //  Loadcell 2 on board
HX711_ADC LoadCell_3( 6, 7);          //  Loadcell 3 on board

long t;
long countSum = 0;
long startTime = millis();
long delayThreshold = 50; // millisecond delay between samplings.

void setup() {
  Serial.begin(115200);
  //Serial.println("Wait...");
  // read EEPROM values
  int addrOffset = 0;                       // starting address for calibration data storage
  CalibrationData storedCal;    // variable to store custom sensor calibration data from EEPROM.
  EEPROM.get(addrOffset, storedCal);
  // init loadcells
  LoadCell_1.begin();
  LoadCell_2.begin();
  LoadCell_3.begin(); 
      
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  byte loadcell_3_rdy = 0;
  
  while ((loadcell_2_rdy + loadcell_3_rdy + loadcell_1_rdy) < 3) { //run startup, stabilization and tare, both modules simultaniously
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilisingtime);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilisingtime);
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilisingtime);    
  }  
  LoadCell_1.setCalFactor(storedCal.sensor1); 
  LoadCell_2.setCalFactor(storedCal.sensor2); 
  LoadCell_3.setCalFactor(storedCal.sensor3); 
  Serial.println("SETUP!OK");
}

void loop() {  
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in scetch will reduce effective sample rate (be carefull with delay() in loop)
  LoadCell_1.update();
  LoadCell_2.update();
  LoadCell_3.update();  

  //get smoothed value from data set + current calibration factor
  if (millis() > t + delayThreshold) {    
    countSum = (millis() - startTime) ; // time since starting in milliseconds    
    // get loadcell values
    float a = LoadCell_1.getData(); 
    float b = LoadCell_2.getData();
    float c = LoadCell_3.getData();
    
    float sum = a+b+c;        
    float COS60 = 0.5;
    float COS30 = 0.8660254038 ;
    float w1x = a * COS60;
    float w1y = -a * COS30;
    float w2x = b * COS60;
    float w2y = b * COS30;
    float w3x = -c ;
    float w3y = 0.0 ;

    float deltaX = (w1x + w2x + w3x) / 3.0;
    float deltaY = (w1y + w2y + w3y) / 3.0;

    float ballMass = sum;   // using 200g test weight for now.
    float ballMassThird = ballMass / 3.0;   // weight per support spike of uniform test object
    float ballRadius = 72.0/2;  // radius of support spike
    float scale = 3.6/4;
    float distX = scale* (deltaX * ballRadius / ballMassThird );
    float distY = scale * (deltaY * ballRadius / ballMassThird);

    //Serial.print("n:,");
    //Serial.print(countSum);
    Serial.print("SCALE!Raw/" +  String(a) + "/" + String(b) + "/" + String(c) 
                + "/Sum/" + String(sum) 
                + "/W1/" + String(w1x) + "/" + String(w1y) 
                + "/W2/" +String(w2x) + "/" + String(w2y) 
                + "/W3/" + String(w3x) + "/" + String(w3y)
               + "/dx/" + String(deltaX) + "/dy/" + String(deltaY)
               +"/distX/" + String(distX) + "/distY/" + String(distY)
               + "\n");
    delay(250); 
  }

  //receive from serial terminal
  if (Serial.available() > 0) {
    float i;
    char inByte = Serial.read();
    if (inByte == 't') {
      LoadCell_1.tareNoDelay();
      LoadCell_2.tareNoDelay();
      LoadCell_3.tareNoDelay();      
      while (LoadCell_1.getTareStatus()){
        delay(100);        
      }
     // Serial.println("Tare load cell 1 complete");
      while (LoadCell_2.getTareStatus()){
        delay(100);        
      }
     // Serial.println("Tare load cell 2 complete");
      while (LoadCell_3.getTareStatus()){
        delay(100);        
      }
      Serial.println("TARE!OK");
    }
  }
}

