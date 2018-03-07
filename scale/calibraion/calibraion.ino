//-------------------------------------------------------------------------------------
// HX711_ADC.h
// Arduino master library for HX711 24-Bit Analog-to-Digital Converter for Weigh Scales
// Olav Kallhovd sept2017
// Tested with      : HX711 asian module on channel A and YZC-133 3kg load cell
// Tested with MCU  : Arduino Nano
//-------------------------------------------------------------------------------------
// This is an example sketch on how to use this library for two or more HX711 modules
// Settling time (number of samples) and data filtering can be adjusted in the HX711_ADC.h file



#include <HX711_ADC.h>
#include <EEPROM.h>       // The sensor values are stored in EEPROM now. 

struct CalibrationData {
  float sensor1;
  float sensor2;
  float sensor3;
  word  eepromWrites;  
};


float calStored1;
float calStored2;
float calStored3;
float calFactor1;
float calFactor2;
float calFactor3;
word  eepromWriteCount;


float calWeight = 100.0; // calibration weight in grams
float calAcceptMax = 0.005;  // acceptable maximum error during calibration (tolerance)

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
  Serial.println("Wait...");
  LoadCell_1.begin();
  LoadCell_2.begin();
  LoadCell_3.begin();
        
  long stabilisingtime = 2000; // tare preciscion can be improved by adding a few seconds of stabilising time
  byte loadcell_1_rdy = 0;
  byte loadcell_2_rdy = 0;
  byte loadcell_3_rdy = 0;
  while ((loadcell_1_rdy + loadcell_2_rdy + loadcell_3_rdy) < 3) { //run startup, stabilization and tare, both modules simultaniously    
    if (!loadcell_1_rdy) loadcell_1_rdy = LoadCell_1.startMultiple(stabilisingtime);
    if (!loadcell_2_rdy) loadcell_2_rdy = LoadCell_2.startMultiple(stabilisingtime);
    if (!loadcell_3_rdy) loadcell_3_rdy = LoadCell_3.startMultiple(stabilisingtime);    
  }      
}

void loop() {
  // read EEPROM values
  int addrOffset = 0;                       // starting address for calibration data storage
  CalibrationData storedCal;    // variable to store custom sensor calibration data from EEPROM.  
  EEPROM.get(addrOffset, storedCal);


  // ONLY UNCOMMENT IF NEEDING BRAND NEW VALUES
 //storedCal.sensor1 = 6498.17;
 //storedCal.sensor2 = 6691.42;
 //storedCal.sensor3 = 6621.78;

  calStored1 = storedCal.sensor1;
  calStored2 = storedCal.sensor2;
  calStored3 = storedCal.sensor3;
  eepromWriteCount = storedCal.eepromWrites;
  calFactor1 = calStored1;
  calFactor2 = calStored2;
  calFactor3 = calStored3;
  //update() should be called at least as often as HX711 sample rate; >10Hz@10SPS, >80Hz@80SPS
  //longer delay in scetch will reduce effective sample rate (be carefull with delay() in loop)
  LoadCell_1.setCalFactor(calFactor1); // user set calibration factor (float)
  LoadCell_2.setCalFactor(calFactor2); // user set calibration factor (float)
  LoadCell_3.setCalFactor(calFactor3); // user set calibration factor (float)  
  
  LoadCell_1.update();
  LoadCell_2.update();
  LoadCell_3.update();  
  Serial.println("Startup + tare is complete");

  Serial.println("=====================================================");
  Serial.println("     CRICKET BALL  Sensor Calibration STARTING       ");
  Serial.println("=====================================================");

  Serial.println("Reading EEPROM ...");
  Serial.print("LoadCell #1 Initial Calibration value = ");
  Serial.println( calFactor1);
  Serial.print("LoadCell #2 Initial Calibration value = ");
  Serial.println( calFactor2);
  Serial.print("LoadCell #3 Initial Calibration value = ");
  Serial.println( calFactor3);
  Serial.print("Calibration Weight set to ");
  Serial.print( calWeight );
  Serial.println(" grams ");
    
  float oldcalFactor ;
  float oldweight ; 
  float deltaWeight;   // calc weight difference      
  bool stayTillItsRight ;
  int attemptCount = 0;

  
  Serial.println("\r\n\tCALIBRATION:");
  Serial.println("   Isolate all scale points.");  
  Serial.print("01 Press enter key to tare scale #1 ");
  
  wait4inputWithUpdate();      
  LoadCell_1.tareNoDelay();         //check if last tare operation is complete
  delay(100);        
  while (LoadCell_1.getTareStatus()){
    delay(50);            
  }                  
  Serial.println(" complete");                     
  Serial.print("02 place ");
  Serial.print( calWeight);
  Serial.print(" weight on scale point #1 and press enter key ");
  wait4inputWithUpdate();                       // we can calibrate it now.      
  oldcalFactor = calFactor1;    
 stayTillItsRight = true;
  while (stayTillItsRight){
    Serial.print(".");
    attemptCount ++ ;
    if (attemptCount > 10){
      // something has gone wrong with the calibration attempt.
      Serial.print("ERROR Reset");
      LoadCell_1.setCalFactor(calStored1);        // reset initial calibration factor (float)                      
      attemptCount = 0;
    }
    for (int i = 0; i <=10; i++ ){
      LoadCell_1.update(); delay(50); oldweight = LoadCell_1.getData();                       
    }    
    deltaWeight = abs (oldweight - calWeight );    // calc positive weight difference        
    if ( deltaWeight <= calAcceptMax ) {
      // within acceptable tolerance
      stayTillItsRight = false;                   // leave while loop          
      Serial.println(" DONE");
    } else {
      // make correction to calibration factor
      calFactor1 = (oldcalFactor * oldweight) / calWeight;  // update storage value
      LoadCell_1.setCalFactor(calFactor1);        // user set calibration factor (float)                
      //Serial.println(oldweight/calWeight);      
      for (int i = 0; i <=3; i++ ){
        LoadCell_1.update(); delay(50); oldweight = LoadCell_1.getData();                       
      }    
    }            
  }  // repeat while till it's right

  Serial.print("03 Press enter key to tare scale #2 ");  
  wait4inputWithUpdate();      
  LoadCell_2.tareNoDelay();         //check if last tare operation is complete
  delay(100);        
  while (LoadCell_2.getTareStatus()){
    delay(50);            
  }                  
  Serial.println(" complete");                   
  
  Serial.print("04 place ");
  Serial.print( calWeight);
  Serial.print(" weight on scale point #2 and press enter key ");
  wait4inputWithUpdate();                       // we can calibrate it now.      
  oldcalFactor = calFactor2;    
  stayTillItsRight = true;
  while (stayTillItsRight){
    Serial.print(".");
    attemptCount ++ ;
    if (attemptCount > 5){
      // something has gone wrong with the calibration attempt.
      Serial.print("ERROR Reset");
      LoadCell_2.setCalFactor(calStored2);        // reset initial calibration factor (float)                      
      attemptCount = 0;
    }
    for (int i = 0; i <=10; i++ ){
      LoadCell_2.update(); delay(50); oldweight = LoadCell_2.getData();                       
    }    
    deltaWeight = abs (oldweight - calWeight );    // calc positive weight difference        
    if ( deltaWeight <= calAcceptMax ) {
      // within acceptable tolerance
      stayTillItsRight = false;                   // leave while loop          
      Serial.println(" DONE");
    } else {
      // make correction to calibration factor
      calFactor2 = (oldcalFactor * oldweight) / calWeight;  // update storage value
      LoadCell_2.setCalFactor(calFactor2);        // user set calibration factor (float)                
      //Serial.println(oldweight/calWeight);      
      for (int i = 0; i <=3; i++ ){
        LoadCell_2.update(); delay(50); oldweight = LoadCell_2.getData();                       
      }    
    }            
  }  // repeat while till it's right

  Serial.print("05 Press enter key to tare scale #3 ");  
  wait4inputWithUpdate();      
  LoadCell_3.tareNoDelay();         //check if last tare operation is complete
  delay(100);        
  while (LoadCell_3.getTareStatus()){
    delay(50);            
  }                  
  Serial.println(" complete");                   
  
  Serial.print("06 place ");
  Serial.print( calWeight);
  Serial.print(" weight on scale point #3 and press enter key ");
  wait4inputWithUpdate();                       // we can calibrate it now.      
  oldcalFactor = calFactor3;    
  stayTillItsRight = true;
  while (stayTillItsRight){
    Serial.print(".");
    attemptCount ++ ;
    if (attemptCount > 5){
      // something has gone wrong with the calibration attempt.
      Serial.print("ERROR Reset");
      LoadCell_3.setCalFactor(calStored3);        // reset initial calibration factor (float)                      
      attemptCount = 0;
    }
    for (int i = 0; i <=10; i++ ){
      LoadCell_3.update(); delay(50); oldweight = LoadCell_3.getData();                       
    }    
    deltaWeight = abs (oldweight - calWeight );    // calc positive weight difference        
    if ( deltaWeight <= calAcceptMax ) {
      // within acceptable tolerance
      stayTillItsRight = false;                   // leave while loop          
      Serial.println(" DONE");
    } else {
      // make correction to calibration factor
      calFactor3 = (oldcalFactor * oldweight) / calWeight;  // update storage value
      LoadCell_3.setCalFactor(calFactor3);        // user set calibration factor (float)                
      //Serial.println(oldweight/calWeight);      
      for (int i = 0; i <=3; i++ ){
        LoadCell_3.update(); delay(50); oldweight = LoadCell_3.getData();                       
      }    
    }            
  }  // repeat while till it's right
  



  
  Serial.println("07 Remove all weights for re-tareing and hit return.\r\n");
  wait4inputWithUpdate();
  LoadCell_1.tareNoDelay();
  LoadCell_2.tareNoDelay();
  LoadCell_3.tareNoDelay();                 
  while (LoadCell_1.getTareStatus()){
    delay(100);        
  }
  while (LoadCell_2.getTareStatus()){
    delay(100);        
  }
  while (LoadCell_3.getTareStatus()){
    delay(100);        
  }
  Serial.print(" LoadCell #1 Final Calibration value = ");
  Serial.println( calFactor1);
  Serial.print(" LoadCell #2 Final Calibration value = ");
  Serial.println( calFactor2);
  Serial.print(" LoadCell #3 Final Calibration value = ");
  Serial.println( calFactor3);

  Serial.println(" =====================================================");
  Serial.println("  CRICKET BALL  Sensor Calibration  COMPLETED         ");
  Serial.println(" =====================================================");
  Serial.print("\tPRESS ENTER TO STORE CALIBRATION DATA IN EEPROM ........ ");  
  wait4inputWithUpdate();
  storedCal.sensor1 = calFactor1;
  storedCal.sensor2 = calFactor2;
  storedCal.sensor3 = calFactor3;
  eepromWriteCount ++; 
  storedCal.eepromWrites = eepromWriteCount ;
  EEPROM.put(addrOffset,storedCal);   // STORE IN EEPROM.
  Serial.println(" STORED.");
  Serial.print("\tEEPROM has been written to ");
  Serial.print( eepromWriteCount );
  Serial.println(" times."); 
  Serial.println("\tPRESS ENTER TO RESTART CALIBRATION PROCEDURE........ ");  
  wait4inputWithUpdate();
}    
  



      
void wait4inputWithUpdate(){
  // clear Serial buffer    
  while (Serial.available()){
    Serial.read();        
  }
  // now the buffer is empty. we wait till user presses something.
  while (!Serial.available()) {
    LoadCell_1.update();          
    LoadCell_2.update();          
    LoadCell_3.update();          
    
    delay(80);          
  }
  return;
}


void wait4input(){
  // clear Serial buffer    
  while (Serial.available()){
    Serial.read();        
  }
  // now the buffer is empty. we wait till user presses something.
  while (!Serial.available()) {
  ;        
  }
  return;
}


