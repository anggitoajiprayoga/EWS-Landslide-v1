void setup_sd_card() {
  if (!SD.begin(SD_CS)) {
    eror_sdcard = true;
    Serial.println(F("Card failed or not present, no SD Card data logging possible..."));
  } else {
    eror_sdcard = false;
    Serial.println(F("Card initialised... file access enabled..."));
  }
  uint8_t cardType = SD.cardType();
  if (cardType == CARD_NONE) {
    eror_sdcard = true;
    Serial.println(F("Tidak Ada Kartu SD"));
    state_sdcard = 1;
    return;
  }
  else {
    eror_sdcard = false;
    state_sdcard = 0;
  }
  Serial.println(F("Succes initialization SD Card"));
}

void check_dir() {
  File file = SD.open("/history.csv");
  if (!file) {
    Serial.println("File /history.csv doens't exist");
    Serial.println("Creating file...");
    writeFile(SD, "/history.csv", "sn, cn, dateTime, timeZone, displacement_z1_z0, inclination_angle_a0, inclination_angle_a1, slope_length_y0, slope_length_y1, horizontal_length_z0, horizontal_length_z1, rotary_encorder, gyroscope, condition_state, battery_voltage, pv_voltage, rssi\n");
  }
  else {
    Serial.println("File /history.csv already exists");
  }
  file.close();

  uint64_t cardSize = SD.cardSize() / (1024 * 1024);
  Serial.printf("SD Card Size: %lluMB\n", cardSize);
  Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
  Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
}

void logging(bool en_log) {
  while (en_log == true) {
     String dataLog = String(sn) + "," +
                     String(cn) + "," +
                     String(dateTime) + "," +
                     String(timeZone) + "," +
                     String(dist_slide, 2) + "," +
                     String(setpoint_X0, 2) + "," +
                     String(angVal, 2) + "," +
                     String(rope_default, 2) + "," +
                     String(long_Y1, 2) + "," +
                     String(Z0, 2) + "," +
                     String(Z1, 2) + "," +
                     String(dist_enco, 2) + "," +
                     String(angVal, 2) + "," +  
                     String(condition_state) + "," +
                     String(battery_voltage(), 2) + "," +
                     String(pv_voltage(), 2) + "," +
                     String(rssi) + "\n";
    Serial.println("dataLog: " + dataLog);
    check_dir();
    appendFile(SD, "/history.csv", dataLog.c_str());
    delay(1000);
    en_log = false;
  }
}

void loop_logging() {
  static unsigned long prev_log = 0;
  static unsigned int count_log = 0;
  if ((millis() - prev_log) > 1000) {
    count_log++;
    prev_log = millis();
  }

  if (count_log > 600) {
    logging(true);
    count_log = 0;
  }
}
