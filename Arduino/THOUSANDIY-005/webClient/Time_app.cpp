/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "Time_app.hpp"
#include "config.h"

void Time_appClass::start()
{
  Serial.println( "time_app->start-----------------------" );
  beginMicros = micros();
  return;
}

void Time_appClass::setTimeCount(const uint16_t value)
{
  testCountMax = value;
}
void Time_appClass::end(unsigned long byteCount)
{
  float seconds;

  endMicros = micros();
  Serial.println("disconnecting.");
  Serial.print("Count ");
  Serial.print(testCount + 1);
  Serial.print(": Received ");
  Serial.print(byteCount);
  Serial.print(" bytes in ");
  seconds = (float)(endMicros - beginMicros - TIMEOUT * 1000) / 1000000.0;
  Serial.print(seconds, 4);
  Serial.print(" seconds");
  rate[testCount] = (float)byteCount * 8 / seconds / 1000.0;
  Serial.print(", rate = ");
  Serial.print(rate[testCount]);
  Serial.print(" Kbps");
  Serial.println();
  testCount++;
  
  clearData();

  if ( testCount >= testCountMax )
  {
    float avg_rate = 0.0;
    float sum_rate = 0.0;
    for ( int i = 0; i < testCountMax; i++ )
    {
      sum_rate += rate[i];
      Serial.print(rate[i]);
      Serial.print("\t");
    }
    avg_rate = (float)(sum_rate / testCountMax / 1.0);
    Serial.println();
    Serial.print("average rate:");
    Serial.print(avg_rate);
    Serial.println(" Kbps");
  }
  Serial.println( "time_app->end-----------------------" );
  return;
}

void Time_appClass::clearData()
{
  beginMicros = 0;
  endMicros = 0;
  return;
}
