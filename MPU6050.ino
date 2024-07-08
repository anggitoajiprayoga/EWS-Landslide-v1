void setupI2c() {
  Wire.begin();
  Wire.setClock(35000);
}

void setupmpu() {
  setupI2c();
  mpu.initialize();
  mpu1.initialize();
  devStatus = mpu.dmpInitialize();
  devStatus1 = mpu1.dmpInitialize();
  getCalibration();
  if (devStatus == 0) {
    eror_gyro1 = false;
    mpu.setDMPEnabled(true);
    mpuIntStatus = mpu.getIntStatus();
    packetSize = mpu.dmpGetFIFOPacketSize();
  }
  else {
    eror_gyro1 = true;
    Serial.println("===================================");
    Serial.println("SENSOR FAILED - Gyroscope Y no detect");
    Serial.println("===================================");
  }
  if (devStatus1 == 0) {
    eror_gyro2 = false;
    mpu1.setDMPEnabled(true);
    mpuIntStatus1 = mpu1.getIntStatus();
    packetSize1 = mpu1.dmpGetFIFOPacketSize();
  }
  else {
    eror_gyro2 = true;
    Serial.println("===================================");
    Serial.println("SENSOR FAILED - Gyroscope X no detect");
    Serial.println("===================================");
  }
}

VectorFloat QtoEulerAngle(Quaternion qt) {
  VectorFloat ret;
  double sqw = qt.w * qt.w;
  double sqx = qt.x * qt.x;
  double sqy = qt.y * qt.y;
  double sqz = qt.z * qt.z;
  ret.x = atan2(2.0 * (qt.x * qt.y + qt.z * qt.w), (sqx - sqy - sqz + sqw));
  ret.y = asin(2.0 * (qt.x * qt.z - qt.y * qt.w) / (sqx + sqy + sqz + sqw));
  ret.z = atan2(2.0 * (qt.y * qt.z + qt.x * qt.w), (-sqx - sqy + sqz + sqw));
  //KONFERSI RADIAN KE DERAJAT :
  ret.x = ret.x * 180 / PI;
  ret.y = ret.y * 180 / PI;
  ret.z = ret.z * 180 / PI;
  return ret;
}

void getCalibration() {
  float offsetValues[6];
  File file = SPIFFS.open("/offsets.txt", "r");
  if (!file) {
    Serial.println("Failed to open calibration file for reading");
    return;
  }

  for (int i = 0; i < 6; i++) {
    offsetValues[i] = file.parseFloat();
  }

  file.close();

  mpu.setXAccelOffset(offsetValues[0]);
  mpu.setYAccelOffset(offsetValues[1]);
  mpu.setZAccelOffset(offsetValues[2]);
  mpu.setXGyroOffset(offsetValues[3]);
  mpu.setYGyroOffset(offsetValues[4]);
  mpu.setZGyroOffset(offsetValues[5]);

  float offsetValues1[6];
  File file1 = SPIFFS.open("/offsets1.txt", "r");
  if (!file1) {
    Serial.println("Failed to open calibration file for reading");
    return;
  }

  for (int i1 = 0; i1 < 6; i1++) {
    offsetValues1[i1] = file1.parseFloat();
  }

  file1.close();

  mpu1.setXAccelOffset(offsetValues1[0]);
  mpu1.setYAccelOffset(offsetValues1[1]);
  mpu1.setZAccelOffset(offsetValues1[2]);
  mpu1.setXGyroOffset(offsetValues1[3]);
  mpu1.setYGyroOffset(offsetValues1[4]);
  mpu1.setZGyroOffset(offsetValues1[5]);
}

void setCalibration() {
  int Data[6];
  mpu.CalibrateAccel(6);
  mpu.CalibrateGyro(6);
  mpu.PrintActiveOffsets();
  int16_t *offsets = mpu.GetActiveOffsets();

  File file = SPIFFS.open("/offsets.txt", "w");
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }

  for (int i = 0; i < 6; i++) {
    file.print(offsets[i]);
    file.print(" ");
  }

  file.close();
  Serial.println("Offsets saved to SPIFFS");

  int Data1[6];
  mpu1.CalibrateAccel(6);
  mpu1.CalibrateGyro(6);
  mpu1.PrintActiveOffsets();
  int16_t *offsets1 = mpu1.GetActiveOffsets();

  File file1 = SPIFFS.open("/offsets1.txt", "w");
  if (!file1) {
    Serial.println("Failed to open file for writing");
    return;
  }

  for (int i1 = 0; i1 < 6; i1++) {
    file1.print(offsets1[i1]);
    file1.print(" ");
  }

  file1.close();
  Serial.println("Offsets saved to SPIFFS");
}

void loopmpu() {
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    VectorFloat ea = QtoEulerAngle(q);

    float xangVal = ea.z;
    float sangVal = scaling(xangVal, 0, 90, 90, 0);
    angVal = avg_gyro.reading(sangVal);

    if (angVal > 99) {
      angVal = 99;
    }
    if (angVal < -99) {
      angVal = -99;
    }
  }

  if (mpu1.dmpGetCurrentFIFOPacket(fifoBuffer1)) {
    mpu1.dmpGetQuaternion(&q, fifoBuffer1);
    VectorFloat ea1 = QtoEulerAngle(q);

    float xangVal1 = ea1.z;
    angVal1 = avg_gyro1.reading(xangVal1);

    if (angVal1 > 99) {
      angVal1 = 99;
    }
    if (angVal1 < -99) {
      angVal1 = -99;
    }
  }
}
