/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include <RTC.h>
#include <Flash.h>
#include <Camera.h>

#ifndef SENSOR_APPClass_HPP
#define SENSOR_APPClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class sensor_appClass
 * @brief sensor_appClass Library Class Definitions.
 */
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/"
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/"
#define DATA_FOLDER_BMI270 "bmi270"
#define DATA_FOLDER_ISX012 "isx012"
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 128

class sensor_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static sensor_appClass* getInstance()
  {
    static sensor_appClass instance;
    return &instance;
  }
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void save_data(CamImage *img);
  void save_data(float ax, float ay, float az, float gx, float gy, float gz);

  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  sensor_appClass()
  {
  }
  sensor_appClass(const sensor_appClass&);
  sensor_appClass& operator=(const sensor_appClass&);
  ~sensor_appClass() {
      myFile.close();
  }
  void display_menu(void);
  void create_path(const char* name);
  void save_header();
  void create_file(void);
  void open_file(bool is_delete, const char* file_path);
  void set_interval(unsigned int time_interval);
  
  /* private member */
  unsigned int interval;
  SDClass SD;  
  File myFile;
  RtcTime rtc;
  where_t where;
  char file_path_isx012[FILE_PATH_LEN];
  char file_path_bmi270[FILE_PATH_LEN];
  char header[DATA_HEADER_LEN];
};

// #endif //__cplusplus

/* SENSOR_APPClass_HPP */
#endif
