void setup() {
  Serial.begin(9600);
  analogWriteResolution(12);
  analogReadResolution(12);
}

void loop() {
  analogWrite(A14, (int)1024);
  int val = analogRead(A0);     // read the input pin
  Serial.println(val);             // debug value
}
