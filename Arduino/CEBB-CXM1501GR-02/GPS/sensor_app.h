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
#define ID_GPS          0x81              /*GPS(GNSS)*/

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
  void set_lat_lon(eltres_board_gga_info *gga_info, uint32_t gnss_time, uint8_t payload_data[16]);
  
private:
  Sensor_appClass()
  {
    
  }
  Sensor_appClass(const Sensor_appClass&);
  Sensor_appClass& operator=(const Sensor_appClass&);
  ~Sensor_appClass() {
  }
};

// #endif //__cplusplus
#endif //SENSOR_APP_CLASS_H
