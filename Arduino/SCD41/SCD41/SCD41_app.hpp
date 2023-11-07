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

#ifndef SCD41_appClass_HPP
#define SCD41_appClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class SCD41_appClass
 * @brief SCD41_appClass Library Class Definitions.
 */
#define DATA_FOLDER "scd41"
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 128

class SCD41_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static SCD41_appClass* getInstance()
  {
    static SCD41_appClass instance;
    return &instance;
  }
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void save_data(uint16_t co2_act, float temp_act, float humidity_act);
  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  SCD41_appClass()
  {
    
  }
  SCD41_appClass(const SCD41_appClass&);
  SCD41_appClass& operator=(const SCD41_appClass&);
  ~SCD41_appClass() {
      myFile.close();
  }
  void display_menu(void);
  void create_path(void);
  void save_header(void);
  void open_file(bool is_delete);
  void set_interval(unsigned int time_interval);
  
  //private member
  unsigned int interval;
  SDClass SD;  
  File myFile;
  RtcTime rtc;
  where_t where;
  char file_path[FILE_PATH_LEN];
  char header[DATA_HEADER_LEN];
};

// #endif //__cplusplus
#endif //SCD41_appClass_HPP
