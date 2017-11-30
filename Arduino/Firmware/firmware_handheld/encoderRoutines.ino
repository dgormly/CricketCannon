
void doEncoderA() {
  enc_velocity = 1000 / (millis() - enc_last_puls);
  // Test transition
  A_set = digitalRead(enc_up) == HIGH;
  // and adjust counter + if A leads B
  if (!encoderDisable) {
    encoderPos += (A_set != B_set) ? +0.001 * (enc_velocity / 4) : -0.001 * (enc_velocity / 4);
    enc_last_puls = millis();
    needSendSP = true;
  }
}

// Interrupt on B changing state
void doEncoderB() {
  enc_velocity = 500 / (millis() - enc_last_puls);
  // Test transition
  B_set = digitalRead(enc_down) == HIGH;
  // and adjust counter + if B follows A
  if (!encoderDisable) {
    encoderPos += (A_set == B_set) ? +0.001 * (enc_velocity / 4) : -0.001 * (enc_velocity / 4);
  }
  enc_last_puls = millis();
  needSendSP = true;
}


float round_to_qrt(float input) {
  input = input + 0.125;
  input = input * 4;
  input = int(input);
  input = input / 4.0;
  if (encoderPos < 0) {
    encoderPos = 0;
  }
  if (encoderPos > 400) {
    encoderPos = 400;
  }
  input = min(input, 100);
  input = max(input, 0);
  return input;

}
