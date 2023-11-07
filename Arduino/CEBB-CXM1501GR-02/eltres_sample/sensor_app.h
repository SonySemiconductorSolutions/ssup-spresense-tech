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
#define ID_ENVIRONMENT  0x82              /*温度・湿度・CO2 ペイロード*/
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
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void set_temputrue(double temp_act, uint8_t *payload);
  void set_pressure(double pressure, uint8_t payload[16]);
  void set_lat_lon(eltres_board_gga_info *gga_info, uint32_t gnss_time, uint8_t payload_data[16]);

  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  Sensor_appClass()
  {
    
  }
  Sensor_appClass(const Sensor_appClass&);
  Sensor_appClass& operator=(const Sensor_appClass&);
  ~Sensor_appClass() {
  }
  void set_interval(unsigned int time_interval);
  void Sensor_appClass::revStr(uint8_t* str, int str_size);
  
  //private member
  unsigned int interval;
};

// #endif //__cplusplus
#endif //SENSOR_APP_CLASS_H
