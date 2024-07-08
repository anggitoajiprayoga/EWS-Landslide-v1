void setup_rtc() {
  Wire.begin();
  Wire2.begin(SDA2, SCL2);
  if (!rtc.begin(&Wire2)) {
    eror_rtc = true;
    Serial.println("Couldn't find RTC");
  } else {
    eror_rtc = false;
    Serial.println("RTC connection successful");
  }
}

void loop_rtc() {
  DateTime now = rtc.now();
  printDateTime(now);
  Year = now.year();
  Month = now.month();
  Day = now.day();
  Hour = now.hour();
  Minute = now.minute();
  Second = now.second();

//  RtcTemperature temp = Rtc.GetTemperature();
//  board_temp = String(temp.AsFloatDegC(), 1);
}

#define countof(a) (sizeof(a) / sizeof(a[0]))
void printDateTime(const DateTime& dt)
{
  char tanggalx[15];
  char waktux[15];
  snprintf_P(tanggalx,
             countof(tanggalx),
             PSTR("%04u-%02u-%02u"),
             dt.year(),
             dt.month(),
             dt.day());

  snprintf_P(waktux,
             countof(waktux),
             PSTR("%02u:%02u:%02u"),
             dt.hour(),
             dt.minute(),
             dt.second() );

  times = String(waktux);
  dates = String(tanggalx);
  dateTime = dates + " " + times;
}
