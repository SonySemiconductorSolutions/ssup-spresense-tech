/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#ifndef _DATA_STORAGE_H_
#define _DATA_STORAGE_H_

#include <Arduino.h>
#include <SDHCI.h>
#include <File.h>
#include <RTC.h>
#include <Flash.h>

#define DATA_FOLDER            "mm-s50mv"
#define DATA_MOUNTPT_SDCARD    "/mnt/sd0/" DATA_FOLDER
#define DATA_MOUNTPT_SPIFLASH  "/mnt/spif/" DATA_FOLDER
#define CSV_FORMAT             ","
#define FILE_PATH_LEN          128
#define DATA_HEADER_LEN        128

/*
 * @class DataStorageClass
 * @brief DataStorageClass Library Class Definitions.
*/
class DataStorageClass {
 public:
  /* To avoid create multiple instance*/
  static DataStorageClass* GetInstance() {
    static DataStorageClass instance;
    return &instance;
  }
  void Initialize(unsigned int time_interval);
  unsigned int GetInterval(void);
  void SaveData(int32_t onedimensional_distance, uint16_t onedimensional_lightquantity,
                int32_t* threedimensional_distance ,uint16_t* threedimensional_lightquantity);
  typedef enum { 
    INTO_SD,
    INTO_FLASH
  }WhereInto;
  
 private:
  DataStorageClass() {}
  DataStorageClass(const DataStorageClass&);
  DataStorageClass& operator=(const DataStorageClass&);
  ~DataStorageClass() { myFile.close(); }
  void DisplayMenu(void);
  void CreatePath(void);
  void SaveHeader(void);
  void OpenFile(bool is_delete);
  void SetInterval(unsigned int time_interval);
  //private member
  unsigned int interval_;
  SDClass SD;  
  File myFile;
  RtcTime rtc;
  WhereInto where_;
  char file_path_[FILE_PATH_LEN];
  char header_[DATA_HEADER_LEN];
};

#endif /* _DATA_STORAGE_H_ */
