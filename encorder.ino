void IRAM_ATTR updateEncoder() {
  if (digitalRead(ENCODER_PIN_A) == digitalRead(ENCODER_PIN_B)) {
    encoderPosition++;
  } else {
    encoderPosition--;
  }
  encoderFlag = true;
}

void setup_encorder() {
  pinMode(ENCODER_PIN_A, INPUT_PULLUP);
  pinMode(ENCODER_PIN_B, INPUT_PULLUP);
  pinMode(LED, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(ENCODER_PIN_A), updateEncoder, CHANGE);
}

void loop_encorder() {
  distance_mm = (encoderPosition / offset) * 5;
  dist_enco = scaling(distance_mm, 0, -999, 0, 999);

  if (distance_mm > 999) {
    distance_mm = 999;
  }

  if (distance_mm < -999) {
    distance_mm = -999;
  }

  if (distance_mm > 10) {
    digitalWrite(LED, HIGH);
  }
  else {
    digitalWrite(LED, LOW);
  }

  delay(100); 

  if (encoderFlag) {
    noInterrupts();
    long position = encoderPosition;
    encoderFlag = false;
    interrupts();

    String positionStr = String(position);
    writeFile(SPIFFS, "/encoderPosition.txt", positionStr.c_str());
  }
}
