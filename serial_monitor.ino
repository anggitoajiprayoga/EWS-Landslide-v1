void serial_monitor() {
  if ((millis() - prev_sermon) > 1000) {
    Serial.println("cn: " + cn + " | dateTime: " + dateTime + " | timeZone: " + timeZone);
    Serial.println("lat: " + lat + " | lng: " + lng + " | alt: " + alt + " | spd: " + spd + " | hdg: " + hdg);
    Serial.println("encorder: " + String(dist_enco) + " | encorder_mm: " + String(distance_mm) + " | gyroscope: " + String(angVal) + " | gyroscope1: " + String(angVal1) + " | button_kal: " + String(digitalRead(p_kalibrasi)) + " | button_def: " + String(digitalRead(p_default)) + " | battery: " + String(battery_voltage()));
    Serial.println("Set-Angle:" + String(setpoint_X0) + "| Titik awal:" + String(Z0) + "| Ttik Akhir :" + String(Z1) +"mm" + "| Pergeseran :" + String(dist_slide, 1) + "cm"); 
    Serial.println("broker: " + broker + " | username: " + username + " | password: " + password  + " | interval_publish: " + interval_mqtt  + " | en_lte: " + en_lte  + " | en_mqtt: " + en_mqtt); 
    Serial.println("condition_state: " + condition_state + " | from_safe: " + from_safe + " | to_safe: " + to_safe  + " | from_alert: " + from_alert  + " | to_alert: " + to_alert  + " | from_caution: " + from_caution  + " | to_caution: " + to_caution + " | from_danger: " + from_danger + " | to_danger: " + to_danger); 
    Serial.println();
    prev_sermon = millis();
  }
}
