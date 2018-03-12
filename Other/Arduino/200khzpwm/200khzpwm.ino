
int pwmPin = 3;

void setup() {
  TCCR1A = 0; //clear timer registers
  TCCR1B = 0;

  TCNT1 = 0;

  //ICR1 and Prescaler sets frequency
  //no prescaler .0625 us per count @ 16Mh
  //80 counts x .0625 = 5 us = 200Khz
  //for 50KHz it is 20uS and 320 counts
  TCCR1B |= _BV(CS10); //no prescaler

  ICR1 = 40;// 40 PWM mode counts up and back down for 80 counts

  // need to changed to 320 counts / 2 = 160 counts for 50KHz
  OCR1A = 20;
  OCR1B = 20; //Pin 10 match UNO, for mega its 12
  //output A set rising/clear falling

  //Rise at TCNT 21 upslope, High 38 counts, Fall at TCNT 21 downslope

  TCCR1A |= _BV(COM1A1) | _BV(COM1A0); //output A set rising/clear falling
  //output B clear rising/set falling
  //Fall at TCNT 19 upslope, Low 42, Rise at TCNT 19 downslope
  //47.5% Duty Cycle Pulse centered on TCNT 0. High 38 Low 42
  TCCR1A |= _BV(COM1B1); //output B clear rising/set falling
  TCCR1B |= _BV(WGM13); //PWM mode with ICR1 Mode 10
  TCCR1A |= _BV(WGM11); //WGM13:WGM10 set 1010

  pinMode (10, OUTPUT);
  pinMode (3, OUTPUT);
}

void loop() {
}
