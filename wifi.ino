void setup_wifi() {
  String APssid2 = cn;
  WiFi.softAP(APssid2.c_str(), "");
  WiFi.mode(WIFI_AP);
  server.begin();
  server_begin();
}

void loop_wifi() {
  static unsigned long lastTime = 0;
  static bool simbol, state;
  if ((millis() - lastTime) > 500) {
    String get_payload = payload_web();
    //    Serial.println("//web_socket_payload_send");
    //    Serial.println(get_payload);
    notifyClients(get_payload);
    if (state == simbol) {
      state = !simbol;
    }
    lastTime = millis();
  }

  simbol = state;
  if (state == true) {
    ind = " | ";
  }
  else {
    ind = " - ";
  }
  ws.cleanupClients();
}
