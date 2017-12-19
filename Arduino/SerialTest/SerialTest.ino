int led = 13;

int inByte = 0;
String message = "";

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  while (!Serial);

  pinMode(led, OUTPUT);
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
     } else if (message == "Setup!") {
            Serial.write("Setup!");
    }
  }
    fire();

}


void fire() {
  digitalWrite(led, HIGH); 
  delay(1000); 
  digitalWrite(led, LOW); 
  delay(1000);
  Serial.write("Firing!");
}


void world() {
  Serial.write("World");
}

