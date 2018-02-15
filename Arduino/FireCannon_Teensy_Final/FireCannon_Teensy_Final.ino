#include <Servo.h>


//Proportional Regulator Stuff/////////////////////////
#define PROP_FEEDBACK 14
                       // outside leads to ground and +5V
int val = 0;           // variable to store the value read
char buffer1[50];
int buffercount = 0;
//////////////////////////////////////////////////////
const bool DEBUG = true;
Servo frontservo;
Servo backservo;
const int VALVE_DELAY = 150;
const int DUMP_DELAY = 2000;
const int TANK_DELAY = 1000;
String message = "";
boolean globalstop = false;

//Relay Board Stuff///////////////////////////////////
#define TRIGGER_NO 1   //Relay 2
#define TRIGGER_NC 2    //Relay 3
#define TANK_NO 0      //Relay 1
#define REG_NC 5        //N/A
#define CYLINDER_NO 6    //N/A
#define DUMP_NO 3        //Relay 4
#define CLICKER 4
#define SERVOFRONT 20      // A6
#define SERVOBACK 21
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
#define ADCRES 12
volatile int pressurereadcounter = 0;
volatile int timesincepressureread = 0;
void setup()
{
  //Relay Board Stuff
  analogWrite(A14, 1);
  setup_relays();
  pinMode(13, OUTPUT); 
  frontservo.attach(SERVOFRONT);
  backservo.attach(SERVOBACK);
  frontservo.write(175);
  backservo.write(5);
  //Proportional Regulator Stuff
  Serial.begin(9600);              //  setup serial
  while (!Serial);
  analogWriteResolution(DACRES);
  analogReadResolution(ADCRES);
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

void testservos() {
  frontservo.write(170);
  Serial.println("front servo");
  delay(2000);
  frontservo.write(10);
  delay(2000);
  Serial.println("back servo");
  backservo.write(10);
  delay(2000);
  backservo.write(100);
  delay(2000);
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
    Serial.printf("CANNON/PRESSURE:%3.2f\n",floatMapToComputer(analogRead(PROP_FEEDBACK)));
    timesincepressureread = pressurereadcounter;
  }
}

String line;

void reload_ball(void) {
  backservo.write(50);
  delay(500);
  backservo.write(5);
  frontservo.write(100);
  delay(250);
  frontservo.write(175);
}

void fireSequence(float pressure) {
  float velocity = 0;
  reload_ball();
  delay(150); //to ensure ball drops in before a shot is fired assuming that the pressure is already at max
  if(communicationsIncoming())
    return;
  int pressurestatus = PressuriseAndWait(pressure);
  if(pressurestatus == 2 || pressurestatus == 0) {
    return; //comms incoming for 2, error for 0. either way want to exit
  }
  fire();
  if(communicationsIncoming())
    return;
  ValueToReg(floatMapToReg(pressure));
  //check lasers
  //confirm ball back in hopper
  Serial.printf("CANNON/RESULTS:{'PRESSURE':%3.2f, VELOCITY}\n",floatMapToComputer(analogRead(PROP_FEEDBACK)));
}


void handleContact() {
  // If we get a valid byte, read analog ins:
  // get incoming byte:
  if(Serial.available() > 0) {
    line = Serial.readStringUntil('/');
    if (line == "CANNON") {
      line = Serial.readStringUntil(':');
      if(line == "FIRE") {
        line = Serial.readStringUntil('\n');
        fireSequence(line.toFloat());
      } else if (line == "AUTO") {
        for(int i = 0; i < 20; i++) {
          fireSequence(90);
          if(communicationsIncoming()) {
            return;
          }
          delay(300);
        }
        depressurise();
      } else if(line == "TEST") {
        testpropreg();
      } else if(line == "STOP") {
        depressurise();
      } 
    } else if(line == "TESTSERVOS") {
      testservos();
    } else if(line == "SERVO") {
      line  = Serial.readStringUntil('/');
      if(line == "FRONT") {
        line = Serial.readStringUntil('\n');
        frontservo.write(line.toInt());
      } else if(line == "BACK") {
        line = Serial.readStringUntil('\n');
        backservo.write(line.toInt());        
      } else if(line == "TESTDROP") {
        frontservo.write(100);
        delay(300);
        frontservo.write(175);
      }
    }
  }
}

float globalpsi = 0;

void testpropreg() {
  line = Serial.readStringUntil('\n');
  float psi = line.toFloat();
  globalpsi = psi;
  float bar = psi/ 14.5038;
  long valtodac = floatMap(bar, 1.5, 7, 0, pow(2, DACRES)); // Map voltage
  Serial.println("Pressurising to " + String(psi) + " with DAC value " + String(valtodac));
  analogWrite(A14, valtodac);
  int reg_val;
  for(int i = 0; i < 10; i++) {
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

bool communicationsIncoming(void) {
  if (Serial.available() > 0) {
    return true;
  }
  return false;
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

void fire() {
  //Fire Cannon Function: Solenoid Sequence to Trigger QEV
  reload();
  digitalWrite(TRIGGER_NO, HIGH);
  digitalWrite(CAMERA, HIGH);
  delay(50);
  digitalWrite(CAMERA, LOW);
  delay(VALVE_DELAY);
  digitalWrite(TRIGGER_NC, HIGH);
  delay(DUMP_DELAY);
  digitalWrite(TRIGGER_NO, LOW);
  digitalWrite(TRIGGER_NC, LOW);
}

void reload() {
  //Reload Cannon Function: Closes Tank Valve to Close QEV.
  digitalWrite(TANK_NO, HIGH);
  delay(TANK_DELAY);
  digitalWrite(TANK_NO, LOW);
}

#define REGTHRESHOLD 100

void depressurise() {
  float bar;
  long valtodac;
  int reg_val, count = 0;
  for(int i = globalpsi; i > 21.8; i--) { 
    bar = i / 14.5038;
    valtodac = floatMap(bar, 1.5, 7, 0, pow(2, DACRES)); // Map voltage
    analogWrite(A14, valtodac);
    delay(50);
    reg_val = analogRead(PROP_FEEDBACK);
    long expected_feedback = floatMap(valtodac, 0, pow(2, DACRES), 0 , 2800);
    count++;
    if(DEBUG && count > 4) { //forces this to only happen every 200 ms
      Serial.printf("CANNON/DEBUG:Reading from PropReg:%d, Desired:%d\n", reg_val, expected_feedback);
      count = 0;
    }
  }
}

void ValueToReg(int valtodac) {
  if(DEBUG)
    Serial.println("CANNON/DEBUG:Pressurising to " + String(valtodac) + " with DAC value " + String(valtodac));
  analogWrite(A14, valtodac);
}


/*
 * Waits for pressure to reach the desired pressure, found in globalpsi that is set in the PressueToReg function
 * returns 0 for error, 1 for at pressure, 2 for comms inbound
 */
int checkPressure(int valtodac) {
  int reg_val;
  bool atpressure = false;
  long expected_feedback = floatMap(valtodac, 0, pow(2, DACRES), 0 , 2800);
  while(!atpressure){
    if(communicationsIncoming())
      return 2;
    reg_val = analogRead(PROP_FEEDBACK);
    if((reg_val >= (expected_feedback - REGTHRESHOLD)) && (reg_val <= (expected_feedback + REGTHRESHOLD))) {
      return 1;
    }
    printpressurecycle();
    if(DEBUG){
      Serial.printf("CANNON/DEBUG:{'Reading from PropReg': %d,'Desired': %d}\n", reg_val, expected_feedback);
    }
    delay(50);
  } 
}

/*
 * Returns an integer value necessary to get an appropriate voltage at the pressure regulator for
 * setting PSI output
 */
int floatMapToReg(float psi) {
  float bar = psi / 14.5038;
  return floatMap(bar, 1.5, 7, 0, pow(2, DACRES));
}

float floatMapToComputer(int adcval) {
  //ADC val back to bar, then taken back to PSI
  return (floatMap(adcval, 0 , pow(2,ADCRES),1.5 ,7)) * 14.5038;
}

/*
 * returns 0 for error, 1 for at pressure, 2 for comms inbound
 */
int PressuriseAndWait(float psi){
  globalpsi = psi;
  ValueToReg(floatMapToReg(psi));
  int pressurestatus = checkPressure(floatMapToReg(psi));
  if(pressurestatus == 2) {
    return 2;
  } else if (pressurestatus = 1){
    return 1;
  } else {
    return 0;
  }
}

float floatMap(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

