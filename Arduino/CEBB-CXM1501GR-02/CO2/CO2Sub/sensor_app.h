/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include <SensirionI2CScd4x.h>


#ifndef SENSOR_APP_CLASS_H
#define SENSOR_APP_CLASS_H
/*--------------------------------------------------------------------------*/

/**
 * @class Sensor_appClass
 * @brief Sensor_appClass Library Class Definitions.
 */
#define ID_ENVIRONMENT  0x82              /*温度・湿度・CO2 ペイロード*/


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
  void initialize();
  void get_measurement();
  void set_measurement(uint8_t* payload);
  
private:
  Sensor_appClass() : co2(0), temperature(0.0f), humidity(0.0f) {
  }
  Sensor_appClass(const Sensor_appClass&);
  Sensor_appClass& operator=(const Sensor_appClass&);
  ~Sensor_appClass() {
  }

  void printUint16Hex(uint16_t value);
  void printSerialNumber(uint16_t serial0, uint16_t serial1, uint16_t serial2);
  void revStr(uint8_t* str, int str_size);

  // private member
  SensirionI2CScd4x scd4x;
  uint16_t co2;
  float temperature;
  float humidity;
};

// #endif //__cplusplus
#endif //SENSOR_APP_CLASS_H
