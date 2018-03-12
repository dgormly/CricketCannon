const int TRIGGER_NO = 3;
const int TRIGGER_NC = 4;
const int TANK_NO = 5;
const int REG_NC = 6;
const int CYLINDER_NO = 7;

const int VALVE_DELAY = 1000;
const int DUMP_DELAY = 1500;

String message = "";


void setup() {
  pinMode(TRIGGER_NO, OUTPUT);
  pinMode(TRIGGER_NC, OUTPUT);
  pinMode(TANK_NO, OUTPUT);
  pinMode(REG_NC, OUTPUT);
  pinMode(CYLINDER_NO, OUTPUT);
  
  pinMode(13, OUTPUT);


  Serial.begin(9600);
  while (!Serial);

}

void loop() {
  // Wait for response
  establishContact();
  handleContact();
}


void establishContact() {
  while (Serial.available() <= 0) {
    delay(300);
  }
}



void handleContact() {
  // If we get a valid byte, read analog ins:
  // get incoming byte:
  char c = Serial.read();
  message += c;
  if (c == '#') {
    //do stuff
    message = ""; //clears variable for new input      
  } else {
    
     if (message == "Fire!") {
            fire();
            message = "";
     } else if (message == "Setup!") {
            Serial.write("Setup!");
            return;
    }
  }
}
void fire() {
  digitalWrite(TRIGGER_NO, HIGH);
  delay(VALVE_DELAY);
  digitalWrite(TRIGGER_NC, HIGH);
  delay(DUMP_DELAY);
  digitalWrite(TRIGGER_NO, LOW);
  digitalWrite(TRIGGER_NC, LOW);
}

