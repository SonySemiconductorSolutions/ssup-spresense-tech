/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>

#ifndef Time_appClass_HPP
#define Time_appClass_HPP
/*--------------------------------------------------------------------------*/
/**
 * @class Time_appClass
 * @brief Time_appClass Library Class Definitions.
 */
class Time_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static Time_appClass* getInstance()
  {
    static Time_appClass instance;
    return &instance;
  }
  void setTimeCount(const uint16_t value);
  
  void start();
  void end(unsigned long byteCount);
private:
  Time_appClass()
  {
    beginMicros = 0;
    endMicros = 0;
    testCount = 0;
    testCountMax = 0;
  }
  Time_appClass(const Time_appClass&);

  void clearData();
  Time_appClass& operator=(const Time_appClass&);
  ~Time_appClass() {
  }

  unsigned long beginMicros;
  unsigned long endMicros;

  uint16_t testCount;
  uint16_t testCountMax;
  float rate[1024];
};

// #endif //__cplusplus

/* Time_appClass_HPP */
#endif
