#include <Servo.h>


//Proportional Regulator Stuff/////////////////////////
const int PROP_FEEDBACK = 14;
                       // outside leads to ground and +5V
int val = 0;           // variable to store the value read
char buffer1[50];
int buffercount = 0;
//////////////////////////////////////////////////////
const bool DEBUG = true;
Servo servo;
const int VALVE_DELAY = 150;
const int DUMP_DELAY = 2000;
const int TANK_DELAY = 1000;
String message = "";

//Relay Board Stuff///////////////////////////////////
#define TRIGGER_NO 1   //Relay 2
#define TRIGGER_NC 2    //Relay 3
#define TANK_NO 0      //Relay 1
#define REG_NC 5        //N/A
#define CYLINDER_NO 6    //N/A
#define DUMP_NO 3        //Relay 4
#define CLICKER 4
#define SERVO 20         // A6
#define CAMERA A9
////////////////////////////////////////////////////

//Laser detection variables///////////////////
volatile long velocitydiff_1 = 0;
volatile long velocitydiff_2 = 0;
volatile long velocitydiff_3 = 0;
volatile uint64_t velocitycapture_1 = 0;
volatile uint64_t velocitycapture_2 = 0;
volatile uint64_t velocitycapture_3 = 0;
volatile byte hasvelocitycaptured = 0;
volatile int state_error = 0;
volatile int ballhaspassed_1 = 0;
volatile int ballhaspassed_2 = 0;
volatile int ballhaspassed_3 = 0;
volatile int ballhaspassed_4 = 0;
IntervalTimer velocitytimer;
volatile uint64_t timer_counter;
#define IDLING 0
#define FIRST 1
#define SECOND 2
#define THIRD 3
volatile byte velocity_state = IDLING;
//Pin assignments
const int LASER_1 = 7;
const int LASER_2 = 8;
const int LASER_3 = 9;
const int LASER_4 = 10;
//////////////////////////////////////////////////
#define DACRES 12
volatile int pressurereadcounter = 0;
volatile int timesincepressureread = 0;
void setup()
{
  //Relay Board Stuff
  setup_relays();
  pinMode(13, OUTPUT); 
  servo.attach(SERVO);
  //Proportional Regulator Stuff
  Serial.begin(9600);              //  setup serial
  while (!Serial);
  analogWriteResolution(DACRES);
  analogReadResolution(12);
  pinMode(PROP_FEEDBACK, INPUT);
  setup_lasers();
  velocitytimer.begin(velocity_manage, 100);
  interrupts();
}

void setup_relays() {
  pinMode(TRIGGER_NO, OUTPUT);
  pinMode(TRIGGER_NC, OUTPUT);
  pinMode(TANK_NO, OUTPUT);
  pinMode(REG_NC, OUTPUT);
  pinMode(CYLINDER_NO, OUTPUT);
  pinMode(DUMP_NO, OUTPUT);
  pinMode(CLICKER, INPUT);
  pinMode(CAMERA, OUTPUT);
}


void velocity_manage() {
  if(velocity_state == IDLING) {
    if(ballhaspassed_1) {
      velocity_state = FIRST;
      timer_counter = 0;
      velocitycapture_1 = timer_counter;
      ballhaspassed_2 = 0;
      ballhaspassed_3 = 0;
      ballhaspassed_4 = 0; //clear these here to prevent the possibility of ringing affecting their clearing at end of THIRD state
    }
  } else if(velocity_state == FIRST){
    if(ballhaspassed_2) {
      velocity_state = SECOND;
      velocitydiff_1 = timer_counter - velocitycapture_1;
      velocitycapture_2 = timer_counter;
    }
    
    if(ballhaspassed_3 || ballhaspassed_4) {
      //A LASER HAS BEEN MISSED, EXIT LOOP
      ballhaspassed_1 = 0;
      ballhaspassed_2 = 0;
      ballhaspassed_3 = 0;
      ballhaspassed_4 = 0;
      velocity_state == IDLING;
      state_error = 2;
    }
  } else if(velocity_state == SECOND) {
    if(ballhaspassed_3){
      velocity_state = THIRD;
      velocitydiff_2 = timer_counter - velocitycapture_2;
      velocitycapture_3 = timer_counter; 
      ballhaspassed_1 = 0;     
    }

    if(ballhaspassed_4) {
      //A LASER HAS BEEN MISSED, EXIT LOOP
      ballhaspassed_1 = 0;
      ballhaspassed_2 = 0;
      ballhaspassed_3 = 0;
      ballhaspassed_4 = 0;
      velocity_state == IDLING;
      state_error = 3;
    }
  }else if(velocity_state == THIRD) {
    if(ballhaspassed_4){
      velocity_state = IDLING;
      velocitydiff_3 = timer_counter - velocitycapture_3;
      ballhaspassed_1 = 0;
      hasvelocitycaptured = 1;
    }
    if(ballhaspassed_1) {
      //A LASER HAS BEEN MISSED, EXIT LOOP
      ballhaspassed_1 = 0;
      ballhaspassed_2 = 0;
      ballhaspassed_3 = 0;
      ballhaspassed_4 = 0;
      velocity_state == IDLING;
      state_error = 4;
    }
  }
  timer_counter++;
  pressurereadcounter++;
}

void setup_lasers() {
    pinMode(LASER_1, INPUT_PULLDOWN);
    pinMode(LASER_2, INPUT_PULLDOWN);
    pinMode(LASER_3, INPUT_PULLDOWN);
    pinMode(LASER_4, INPUT_PULLDOWN);
    attachInterrupt(LASER_1,laser1Detect ,RISING);
    attachInterrupt(LASER_2,laser2Detect ,RISING);
    attachInterrupt(LASER_3,laser3Detect ,RISING);
    attachInterrupt(LASER_4,laser4Detect ,RISING);
}

void loop()
{
  // Wait for response
  if(hasvelocitycaptured) {
    String velocityoutput = "Diff1: " + String(velocitydiff_1) + "\n\rDiff2: " + String(velocitydiff_2) + "\n\rDiff3: " + String(velocitydiff_3);
    Serial.println(velocityoutput);
    hasvelocitycaptured = 0;
  }
  printpressurecycle();
  if(state_error) {
    Serial.printf("Error in state %d\n", state_error);
    state_error = 0;
  }
  handleContact();
}

void printpressurecycle(void) {
   if(pressurereadcounter > timesincepressureread + 2500) {
    Serial.printf("CANNON/PRESSURE/%u\n",analogRead(PROP_FEEDBACK));
    timesincepressureread = pressurereadcounter;
  }
}

String line;

void handleContact() {
  // If we get a valid byte, read analog ins:
  // get incoming byte:
  if(Serial.available() > 0) {
    line = Serial.readStringUntil('/');
    if (line == "CANNON") {
      line = Serial.readStringUntil('/');
      if(line == "FIRE") {
        line = Serial.readStringUntil('\n');
        fire(line.toFloat());
            
      } else if (line == "AUTO") {
        for(int i = 0; i < 10; i++) {
          fire(100);
          delay(10000);
        }
      } else if(line == "TEST") {
        testpropreg();
      }

    }
  }
}

void testpropreg() {
  line = Serial.readStringUntil('\n');
  float psi = line.toFloat();
  float bar = psi/ 14.5038;
  long valtodac = map(bar, 1.5, 7, 0, pow(2, DACRES)); // Map voltage
  Serial.println("Pressurising to " + String(psi) + " with DAC value " + String(valtodac));
  analogWrite(A14, valtodac);
  int reg_val;
  for(int i = 0; i < 50; i++) {
    reg_val = analogRead(PROP_FEEDBACK);
    Serial.printf("\n\rReading from PropReg: %d\n\rDesired: %d\n\r", reg_val, valtodac);
    delay(100);
  }
  /*
  for(valtodac; valtodac > 0; valtodac--){
    analogWrite(A14, valtodac);
    delay(10);
  }
  */
}

void laser1Detect() {
  ballhaspassed_1 = 1;
}

void laser2Detect() {
  ballhaspassed_2 = 1;
}


void laser3Detect() {
  ballhaspassed_3 = 1;
}


void laser4Detect() {
  ballhaspassed_4 = 1;
}


void GetPressure() {
  Serial.write("\nEnter Pressure (PSI):");
  message = "";
  while(1) {
    if(Serial.available() > 0) {  
      char numchar = Serial.read();
      if (numchar == '\n' || numchar == '\r') { 
        Serial.write("\n\r");
        int pval = message.toInt();
        reload();
        Pressurise(pval);
        break;
      } else if(numchar == -1){
        continue;
      }
      else {
        Serial.write(numchar);
        message += numchar;
      }
    }
  }
}

void fire(float psi) {
  //Fire Cannon Function: Solenoid Sequence to Trigger QEV
  Pressurise(psi);
  digitalWrite(TRIGGER_NO, HIGH);
  digitalWrite(CAMERA, HIGH);
  delay(VALVE_DELAY);
  digitalWrite(TRIGGER_NC, HIGH);
  delay(DUMP_DELAY);
  digitalWrite(TRIGGER_NO, LOW);
  digitalWrite(TRIGGER_NC, LOW);
  digitalWrite(CAMERA, LOW);
  Pressurise(25);
}

void reload() {
  //Reload Cannon Function: Closes Tank Valve to Close QEV.
  //delay(2000);
  digitalWrite(TANK_NO, HIGH);
  delay(TANK_DELAY);
  digitalWrite(TANK_NO, LOW);
}

#define REGTHRESHOLD 300

void Pressurise(float psi){
  float bar = psi / 14.5038;
  long valtodac = map(bar, 1.5, 7, 0, pow(2, DACRES)); // Map voltage
  Serial.println("Pressurising to " + String(psi) + " with DAC value " + String(valtodac));
  analogWrite(A14, valtodac);
  bool atpressure = false;
  int reg_val;
  delay(10000); //THIS CAN BE REMOVED ONCE WE HAVE FIGURED OUT WHY THE PRESSURE REG JUMPS AROUND SO MUCH
  /*
  while(!atpressure){
    reg_val = analogRead(PROP_FEEDBACK);  
    if(reg_val == (valtodac - REGTHRESHOLD)) {
      atpressure = true;
    }
    printpressurecycle();
    if(DEBUG){
      Serial.printf("\n\rReading from PropReg: %d\n\rDesired: %d\n\r", reg_val, valtodac);
      delay(50);
    }
  }
  */
}

void debug() {
  Serial.println("Running tests");
  // Fire all relays
  for (int i = 3; i > -1; i--) {
    digitalWrite(i, HIGH);
    delay(2000);
    digitalWrite(i, LOW);
  }

  // Rotate servo from 0 - 90 - 0
  servo.write(10);
  delay(2000);
  servo.write(100);
}

