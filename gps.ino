void setupGps() {
  serial_gps.begin(GPSBaud);
}

void loop_gps() {
  if (millis() > 5000 and gps.charsProcessed() < 10) {
    Serial.println("No GPS data received: check wiring");
    eror_gps = true;
  }
  else {
    eror_gps = false;
  }

  if (gps.location.isValid()) {
    lat = String(gps.location.lat());
    lng = String(gps.location.lng());
    spd = String(gps.speed.kmph());
    alt = String(gps.altitude.meters());
    hdg = String(gps.course.deg());
  }

  smartDelay(200);
}

static void smartDelay(unsigned long ms)
{
  unsigned long start = millis();
  do
  {
    while (serial_gps.available())
      gps.encode(serial_gps.read());
  } while (millis() - start < ms);
}
