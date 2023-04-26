/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */
#include <BMI160Gen.h>
#include "BMI160_app.hpp"
BMI160_appClass *BMI160_app;
const float FLOAT_16_MAX = 32768.0;

void setup() {
  /* initialize Serial communication */
  Serial.begin(115200);

  /* wait for the serial port to open */
  while (!Serial);

  /* initialize device */
  Serial.println("Initializing IMU device...");
  BMI160.begin(BMI160GenClass::I2C_MODE);
  uint8_t dev_id = BMI160.getDeviceID();
  Serial.print("DEVICE ID: ");
  Serial.println(dev_id, HEX);

  /* Set the accelerometer range to 250 degrees/second */
  BMI160.setGyroRange(250);
  Serial.println("Initializing IMU device...done.");
  Serial.print("getGyroDLPFMode: ");
  Serial.println(BMI160.getGyroDLPFMode());
  Serial.print("getGyroRate: ");
  Serial.println(BMI160.getGyroRate());
  Serial.print("getAccelDLPFMode: ");
  Serial.println(BMI160.getAccelDLPFMode());
  Serial.print("getAccelerometerRate: ");
  Serial.println(BMI160.getAccelerometerRate());

  BMI160_app = BMI160_appClass::getInstance();

  /* set every 3 senconds to get data */
  BMI160_app->initialize(3);

  sleep(3);
}

float convertRawGyro(int gRaw) {
  /*
   * since we are using 250 degrees/seconds range
   * -250 maps to a raw value of -32768
   * +250 maps to a raw value of 32767
   */
  float g = (gRaw * 250.0) / FLOAT_16_MAX;

  return g;
}

int convertRawAcc(int aRaw) {
  /* 
   *  since we are using 2g range
   * -2000 mg maps to a raw value of -32768
   * +2000 mg maps to a raw value of 32767
   */

  int mg = (aRaw * 2.0 * 1000) / FLOAT_16_MAX;

  return mg;
}

void getGyro(float *gx, float *gy, float *gz) {
  /* raw gyro values */
  int gxRaw, gyRaw, gzRaw;

  /* read raw gyro measurements from device */
  
  BMI160.readGyro(gxRaw, gyRaw, gzRaw);
  
  /* convert the raw gyro data to degrees/second */
  *gx = convertRawGyro(gxRaw);
  *gy = convertRawGyro(gyRaw);
  *gz = convertRawGyro(gzRaw);
  Serial.print("convert to °/s ±250 Range:\t");
  Serial.print("x:");
  Serial.print(*gx);
  Serial.print("°/s");
  Serial.print("\t\t");
  Serial.print("y:");
  Serial.print(*gy);
  Serial.print("°/s");
  Serial.print("\t\t");
  Serial.print("z:");
  Serial.print(*gz);
  Serial.print("°/s");
  Serial.println();
  Serial.println("-------------------------------------------------------------------------------------------------");
}

void getAcc(int *ax, int *ay, int *az) {
  /* raw acc values */
  int axRaw, ayRaw, azRaw;

  /* read raw acc measurements from device */
  BMI160.readAccelerometer(axRaw, ayRaw, azRaw);
  
  /* convert the raw acc data to mg(0.001 g) */
  *ax = convertRawAcc(axRaw);
  *ay = convertRawAcc(ayRaw);
  *az = convertRawAcc(azRaw);
  Serial.print("convert to ±2g Range:\t\t");
  Serial.print("x:");
  Serial.print(*ax);
  Serial.print("mg");
  Serial.print("\t\t\t");
  Serial.print("y:");
  Serial.print(*ay);
  Serial.print("mg");
  Serial.print("\t\t");
  Serial.print("z:");
  Serial.print(*az);
  Serial.print("mg");
  Serial.println();
  Serial.println("-------------------------------------------------------------------------------------------------");
}

void loop() {
  int ax, ay, az;
  float gx, gy, gz;

  /* get acc x,y,z (rage -2000~2000 mg) */
  getAcc(&ax, &ay, &az);
  /* get gyro x,y,z (rage -250~250 degrees/second) */
  getGyro(&gx, &gy, &gz);

  BMI160_app->save_data(ax, ay, az, gx, gy, gz);
  delay((BMI160_app->get_interval()) * 1000);
}
