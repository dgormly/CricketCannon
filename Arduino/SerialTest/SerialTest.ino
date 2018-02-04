#include <Servo.h>

//const int RELOAD_DELAY = 1000;
//const int SERVO_DELAY = 1000;
//const int PWM_OUTPUT_OPEN; // Not sure what you mean about this.
//const int PWM_OUTPUT_CLOSED;
//const int VALVE_THRESHOLD;
//const int NOMINAL_PRESSURE;
//


/* Pin settings */
int led = 13;


/* Package struct */
typedef struct message_t {
  String messageType = "";
  String value = "0";
} message_t;


// Globals
message_t req;
message_t res;
bool respond = false;

/*
 * Exit Codes:
 *  1: Incorrect parameters passed in.
 */

/* Setup arduino on startup */
void setup() {
  // Setup pins
  pinMode(led, OUTPUT);

  // Setup pins
  Serial.begin(9600);
  while (!Serial);
}

/* Main loop */
void loop() {
  // Wait for request
  delay(500);
  if (Serial.available() > 0) {
    read_request();
    
    if (!check_request()) {
      res.messageType = "ERROR";
      res.value = "1";
      Serial.println(create_packet(res));
      res.messageType = "";
      res.value = "0";
    } else {
      fire();
      Serial.println(create_packet(res));
    }
  }
}

/* ####################################################################
 *  Communication functions.
 */

/* 
 * Handle contact with serial port. 
 * 
 * Responsible the main flow of serial communication.
 */
void read_request() {
  // Get available bytes.
  req.messageType = Serial.readStringUntil('/');
  req.value = Serial.readStringUntil('\n');
}


/* Validates the incoming request. */
bool check_request() {
  if (req.messageType == "FIRE" && req.value.toInt() < 100 && req.value.toInt() > 36.2594) {
    return true;
  }
  req.messageType = "";
  req.value = 0;
  return false;
}

String create_packet(message_t res) {
  return "" + res.messageType + "/" + res.value;
}

/* ###################################################################
 *  Fire sequence functions.
 */

/* Begin fire sequence for a single shot. */
void fire() {
  reload();
  pressTank(10);
}


/* Reload Cannon */
void reloading() {
  delay(1000);  
}


/* Pressurise tank - Maps pressure to bar and pressurises tank. */
void pressTank(int psi) {
  long bar = psi / 14.5038;
  long inputVoltage = map(bar, 0, 3.3, 1.5, 7); // Map input voltage.
  
  for (int i = 0; i < psi; i++) {
    res.messageType = "PRESSURE";
    res.valueLong = i;
    Serial.println(create_packet(res));
    delay(100);
  }
}

bool reload() {
  
}



