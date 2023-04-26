/*
   SPDX-License-Identifier: LGPL-2.1-or-later

   Copyright 2022 Sony Semiconductor Solutions Corporation.
*/

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include <RTC.h>
#include <Flash.h>

#ifndef BMP280_appClass_HPP
#define BMP280_appClass_HPP
/*--------------------------------------------------------------------------*/

/**
   @class BMP280_appClass
   @brief BMP280_appClass Library Class Definitions.
*/
#define DATA_FOLDER "bmp280"
#define DATA_MOUNTPT_SDCARD   "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT  ","
#define FILE_PATH_LEN 128
#define DATA_HEADER_LEN 128

class BMP280_appClass
{
  public:
    /**
       To avoid create multiple instance
    */
    static BMP280_appClass* getInstance()
    {
      static BMP280_appClass instance;
      return &instance;
    }
    void initialize(unsigned int time_interval);
    unsigned int get_interval(void);
    void save_data(double temp_act, double press_act);
    typedef enum
    { INTO_SD,
      INTO_FLASH
    } where_t;
  private:
    BMP280_appClass()
    {

    }
    BMP280_appClass(const BMP280_appClass&);
    BMP280_appClass& operator=(const BMP280_appClass&);
    ~BMP280_appClass() {
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

/* BMP280_appClass_HPP */
#endif
