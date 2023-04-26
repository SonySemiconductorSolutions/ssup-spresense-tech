/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "data_storage.h"

void DataStorageClass::Initialize(unsigned int time_interval) {
  DisplayMenu();
  CreatePath();
  SaveHeader();
  SetInterval(time_interval);
}

void DataStorageClass::SaveData(int32_t onedimensional_distance) {
  
  char data[1024] = {0};
  char data_time[64] = {0};

  rtc = RTC.getTime();
  snprintf(file_path_, sizeof(file_path_), "%s/%04d%02d%02d.csv",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day());
  OpenFile(false);  
  snprintf(data_time, sizeof(data_time), "%04d/%02d/%02d/%02d:%02d:%02d%s", rtc.year(), rtc.month(),rtc.day(),rtc.hour(), rtc.minute(),rtc.second(), CSV_FORMAT); 

  char oned_distance[64] = {0};
  snprintf(oned_distance, sizeof(oned_distance), "%ld%s\n", onedimensional_distance, CSV_FORMAT); 

  snprintf(data, sizeof(data), "%s%s\n", data_time, oned_distance);
  Serial.print(data);
  myFile.print(data);
  myFile.close();
}

void DataStorageClass::SetInterval(unsigned int time_interval) {
  interval_ = time_interval;
}

unsigned int DataStorageClass::GetInterval(void) {
  return interval_;
}

void DataStorageClass::DisplayMenu(void) {
  /* Display Menu */
  Serial.println("Select where to save data?");
  Serial.println(" [1]: SD Card (Insert SD Card on the extension board)");
  Serial.println(" [2]: SPI-Flash");

  /* User Menu Selection */
  int c;
  while (true) {
    Serial.println("");
    Serial.print("Please input number. [1-2] ? ");
    while (!Serial.available());
    c = Serial.read();
    while (Serial.available()) {
      Serial.read(); // Discard inputs except for 1st character
    }
    
    if (c == '1') {
      Serial.println("1 -> SD Card");
      where_ = INTO_SD;
      break;
    } else if (c == '2') {
      Serial.println("2 -> SPI-Flash");
      where_ = INTO_FLASH;
      break;
    } else {
      if ((c != '\n') && (c != '\r'))
        Serial.write(c);
    }
  }    
}

void DataStorageClass::CreatePath(void) {
  int ret;
  char dir_path[64];
  /* If install to SD Card, check whether the SD Card is inserted. */
  if (where_ == INTO_SD) {
    struct stat buf;

    strncpy(dir_path, DATA_MOUNTPT_SDCARD, sizeof(dir_path));
    for (;;) {
      ret = stat("/mnt/sd0", &buf);
      if (ret) {
        Serial.println("Please insert formatted SD Card.");
        sleep(1);
      } else {
        break;
      }
    }
  } else if (where_ == INTO_FLASH) {
    strncpy(dir_path, DATA_MOUNTPT_SPIFLASH, sizeof(dir_path));
  }

  mkdir(dir_path, 0777);
}

void DataStorageClass::SaveHeader(void) {
  RTC.begin();
  /* Set the temporary RTC time */
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);
  
  rtc = RTC.getTime();
  snprintf(file_path_, sizeof(file_path_), "%s/%04d%02d%02d.csv",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day());
  OpenFile(true);
  snprintf(header_, sizeof(header_), "Time%sDistance(mm)", CSV_FORMAT);
  Serial.println(header_);
  myFile.println(header_);
  myFile.close();
  return;
}

void DataStorageClass::OpenFile(bool is_delete) {
  if (where_ == INTO_SD) {
    if (true == is_delete && SD.exists(file_path_)){
      printf("Remove existing file [%s].\n", file_path_);
      SD.remove(file_path_);
    }
    myFile = SD.open(file_path_, FILE_WRITE);
  } else {
    if (true == is_delete && Flash.exists(file_path_)) {
      printf("Remove existing file [%s].\n", file_path_);
      Flash.remove(file_path_);
    }
    myFile = Flash.open(file_path_, FILE_WRITE);
  }
  /* Verify file open */
  if (!myFile) {
    printf("File open error\n");
    exit(1);
  }
}
