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

#ifndef BMI160_appClass_HPP
#define BMI160_appClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class BMI160_appClass
 * @brief BMI160_appClass Library Class Definitions.
 */
#define DATA_FOLDER "bmi160"
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 256

class BMI160_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static BMI160_appClass* getInstance()
  {
    static BMI160_appClass instance;
    return &instance;
  }
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void save_data(int ax, int ay, int az, float gx, float gy, float gz);
  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  BMI160_appClass()
  {
    
  }
  BMI160_appClass(const BMI160_appClass&);
  BMI160_appClass& operator=(const BMI160_appClass&);
  ~BMI160_appClass() {
      myFile.close();
  }
  void display_menu(void);
  void create_path(void);
  void save_header(void);
  void open_file(bool is_delete);
  void set_interval(unsigned int time_interval);
  
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

/* BMI160_appClass_HPP */
#endif
