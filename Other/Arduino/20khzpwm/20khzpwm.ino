
int pwmPin = 3;

void setup() {
  TCCR1A = _BV(COM1A1) | _BV(COM1B1);
  TCCR1B = _BV(WGM13) | _BV(CS10);
  ICR1 = 400 ; // 10 bit resolution
  OCR1A = 0; // vary this value between 0 and 400 for 10-bit precision
  OCR1B = 328; // vary this value between 0 and 400 for 10-bit precision      

  pinMode (10, OUTPUT);
  pinMode (3, OUTPUT);
}

void loop() {
  analogWrite(3, 127); 
  // put your main code here, to run repeatedly:

}
