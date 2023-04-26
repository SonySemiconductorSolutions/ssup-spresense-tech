/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "data_storage.h"

//#define USE_ANALOG_VOLTAGE

#ifdef USE_ANALOG_VOLTAGE
  #define SENSOR_PIN A0
#else
  #define SENSOR_PIN PIN_D02
#endif


DataStorageClass* datastorage;
int32_t distance = 0;

  
void setup()
{
  #ifdef USE_ANALOG_VOLTAGE
    //do nothing
  #else
    pinMode(SENSOR_PIN, INPUT);
  #endif
  
  Serial.begin(115200);
  while (!Serial);
  
  datastorage=DataStorageClass::GetInstance();
  datastorage->Initialize(1);  
}

void read_sensor() {
  #ifdef USE_ANALOG_VOLTAGE
    distance = analogRead(SENSOR_PIN) * 5;
  #else
    distance = pulseIn(SENSOR_PIN, HIGH);
  #endif
}

void loop()
{
  
  read_sensor();
  
  datastorage->SaveData(distance);
  
  delay((datastorage->GetInterval())*1000);
}
