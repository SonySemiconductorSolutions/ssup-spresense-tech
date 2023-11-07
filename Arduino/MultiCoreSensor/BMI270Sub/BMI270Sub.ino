/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */
 
#include <MP.h>
#include "BMI270_app.hpp"

//#define DEBUG_LOG
#define MSGLEN      6
#define MY_MSGID    10

BMI270Class BMI270;
BMI270_appClass *BMI270_app;
static const float acc_range = 4.0;
static const float gyro_range = 250.0;

struct MyPacket {
  volatile int status; /* 0:ready, 1:busy */
  float message[MSGLEN];
};
MyPacket packet;

#ifdef SUBCORE

#if   (SUBCORE == 1)
#define SUBID "Sub1"
#elif (SUBCORE == 2)
#define SUBID "Sub2"
#elif (SUBCORE == 3)
#define SUBID "Sub3"
#elif (SUBCORE == 4)
#define SUBID "Sub4"
#elif (SUBCORE == 5)
#define SUBID "Sub5"
#endif
#endif

void setup() {
  memset(&packet, 0, sizeof(packet));
  MP.begin();
  
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

  BMI270_app->configure_sensor(&BMI270, acc_range, gyro_range);
}

void loop() {
  int ret;

  struct bmi2_sens_float sensor_data;
  BMI270.bmi2_get_sensor_float(&sensor_data);
  
  if (packet.status == 0) {
    /* Create a message */
    packet.message[0] = sensor_data.acc.x;
    packet.message[1] = sensor_data.acc.y;
    packet.message[2] = sensor_data.acc.z;
    packet.message[3] = sensor_data.gyr.x;
    packet.message[4] = sensor_data.gyr.y;
    packet.message[5] = sensor_data.gyr.z;
#ifdef DEBUG_LOG
    Serial.print("a:\t");
    Serial.print(packet.message[0]);
    Serial.print("\t");
    Serial.print(packet.message[1]);
    Serial.print("\t");
    Serial.print(packet.message[2]);
    Serial.print("\t\tg:\t");
    Serial.print(packet.message[3]);
    Serial.print("\t");
    Serial.print(packet.message[4]);
    Serial.print("\t");
    Serial.print(packet.message[5]);
    Serial.println();
#endif  
    /* status -> busy */
    packet.status = 1;
    /* Send to MainCore */
    ret = MP.Send(MY_MSGID, &packet);
    if (ret < 0) {
      printf("MP.Send error = %d\n", ret);
    }
  }
}
