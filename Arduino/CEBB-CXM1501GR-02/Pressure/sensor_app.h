/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include <EltresAddonBoard.h>

#ifndef SENSOR_APP_CLASS_H
#define SENSOR_APP_CLASS_H
/*--------------------------------------------------------------------------*/

/**
 * @class Sensor_appClass
 * @brief Sensor_appClass Library Class Definitions.
 */
#define ID_ENVIRONMENT_2  0x85            /*気圧圧力照度距離 ペイロード*/


class Sensor_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static Sensor_appClass* getInstance()
  {
    static Sensor_appClass instance;
    return &instance;
  }
  void set_pressure(double pressure, uint8_t payload[16]);
  
private:
  Sensor_appClass()
  {
    
  }
  Sensor_appClass(const Sensor_appClass&);
  Sensor_appClass& operator=(const Sensor_appClass&);
  ~Sensor_appClass() {
  }

  void revStr(uint8_t* str, int str_size);
};

// #endif //__cplusplus
#endif //SENSOR_APP_CLASS_H
