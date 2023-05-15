/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */
#include "BMI270_Arduino.h"
#include "BMI270_app.hpp"

BMI270Class BMI270;
BMI270_appClass *BMI270_app;
static const float acc_range = 4.0;
static const float gyro_range = 250.0;

void setup() {
  /* initialize Serial communication */
  Serial.begin(115200);

  /* wait for the serial port to open */
  while (!Serial);

  /* initialize device */
  Serial.println("Initializing IMU device...");
  BMI270.begin(BMI270_I2C, BMI2_I2C_SEC_ADDR);

  /* Set the accelerometer range to 250 degrees/second */
  Serial.println("Initializing IMU device...done.");
  Serial.print("getGyroRate: ");
  Serial.println(gyro_range);
  Serial.print("getAccelerometerRate: ");
  Serial.println(acc_range);

  BMI270_app = BMI270_appClass::getInstance();

  /* set every 3 senconds to get data */
  BMI270_app->initialize(3);

  sleep(3);

  BMI270_app->configure_sensor(&BMI270, acc_range, gyro_range);
}

void loop() {
  float ax, ay, az, gx, gy, gz;

  struct bmi2_sens_float sensor_data;
  BMI270.bmi2_get_sensor_float(&sensor_data);
  
  Serial.print("a:\t");
  Serial.print(sensor_data.acc.x);
  Serial.print("\t");
  Serial.print(sensor_data.acc.y);
  Serial.print("\t");
  Serial.print(sensor_data.acc.z);
  Serial.print("\t\tg:\t");
  Serial.print(sensor_data.gyr.x);
  Serial.print("\t");
  Serial.print(sensor_data.gyr.y);
  Serial.print("\t");
  Serial.print(sensor_data.gyr.z);
  Serial.println();

  ax = sensor_data.acc.x;
  ay = sensor_data.acc.y;
  az = sensor_data.acc.z;
  gx = sensor_data.gyr.x;
  gy = sensor_data.gyr.y;
  gz = sensor_data.gyr.z;

  BMI270_app->save_data(ax, ay, az, gx, gy, gz);
  delay((BMI270_app->get_interval()) * 1000);
}
