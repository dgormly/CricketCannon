int triggerNO = 3;
int triggerNC = 4;
int tankNO = 5;
int regulatorNC = 6;
int cylinder = 7;
int testPin = 2;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(triggerNO, OUTPUT);
  pinMode(triggerNC, OUTPUT);
  pinMode(tankNO, OUTPUT);
  pinMode(regulatorNC, OUTPUT);
  pinMode(cylinder, OUTPUT);  
  pinMode(13, OUTPUT);
  pinMode(testPin, OUTPUT);
}

void loop() {
  digitalWrite(testPin, HIGH);
  digitalWrite(13, HIGH);
  delay(5000);
  digitalWrite(testPin, LOW);
  digitalWrite(13, LOW);
  delay(5000);
}
