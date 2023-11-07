/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "DownloadTime.hpp"

void DownloadTime::start()
{
  Serial.println( "download timer started -----------------------" );
  beginMicros = micros();
}

void DownloadTime::setTimeCount(const uint16_t value)
{
  testCountMax = value;
}

void DownloadTime::end(unsigned long byteCount)
{ 
  endMicros = micros();
  Serial.println( "download timer stopped -----------------------" );
  downloadTime[testCount] = (float)(endMicros - beginMicros) / 1000000.0;
  rate[testCount] = (float)byteCount * 8 / downloadTime[testCount] / 1000.0;

  Serial.print("Count ");
  Serial.print(testCount + 1);
  Serial.print(": Received ");
  Serial.print(byteCount);
  Serial.print(" bytes in ");
  Serial.print(downloadTime[testCount], 4);
  Serial.print(" seconds");
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
    Serial.print(" Kbps");
    exit(0);
  }
}

void DownloadTime::clearData()
{
  beginMicros = 0;
  endMicros = 0;
  return;
}
