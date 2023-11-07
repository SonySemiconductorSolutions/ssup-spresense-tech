/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>

#ifndef DownloadTime_HPP
#define DownloadTime_HPP
/*--------------------------------------------------------------------------*/
/**
 * @class DownloadTime
 * @brief DownloadTime Library Class Definitions.
 */
class DownloadTime
{
public:
  /**
   * To avoid create multiple instance
   */
  static DownloadTime* getInstance()
  {
    static DownloadTime instance;
    return &instance;
  }
  void setTimeCount(const uint16_t value);
  
  void start();
  void end(unsigned long byteCount);
private:
  DownloadTime()
  {
    beginMicros = 0;
    endMicros = 0;
    testCount = 0;
    testCountMax = 0;
  }

  void clearData();
  
  ~DownloadTime() 
  {
  }

  unsigned long beginMicros;
  unsigned long endMicros;

  uint16_t testCount;
  uint16_t testCountMax;
  float downloadTime[1024];
  float rate[1024];
};

// #endif //__cplusplus

/* Time_HPP */
#endif
