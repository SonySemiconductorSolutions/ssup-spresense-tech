/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include "BMI270_Arduino.h"

#ifndef BMI270_appClass_HPP
#define BMI270_appClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class BMI270_appClass
 * @brief BMI270_appClass Library Class Definitions.
 */

class BMI270_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static BMI270_appClass* getInstance()
  {
    static BMI270_appClass instance;
    return &instance;
  }
  int8_t configure_sensor(BMI270Class *BMI270, float acc_range, float gyro_range);

private:
  BMI270_appClass()
  {
    
  }
  BMI270_appClass(const BMI270_appClass&);
  BMI270_appClass& operator=(const BMI270_appClass&);
  ~BMI270_appClass() {
  }
  uint8_t setAccelerometerRange(float range);
  uint8_t setGyroRange(float range);

  /* private member */
};

// #endif //__cplusplus

/* BMI270_appClass_HPP */
#endif
