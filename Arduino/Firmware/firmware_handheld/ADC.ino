float ReadADC(int sensorpin){   // read multiple values and sort them to take the mode ///adapted from :http://www.elcojacobs.com/eleminating-noise-from-sensor-readings-on-arduino-with-digital-filtering/
     
   int sortedValues[NUM_READS];
   
   for(int i=0;i<NUM_READS;i++){   ///generate an array NUM_READS in size and fill it with ADC readings sorted from largest-smallest
     int value = analogRead(sensorpin);
     int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   
   
   
   
   //return scaled mode of MEDIAN_SIZE values
   float returnval = 0;
   for(int i=NUM_READS/2-(MEDIAN_SIZE/2);i<(NUM_READS/2+(MEDIAN_SIZE/2));i++){
     
     returnval +=sortedValues[i];
   }
   returnval = returnval/MEDIAN_SIZE;
   //return returnval*1100/1023;
   return returnval;
}

float ReadVCC(){   // read multiple values and sort them to take the mode ///adapted from :http://www.elcojacobs.com/eleminating-noise-from-sensor-readings-on-arduino-with-digital-filtering/
     
   int sortedValues[NUM_READS];
   
   for(int i=0;i<NUM_READS;i++){   ///generate an array NUM_READS in size and fill it with ADC readings sorted from largest-smallest
     int value = readRails();
     int j;
     if(value<sortedValues[0] || i==0){
        j=0; //insert at first position
     }
     else{
       for(j=1;j<i;j++){
          if(sortedValues[j-1]<=value && sortedValues[j]>=value){
            // j is insert position
            break;
          }
       }
     }
     for(int k=i;k>j;k--){
       // move all values higher than current reading up one position
       sortedValues[k]=sortedValues[k-1];
     }
     sortedValues[j]=value; //insert current reading
   }
   
   
   
   
   //return scaled mode of MEDIAN_SIZE values
   float returnval = 0;
   for(int i=NUM_READS/2-(MEDIAN_SIZE/2);i<(NUM_READS/2+(MEDIAN_SIZE/2));i++){
     
     returnval +=sortedValues[i];
   }
   returnval = returnval/MEDIAN_SIZE;
   //return returnval*1100/1023;
   return returnval;
}

long readRails() {
  // Read 1.1V reference against AVcc
  // set the reference to Vcc and the measurement to the internal 1.1V reference
  #if defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
    ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #elif defined (__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__)
    ADMUX = _BV(MUX5) | _BV(MUX0);
  #elif defined (__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__)
    ADMUX = _BV(MUX3) | _BV(MUX2);
  #else
    ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  #endif  

  //delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Start conversion
  while (bit_is_set(ADCSRA,ADSC)); // measuring

  uint8_t low  = ADCL; // must read ADCL first - it then locks ADCH  
  uint8_t high = ADCH; // unlocks both

  long result = (high<<8) | low;

  result = 1125300L / result; // Calculate Vcc (in mV); 1125300 = 1.1*1023*1000
  return result; // Vcc in millivolts
}
