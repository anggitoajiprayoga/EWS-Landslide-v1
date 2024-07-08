void setup_main() {
  pinMode(p_kalibrasi, INPUT);
  pinMode(p_default, INPUT);
  pinMode(p_sv, INPUT);
  avg_battery.begin();
  avg_pv.begin();
  avg_gyro.begin();
  avg_gyro1.begin();
  avg_Z0.begin();
  avg_Z1.begin();
  if (en_lte == "true") {
    setup_sim7600ce();
  }

  if (en_mqtt == "true") {
    setup_mqtt();
  }
}

void loop_main() {
  if (en_mqtt == "true") {
    loop_mqtt();
  }

  if (digitalRead(p_kalibrasi)) {
    static unsigned long bef_cal = 0;
    static int count_angl = 0;
    if ((millis() - bef_cal) > 1000) {
      count_angl++;
      bef_cal = millis();
    }
    if (count_angl > 8) {
      Serial.println();
      Serial.println("CALIBRATION Get Alpha SUCCESS");
      Serial.println("Get Alpha by Gyroscope Y");
      Serial.println();
      setpoint_X0 = angVal;
      writeFile(SPIFFS, "/setpoint_X0.txt", String(setpoint_X0).c_str());
      text_display("Set-Angle", String(setpoint_X0) , "center");
      count_angl = 0;
      delay(3000);
    }
  }

  if (digitalRead(p_kalibrasi) and digitalRead(p_default)) {
    static unsigned long prev_cal = 0;
    static int count_cal = 0;
    if ((millis() - prev_cal) > 1000) {
      count_cal++;
      prev_cal = millis();
    }

    if (count_cal > 5) {
      count_cal = 0;
      distance_mm = 0;
      encoderPosition = 0;
      setCalibration();
      Serial.println();
      Serial.println("CALIBRATION ZERO POIN SUCCESS");
      Serial.println("Gyroscope & Rotary Encorder");
      Serial.println();
      text_display("GYROSCOPE", "CALBRATN", "center");
      delay(2000);
      text_display("ENCORDER", "CALBRATN", "center");
      delay(2000);
      text_display("CALBRATN", "SUCCESS", "center");
      delay(2000);
    }
  }

  float Y0 = rope_default;
  float Y1 = rope_default + dist_enco;
  float rad_a = setpoint_X0 * (PI / 180.0);
  float rad_b = angVal * (PI / 180.0);

  float xZ0 = sin(rad_a) * Y0;
  Z0 = avg_Z0.reading(xZ0);
  float xZ1 = sin(rad_b) * Y1;
  Z1 = avg_Z1.reading(xZ1);

  float displacement = (Z1 - Z0) / 10;

  dist_slide = (Z1 - Z0) / 10;
  long_Y1 = rope_default + dist_enco;

  static unsigned long prev_update;
  if (en_update == true) {
    if ((millis() - prev_update) > 300000) {
      pub("towerlamp/device/callback/" + cn, "retry update OTA proccess");
      Serial.println("Update Firmware by OTA");
      delay(2000);
      ota();
      prev_update = millis();
    }
  }

  if (firm_ver != xfirm_ver) {
    pub("towerlamp/device/callback/" + cn, "update OTA done");
    writeFile(SPIFFS, "/xfirm_ver.txt", firm_ver.c_str());
    xfirm_ver = readFile(SPIFFS, "/xfirm_ver.txt");
  }

  static unsigned long prev_rst = 0;
  static unsigned int count_reset = 0;
  if ((millis() - prev_rst) > 1000) {
    count_reset++;
    prev_rst = millis();
  }

  if (count_reset > 10800) {
    Serial.println("schedule device restart");
    delay(2000);
    ESP.restart();
  }
}

void event() {
  if ((dist_slide > (from_safe.toFloat() / 10) and dist_slide < (to_safe.toFloat() / 10)) or (dist_slide < ((from_safe.toFloat() / 10) * -1) and dist_slide > ((to_safe.toFloat() / 10) * -1))) {
    condition_state = "safe";
  }
  else if ((dist_slide > (from_alert.toFloat() / 10) and dist_slide < (to_alert.toFloat() / 10)) or (dist_slide < ((from_alert.toFloat() / 10) * -1) and dist_slide > ((to_alert.toFloat() / 10) * -1))) {
    condition_state = "alert";
  }
  else if ((dist_slide > (from_caution.toFloat() / 10) and dist_slide < (to_caution.toFloat() / 10)) or (dist_slide < ((from_caution.toFloat() / 10) * -1) and dist_slide > ((to_caution.toFloat() / 10) * -1))) {
    condition_state = "caution";
  }
  else if ((dist_slide > (from_danger.toFloat() / 10) and dist_slide < (to_danger.toFloat() / 10)) or (dist_slide < ((from_danger.toFloat() / 10) * -1) and dist_slide > ((to_danger.toFloat() / 10) * -1))) {
    condition_state = "danger";
  }
  else if (dist_slide > (to_danger.toFloat() / 10) or dist_slide < ((to_danger.toFloat() / 10) * -1)) {
    condition_state = "danger";
  }
}

void websocket_parse(String input) {
  Serial.println();
  Serial.println("-----MQTT Payload Parse-----");
  Serial.println();
  DynamicJsonDocument doc(1024);
  deserializeJson(doc, input);
  JsonObject obj = doc.as<JsonObject>();

  String xsn  = obj["sn"].as<String>();
  String xcn  = obj["cn"].as<String>();
  String xrope_default  = obj["rope_default"].as<String>();
  String xset_date  = obj["date"].as<String>();
  String xset_time  = obj["time"].as<String>();
  String xfrom_safe  = obj["from_safe"].as<String>();
  String xto_safe  = obj["to_safe"].as<String>();
  String xfrom_alert  = obj["from_alert"].as<String>();
  String xto_alert  = obj["to_alert"].as<String>();
  String xfrom_caution  = obj["from_caution"].as<String>();
  String xto_caution  = obj["to_caution"].as<String>();
  String xfrom_danger  = obj["from_danger"].as<String>();
  String xto_danger  = obj["to_danger"].as<String>();
  String xen_lte  = obj["en_lte"].as<String>();
  String xno_simcard  = obj["no_simcard"].as<String>();
  String xen_mqtt  = obj["en_mqtt"].as<String>();
  String xbroker  = obj["broker"].as<String>();
  String xusername  = obj["username"].as<String>();
  String xpassword  = obj["password"].as<String>();
  String xinterval_mqtt  = obj["interval_mqtt"].as<String>();
  String xserver_ota  = obj["server_ota"].as<String>();
  String xresource_ota  = obj["resource_ota"].as<String>();
  String xcal_gyroscope  = obj["cal_gyroscope"].as<String>();
  String xcal_encorder  = obj["cal_encorder"].as<String>();
  String xget_alpha  = obj["get_alpha"].as<String>();

  if (xsn != "null" and xsn != "") {
    sn = xsn;
    writeFile(SPIFFS, "/sn.txt", xsn.c_str());
  }
  if (xcn != "null" and xcn != "") {
    cn = xcn;
    writeFile(SPIFFS, "/cn.txt", xcn.c_str());
  }
  if (xrope_default != "null" and xrope_default != "") {
    rope_default = xrope_default.toFloat();
    writeFile(SPIFFS, "/rope_default.txt", String(xrope_default).c_str());
  }
  if (xset_date != "null" and xset_date != "") {
    writeFile(SPIFFS, "/set_date.txt", xset_date.c_str());
  }
  if (xset_time != "null" and xset_time != "") {
    writeFile(SPIFFS, "/set_time.txt", xset_time.c_str());

    delay(500);
    String set_date = readFile(SPIFFS, "/set_date.txt");
    String set_time = readFile(SPIFFS, "/set_time.txt") + ":0";
    parsing_setDateTime(set_date, set_time);
    delay(500);
    rtc.adjust(DateTime(value_year.toInt(), value_month.toInt(), value_day.toInt(), value_hour.toInt(), value_minute.toInt(), value_second.toInt()));
    //    Rtc.SetDateTime(RtcDateTime(value_year.toInt(), value_month.toInt(), value_day.toInt(), value_hour.toInt(), value_minute.toInt(), value_second.toInt())); //tahun, bulan, tanggal, jam, menit, detik
    Serial.println("/n===============================");
    Serial.println("Set Timw Success");
    Serial.println("data_set: " + set_date + " " + set_time);
    Serial.println("===============================/n");
    delay(1000);
  }
  if (xfrom_safe != "null" and xfrom_safe != "") {
    from_safe = xfrom_safe;
    writeFile(SPIFFS, "/from_safe.txt", xfrom_safe.c_str());
  }
  if (xto_safe != "null" and xto_safe != "") {
    to_safe = xto_safe;
    writeFile(SPIFFS, "/to_safe.txt", xto_safe.c_str());
  }
  if (xfrom_alert != "null" and xfrom_alert != "") {
    from_alert = xfrom_alert;
    writeFile(SPIFFS, "/from_alert.txt", xfrom_alert.c_str());
  }
  if (xto_alert != "null" and xto_alert != "") {
    to_alert = xto_alert;
    writeFile(SPIFFS, "/to_alert.txt", xto_alert.c_str());
  }
  if (xfrom_caution != "null" and xfrom_caution != "") {
    from_caution = xfrom_caution;
    writeFile(SPIFFS, "/from_caution.txt", xfrom_caution.c_str());
  }
  if (xto_caution != "null" and xto_caution != "") {
    to_caution = xto_caution;
    writeFile(SPIFFS, "/to_caution.txt", xto_caution.c_str());
  }
  if (xfrom_danger != "null" and xfrom_danger != "") {
    from_danger = xfrom_danger;
    writeFile(SPIFFS, "/from_danger.txt", xfrom_danger.c_str());
  }
  if (xto_danger != "null" and xto_danger != "") {
    to_danger = xto_danger;
    writeFile(SPIFFS, "/to_danger.txt", xto_danger.c_str());
  }
  if (xen_lte != "null" and xen_lte != "") {
    en_lte = xen_lte;
    writeFile(SPIFFS, "/en_lte.txt", xen_lte.c_str());
  }
  if (xen_mqtt != "null" and xen_mqtt != "") {
    en_mqtt = xen_mqtt;
    writeFile(SPIFFS, "/en_mqtt.txt", xen_mqtt.c_str());
  }
  if (xbroker != "null" and xbroker != "") {
    broker = xbroker;
    writeFile(SPIFFS, "/broker.txt", xbroker.c_str());
  }
  if (xusername != "null" and xusername != "") {
    username = xusername;
    writeFile(SPIFFS, "/username.txt", xusername.c_str());
  }
  if (xpassword != "null" and xpassword != "") {
    password = xpassword;
    writeFile(SPIFFS, "/password.txt", xpassword.c_str());
  }
  if (xinterval_mqtt != "null" and xinterval_mqtt != "") {
    interval_mqtt = xinterval_mqtt;
    writeFile(SPIFFS, "/interval_mqtt.txt", xinterval_mqtt.c_str());
  }
  if (xserver_ota != "null" and xserver_ota != "") {
    server_ota = xserver_ota;
    writeFile(SPIFFS, "/server_ota.txt", xserver_ota.c_str());
  }
  if (xresource_ota != "null" and xresource_ota != "") {
    resource_ota = xresource_ota;
    writeFile(SPIFFS, "/resource_ota.txt", xresource_ota.c_str());
  }
  if (xno_simcard != "null" and xno_simcard != "") {
    no_simcard = xno_simcard;
    writeFile(SPIFFS, "/no_simcard.txt", xno_simcard.c_str());
  }

  if (xcal_gyroscope != "null" and xcal_gyroscope != "") {
    if (xcal_gyroscope = "true" or xcal_gyroscope == "1") {
      setCalibration();
      Serial.println();
      Serial.println("CALIBRATION ZERO POIN SUCCESS");
      Serial.println("Gyroscope X & Gyroscope Y");
      Serial.println();
      xcal_gyroscope = "";
    }
  }
  if (xcal_encorder != "null" and xcal_encorder != "") {
    if (xcal_encorder = "true" or xcal_encorder == "1") {
      distance_mm = 0;
      encoderPosition = 0;
      Serial.println();
      Serial.println("CALIBRATION ZERO POIN SUCCESS");
      Serial.println("Rotary Encorder");
      Serial.println();
      xcal_encorder = "";
    }
  }
  if (xget_alpha != "null" and xget_alpha != "") {
    if (xget_alpha = "true" or xget_alpha == "1") {
      Serial.println();
      Serial.println("CALIBRATION Get Alpha SUCCESS");
      Serial.println("Get Alpha by Gyroscope Y");
      Serial.println();
      setpoint_X0 = angVal;
      writeFile(SPIFFS, "/setpoint_X0.txt", String(setpoint_X0).c_str());
      xget_alpha = "";
    }
  }
}

void parsing_setDateTime(String date, String time) {
  //2000-4-1
  String value_date = "-" + date;

  int batasdate1 = value_date.indexOf('-');
  int batasdate2 = value_date.indexOf('-', batasdate1 + 1);
  int batasdate3 = value_date.indexOf('-', batasdate2 + 1);
  int batasdate4 = value_date.indexOf('-', batasdate3 + 1);

  value_year = value_date.substring(batasdate1 + 1, batasdate2);
  value_month = value_date.substring(batasdate2 + 1, batasdate3);
  value_day = value_date.substring(batasdate3 + 1, batasdate4);

  //12:59:59
  String value_time = ":" + time;

  int batastime1 = value_time.indexOf(':');
  int batastime2 = value_time.indexOf(':', batastime1 + 1);
  int batastime3 = value_time.indexOf(':', batastime2 + 1);
  int batastime4 = value_time.indexOf(':', batastime3 + 1);

  value_hour = value_time.substring(batastime1 + 1, batastime2);
  value_minute = value_time.substring(batastime2 + 1, batastime3);
  value_second = value_time.substring(batastime3 + 1, batastime4);
}

float battery_voltage() {
  float avg = avg_battery.reading(analogRead(p_sv));
  float value = scaling(avg, 800, 2640, 0, 12);
  return value;
}

float pv_voltage() {
  float avg = avg_pv.reading(analogRead(p_sv1));
  float value = scaling(avg, 800, 2640, 0, 12);
  return value;
}

float scaling(float val, float min1, float max1, float min2, float max2) {
  float y;
  y = (((max2 - min2) / (max1 - min1)) * (val - min1)) + min2;
  return y;
}

float roundToPrecision(float value, int precision) {
  float multiplier = pow(10.0, precision);
  return round(value * multiplier) / multiplier;
}
