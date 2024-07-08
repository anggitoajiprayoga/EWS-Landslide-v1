void setup_display() {
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    eror_lcd = true;
  }
  else {
    eror_lcd = false;
  }
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.println();
  display.display();
  text_display("EWS", "Landslide", "center");
  delay(1500);
  text_display("Firm", firm_ver, "center");
  delay(1500);
  text_display(cn, "PT.PPA", "center");
  delay(1500);
  text_display("SS6-COE", "Team", "center");
  delay(1500);
}

void loop_display() {
  //  static int flag = 1;
  //  static String mode = "auto_slide";
  //  static unsigned long prev_manual = 0;
  //  static int count_manual = 0;
  //  static unsigned long prev_flag = 0;
  //
  //  if (mode == "auto_slide") {
  //    if (millis() - prev_flag > 5000) {
  //      flag++;
  //      prev_flag = millis();
  //    }
  //  }
  //  else if (mode == "manual_slide") {
  //    if (digitalRead(p_kalibrasi) == HIGH) {
  //      delay(250);
  //      flag++;
  //    }
  //    else if (digitalRead(p_default) == HIGH) {
  //      delay(250);
  //      flag--;
  //    }
  //
  //    if (millis() - prev_manual > 1000) {
  //      count_manual++;
  //      prev_manual = millis();
  //    }
  //
  //    if (count_manual > 60) {
  //      mode = "auto_slide";
  //      count_manual = 0;
  //    }
  //
  //    if ((digitalRead(p_kalibrasi) == HIGH and digitalRead(p_default) == LOW) or (digitalRead(p_kalibrasi) == LOW and digitalRead(p_default) == HIGH)) {
  //      mode = "manual_slide";
  //      count_manual = 0;
  //    }
  //  }

  //  if ((digitalRead(p_kalibrasi) == HIGH and digitalRead(p_default) == LOW) or (digitalRead(p_kalibrasi) == LOW and digitalRead(p_default) == HIGH)) {
  //    if (mode == "auto_slide") {
  //      mode = "manual_slide";
  //      prev_manual = millis();
  //      count_manual = 0;
  //    }
  //  }

  //  if (flag > 3) {
  //    flag = 0;
  //  }
  //
  //  if (flag < 0) {
  //    flag = 3;
  //  }
  //
  //  flag = 1;

  //  if (flag == 0) {
  //    float batt = battery_voltage();
  //    text_display("Battery:", String(batt, 1) + "V", "center");
  //  }
  //   if (flag == 1)
  text_display("En:" + String(dist_enco, 0) + "mm", "Gyro:" + String(angVal, 1) + (char)247 , "center");
  //    text_display("Encorder:", String(distance_mm, 1) + "mm", "center");

  //  else if (flag == 2) {
  //    text_display("Gyroscope:", String(angVal, 1) + (char)247, "center");
  //  }
  //  else if (flag == 3) {
  //    text_display(dates, times, "center");
  //  }
}

void text_display(String text1, String text2, String position) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);

  int16_t x1, y1;
  uint16_t w1, h1;
  uint16_t w2 = 0, h2 = 0;
  const char* cstr = text1.c_str();
  display.getTextBounds((char*)cstr, 0, 0, &x1, &y1, &w1, &h1);

  if (text2 != "") {
    const char* cstr = text2.c_str();
    display.getTextBounds((char*)cstr, 0, 0, &x1, &y1, &w2, &h2);
  }

  int16_t xPos1 = (SCREEN_WIDTH - w1) / 2;
  int16_t xPos2 = (SCREEN_WIDTH - w2) / 2;
  int16_t yPos1, yPos2;

  if (position == "top") {
    yPos1 = 0;
  } else if (position == "bottom") {
    yPos1 = SCREEN_HEIGHT - h1;
  } else { // center
    yPos1 = (SCREEN_HEIGHT - (h1 + h2)) / 2;
  }

  yPos2 = yPos1 + h1;

  display.setCursor(xPos1, yPos1);
  display.print(text1);

  if (text2 != "") {
    display.setCursor(xPos2, yPos2 + 5);
    display.print(text2);
  }

  display.display();
}

void showIndicator(bool show) {
  if (show) {
    display.fillRect(SCREEN_WIDTH - INDICATOR_SIZE, 0, INDICATOR_SIZE, INDICATOR_SIZE, WHITE);
  } else {
    display.fillRect(SCREEN_WIDTH - INDICATOR_SIZE, 0, INDICATOR_SIZE, INDICATOR_SIZE, BLACK);
  }
  display.display();
}
