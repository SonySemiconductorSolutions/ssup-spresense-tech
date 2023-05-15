/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include <RTC.h>
#include <Flash.h>
#include "BMI270_Arduino.h"

#ifndef BMI270_appClass_HPP
#define BMI270_appClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class BMI270_appClass
 * @brief BMI270_appClass Library Class Definitions.
 */
#define DATA_FOLDER "bmi270"
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 256

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
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void save_data(float ax, float ay, float az, float gx, float gy, float gz);
  int8_t configure_sensor(BMI270Class *BMI270, float acc_range, float gyro_range);
  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  BMI270_appClass()
  {
    
  }
  BMI270_appClass(const BMI270_appClass&);
  BMI270_appClass& operator=(const BMI270_appClass&);
  ~BMI270_appClass() {
      myFile.close();
  }
  void display_menu(void);
  void create_path(void);
  void save_header(void);
  void open_file(bool is_delete);
  void set_interval(unsigned int time_interval);
  uint8_t setAccelerometerRange(float range);
  uint8_t setGyroRange(float range);

  /* private member */
  unsigned int interval;
  SDClass SD;  
  File myFile;
  RtcTime rtc;
  where_t where;
  char file_path[FILE_PATH_LEN];
  char header[DATA_HEADER_LEN];
};

// #endif //__cplusplus

/* BMI270_appClass_HPP */
#endif
