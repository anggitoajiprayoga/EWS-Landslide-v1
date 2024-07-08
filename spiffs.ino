void data_default() // kalau masih  perawan, belum ada database // ntar lu isi defaultnya apa
{
  if (String(readFile(SPIFFS, "/cn.txt")) == "") {
    cn = "EWS-00";
    writeFile(SPIFFS, "/cn.txt", cn.c_str());
  }
  else {
    cn = readFile(SPIFFS, "/cn.txt");
  }

  writeFile(SPIFFS, "/http_username.txt", "admin");
  writeFile(SPIFFS, "/http_password.txt", "admin");
  writeFile(SPIFFS, "/ssid_wifi_ap.txt", cn.c_str());
  writeFile(SPIFFS, "/pass_wifi_ap.txt", "12345678");
  writeFile(SPIFFS, "/cn.txt", cn.c_str());
  writeFile(SPIFFS, "/sn.txt", "1234");
  writeFile(SPIFFS, "/en_lte.txt", "false");
  writeFile(SPIFFS, "/en_mqtt.txt", "false");
  writeFile(SPIFFS, "/broker.txt", "103.88.152.49");
  writeFile(SPIFFS, "/username.txt", "device-client");
  writeFile(SPIFFS, "/password.txt", "yC9QDtWnYM65VERhkswmpT");
  writeFile(SPIFFS, "/interval_mqtt.txt", "5");
  writeFile(SPIFFS, "/server_ota.txt", "storage.ppa-bib.net");
  writeFile(SPIFFS, "/resource_ota.txt", "/firmware/ews/update.bin");
  writeFile(SPIFFS, "/firm_ver.txt", "v1.0");
  writeFile(SPIFFS, "/no_simcard.txt", "08xxxxxxxxxx");

  writeFile(SPIFFS, "/rope_default.txt", "1000");
  writeFile(SPIFFS, "/from_safe.txt", "1");
  writeFile(SPIFFS, "/to_safe.txt", "10");
  writeFile(SPIFFS, "/from_alert.txt", "11");
  writeFile(SPIFFS, "/to_alert.txt", "20");
  writeFile(SPIFFS, "/from_caution.txt", "21");
  writeFile(SPIFFS, "/to_caution.txt", "30");
  writeFile(SPIFFS, "/from_danger.txt", "31");
  writeFile(SPIFFS, "/to_danger.txt", "40");

  Serial.println("default ok");
}

void read_setting()
{
  cn = readFile(SPIFFS, "/cn.txt");

  if (cn == "")
  {
    data_default();
    delay(200);
  }

  cn = readFile(SPIFFS, "/cn.txt");
  sn = readFile(SPIFFS, "/sn.txt");
  http_username = readFile(SPIFFS, "/http_username.txt");
  http_password = readFile(SPIFFS, "/http_password.txt");
  en_lte = readFile(SPIFFS, "/en_lte.txt");
  en_mqtt = readFile(SPIFFS, "/en_mqtt.txt");
  broker = readFile(SPIFFS, "/broker.txt");
  username = readFile(SPIFFS, "/username.txt");
  password = readFile(SPIFFS, "/password.txt");
  interval_mqtt = readFile(SPIFFS, "/interval_mqtt.txt");
  server_ota = readFile(SPIFFS, "/server_ota.txt");
  resource_ota = readFile(SPIFFS, "/resource_ota.txt");
  xfirm_ver = readFile(SPIFFS, "/xfirm_ver.txt");
  rope_default = readFile(SPIFFS, "/rope_default.txt").toFloat();
  from_safe = readFile(SPIFFS, "/from_safe.txt");
  to_safe = readFile(SPIFFS, "/to_safe.txt");
  from_alert = readFile(SPIFFS, "/from_alert.txt");
  to_alert = readFile(SPIFFS, "/to_alert.txt");
  from_caution = readFile(SPIFFS, "/from_caution.txt");
  to_caution = readFile(SPIFFS, "/to_caution.txt");
  from_danger = readFile(SPIFFS, "/from_danger.txt");
  to_danger = readFile(SPIFFS, "/to_danger.txt");
  setpoint_X0 = readFile(SPIFFS, "/setpoint_X0.txt").toFloat();
  encoderPosition = atol(String(readFile(SPIFFS, "/encoderPosition.txt")).c_str());
  no_simcard = readFile(SPIFFS, "/no_simcard.txt");

  _en_lte = readFile(SPIFFS, "/en_lte.txt");
  _en_mqtt = readFile(SPIFFS, "/en_mqtt.txt");
  _broker = readFile(SPIFFS, "/broker.txt");
  _username = readFile(SPIFFS, "/username.txt");
  _password = readFile(SPIFFS, "/password.txt");
  _interval_mqtt = readFile(SPIFFS, "/interval_mqtt.txt");
  _server_ota = readFile(SPIFFS, "/server_ota.txt");
  _resource_ota = readFile(SPIFFS, "/resource_ota.txt");

  Serial.print("SPIFFS Free: "); Serial.println(humanReadableSize((SPIFFS.totalBytes() - SPIFFS.usedBytes())));
  Serial.print("SPIFFS Used: "); Serial.println(humanReadableSize(SPIFFS.usedBytes()));
  Serial.print("SPIFFS Total: "); Serial.println(humanReadableSize(SPIFFS.totalBytes()));
}

String humanReadableSize(const size_t bytes) {
  if (bytes < 1024) return String(bytes) + " B";
  else if (bytes < (1024 * 1024)) return String(bytes / 1024.0) + " KB";
  else if (bytes < (1024 * 1024 * 1024)) return String(bytes / 1024.0 / 1024.0) + " MB";
  else return String(bytes / 1024.0 / 1024.0 / 1024.0) + " GB";
}

String readFile(fs::FS &fs, const char * path) {
  File file = fs.open(path, "r");
  if (!file || file.isDirectory())
  {
    return String();
  }
  String fileContent;
  while (file.available())
  {
    fileContent += String((char)file.read());
  }
  return fileContent;
}

void writeFile(fs::FS &fs, const char * path, const char * message)
{
  File file = fs.open(path, "w");
  if (!file)
  {
    return;
  }
  file.print(message);
}

void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.print(F("Message appending : "));
    Serial.println(message);
    Serial.println("Message appended");
  }
  else {
    Serial.println("Append failed");
  }
  file.close();
}

void notFoundd(AsyncWebServerRequest *request)
{
  request->send(404, "text/plain", "Not found");
}

String processor(const String& var)
{
  if (var == "version")
  {
    return version;
  }
  else if (var == "type_board")
  {
    return type_board;
  }
  else if (var == "sn_dev")
  {
    return readFile(SPIFFS, "/sn_dev.txt");
  }
  else if (var == "cn")
  {
    return readFile(SPIFFS, "/cn.txt");
  }
  else if (var == "sn")
  {
    return readFile(SPIFFS, "/sn.txt");
  }
  else if (var == "rope_default")
  {
    return readFile(SPIFFS, "/rope_default.txt");
  }
  else if (var == "set_time")
  {
    return times;
  }

  else if (var == "set_date")
  {
    return dates;
  }

  else if (var == "from_safe")
  {
    return from_safe;
  }

  else if (var == "to_safe")
  {
    return to_safe;
  }

  else if (var == "from_alert")
  {
    return from_alert;
  }

  else if (var == "to_alert")
  {
    return to_alert;
  }

  else if (var == "from_caution")
  {
    return from_caution;
  }

  else if (var == "to_caution")
  {
    return to_caution;
  }

  else if (var == "from_danger")
  {
    return from_danger;
  }

  else if (var == "to_danger")
  {
    return to_danger;
  }

  else if (var == "en_lte")
  {
    return en_lte;
  }

  else if (var == "selected_enablelte")
  {
    String x = readFile(SPIFFS, "/en_lte.txt");

    if (x == "true")
    {
      return "selected";
    }
    else if (x == "false")
    {
      return " ";
    }
  }

  else if (var == "selected_disablelte")
  {
    String x = readFile(SPIFFS, "/en_lte.txt");

    if (x == "true")
    {
      return " ";
    }
    else if (x == "false")
    {
      return "selected";
    }
  }

  else if (var == "en_mqtt")
  {
    return en_mqtt;
  }

  else if (var == "selected_enablemqtt")
  {
    String x = readFile(SPIFFS, "/en_mqtt.txt");

    if (x == "true")
    {
      return "selected";
    }
    else if (x == "false")
    {
      return " ";
    }
  }

  else if (var == "selected_disablemqtt")
  {
    String x = readFile(SPIFFS, "/en_mqtt.txt");

    if (x == "true")
    {
      return " ";
    }
    else if (x == "false")
    {
      return "selected";
    }
  }

  else if (var == "mqtt_state")
  {
    return mqtt_state;
  }

  else if (var == "broker")
  {
    return broker;
  }

  else if (var == "username")
  {
    return username;
  }

  else if (var == "password")
  {
    return password;
  }

  else if (var == "pass")
  {
    return password;
  }

  else if (var == "interval_mqtt")
  {
    return readFile(SPIFFS, "/interval_mqtt.txt");
  }

  else if (var == "no_simcard")
  {
    return readFile(SPIFFS, "/no_simcard.txt");
  }

  else if (var == "server_ota")
  {
    return readFile(SPIFFS, "/server_ota.txt");
  }

  else if (var == "resource_ota")
  {
    return readFile(SPIFFS, "/resource_ota.txt");
  }

  else if (var == "freespace") {
    int xfreesp = SD.totalBytes() / (1024 * 1024);
    freesp = String(xfreesp) + " MB";
    return freesp;
  }

  else if (var == "usedstorage") {
    int xusedstr = SD.usedBytes() / (1024 * 1024);
    usedstr = String(xusedstr) + " MB";
    return usedstr;
  }

  else if (var == "totalStorage") {
    int xtotalstr = SD.cardSize() / (1024 * 1024);
    totalstr = String(xtotalstr) + " MB";
    return totalstr;
  }

  else
  {
    return String();
  }
}

void server_begin()
{
  server.serveStatic("/", SPIFFS, "/");

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    read_setting();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/logged-out", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(401);
  });

  server.on("/logout", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    request->send(401);
  });

  server.on("/restart", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    Serial.println("================");
    Serial.println("Device Restart");
    Serial.println("================");
    delay(3000);
    ESP.restart();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/default", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    Serial.println("================");
    Serial.println("Reset Default");
    Serial.println("================");
    data_default();
    delay(3000);
    ESP.restart();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/push_traccar", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    Serial.println();
    Serial.println("__________________traccar_test_______________________");
    Serial.println();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/push_http", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    Serial.println();
    Serial.println("__________________http_test_______________________");
    Serial.println();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/publish", HTTP_GET, [](AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    Serial.println();
    Serial.println("__________________mqtt_test_______________________");
    Serial.println();
    request->send(SPIFFS, "/dashboard_ws.html", String(), false, processor);
  });

  server.on("/get", HTTP_GET, [] (AsyncWebServerRequest * request)
  {
    if (!request->authenticate(http_username.c_str(), http_password.c_str()))
      return request->requestAuthentication();
    read_setting();
    request->redirect("/");
  });

  server.onNotFound(notFoundd);
  server.begin();
}
