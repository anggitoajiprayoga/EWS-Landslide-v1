void setup_sim7600ce() {
  Serial1.begin(115200, PROTOCOL, rx, tx); // ← GSM Serial Port Baud
  Serial.println("Intializing modem...");
  pinMode(RST, OUTPUT);
  digitalWrite(RST, LOW);
  delay(500);
  digitalWrite(RST, HIGH);
  delay(500);
  digitalWrite(RST, LOW);
  text_display("Initialzng", "LTE", "center");
  modem.restart();
  String modemInfo = modem.getModemInfo();
  Serial.print("Modem Info: ");
  Serial.println(modemInfo);
  rssi = String(-113 + (2 * modem.getSignalQuality()));

  send_at("ATI");
  send_at("AT+CPIN?");
  send_at("AT+CSQ");
  send_at("AT+CPSI?");
  send_at("AT+IPR?");
  send_at("AT+CNUM");

  imei = send_at_and_get_response("AT+GSN");
  imei = parse_imei(imei);
  Serial.print("IMEI: ");
  Serial.println(imei);
}

void loop_sim7600() {
  if (Serial1.available()) {
    String data = Serial1.readStringUntil('\n');
    Serial.println(data);
  }
}

String send_at_and_get_response(char *_command) {
  SerialAT.println(_command);
  return xwRespon(2000);
}

String xwRespon(long waktu) {
  long cur_time_respon = millis();
  long old_time_respon = cur_time_respon;
  String response = "";

  while (cur_time_respon - old_time_respon < waktu) {
    cur_time_respon = millis();
    while (SerialAT.available() > 0) {
      response += SerialAT.readString();
    }
  }
  return response;
}

String parse_imei(String response) {
  int imei_start = response.indexOf("AT+GSN");
  if (imei_start != -1) {
    response = response.substring(imei_start + 6); 
    response.trim(); 
    int imei_end = response.indexOf("\n");
    if (imei_end != -1) {
      response = response.substring(0, imei_end); 
    }
  }
  response.trim(); 
  return response;
}

void send_at(char *_command) {
  SerialAT.println(_command);
  wRespon(2000);
}

void wRespon(long waktu) {
  static unsigned long cur_time_respon = 0;
  static unsigned long old_time_respon = 0;
  cur_time_respon = millis();
  old_time_respon = cur_time_respon;
  while (cur_time_respon - old_time_respon < waktu ) {
    cur_time_respon = millis();
    while (SerialAT.available() > 0) {
      Serial.print(SerialAT.readString());
    }
  }
}

void getIMEI() {
  SerialAT.println("AT+CGSN");
  static unsigned long cur_time_respon = 0;
  static unsigned long old_time_respon = 0;
  cur_time_respon = millis();
  old_time_respon = cur_time_respon;
  while (cur_time_respon - old_time_respon < 2000) {
    cur_time_respon = millis();
    while (SerialAT.available() > 0) {
      Serial.print(SerialAT.readString());
      String kode = kode + SerialAT.read();
      String kode_sn = kode.substring(kode.length() - 5);
      Serial.println("kode_sn: " + kode_sn);
    }
  }
}