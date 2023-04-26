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
#include <Camera.h>

#ifndef ISX019_APPClass_HPP
#define ISX019_APPClass_HPP
/*--------------------------------------------------------------------------*/

/**
 * @class isx019_appClass
 * @brief isx019_appClass Library Class Definitions.
 */
#define DATA_FOLDER "isx019"
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 128

class isx019_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static isx019_appClass* getInstance()
  {
    static isx019_appClass instance;
    return &instance;
  }
  void initialize(unsigned int time_interval);
  unsigned int get_interval(void);
  void save_data(CamImage *img);
  typedef enum
  { INTO_SD,
    INTO_FLASH
  }where_t;
private:
  isx019_appClass()
  {
  }
  isx019_appClass(const isx019_appClass&);
  isx019_appClass& operator=(const isx019_appClass&);
  ~isx019_appClass() {
      myFile.close();
  }
  void display_menu(void);
  void create_path(void);
  void create_file(void);
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

/* ISX019_APPClass_HPP */
#endif
