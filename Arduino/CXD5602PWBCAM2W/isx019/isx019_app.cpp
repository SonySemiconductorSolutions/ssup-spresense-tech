/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "isx019_app.hpp"

void isx019_appClass::initialize(unsigned int time_interval)
{
  display_menu();
  create_path();
  set_interval(time_interval);
}

void isx019_appClass::save_data(CamImage *img){

  /* Create file name */
  create_file();

  /* Remove the old file with the same file name as new created file,
  * and create new file.
  */
  open_file(true);
  myFile.write(img->getImgBuff(), img->getImgSize());
  myFile.close();
}

void isx019_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

unsigned int isx019_appClass::get_interval(void)
{
  return interval;
}

void isx019_appClass::display_menu(void)
{
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

      /* Discard inputs except for 1st character */
      Serial.read();
    }

    if (c == '1') {
      Serial.println("1 -> SD Card");
      where = INTO_SD;
      break;
    } else if (c == '2') {
      Serial.println("2 -> SPI-Flash");
      where = INTO_FLASH;
      break;
    } else {
      if ((c != '\n') && (c != '\r'))
        Serial.write(c);
    }
  }
}

void isx019_appClass::create_path(void)
{
  int ret;
  char dir_path[64];
  RTC.begin();
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);

  /* If install to SD Card, check whether the SD Card is inserted. */
  if (where == INTO_SD) {
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
  } else if (where == INTO_FLASH) {
    strncpy(dir_path, DATA_MOUNTPT_SPIFLASH, sizeof(dir_path));
  }

  mkdir(dir_path, 0777);
}

void isx019_appClass::create_file(void)
{
  rtc = RTC.getTime();
  snprintf(file_path, sizeof(file_path), "%s/%04d%02d%02d_%02d_%02d_%02d.JPG",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day(),rtc.hour(),rtc.minute(),rtc.second());
  Serial.print("Save taken picture as ");
  Serial.println(file_path);
}

void isx019_appClass::open_file(bool is_delete)
{
  if (where == INTO_SD) {
    if (true == is_delete && SD.exists(file_path))
    {
        printf("Remove existing file [%s].\n", file_path);
        SD.remove(file_path);
    }
    myFile = SD.open(file_path, FILE_WRITE);
  }
  else{
    if (true == is_delete && Flash.exists(file_path))
    {
        printf("Remove existing file [%s].\n", file_path);
        Flash.remove(file_path);
    }
    myFile = Flash.open(file_path, FILE_WRITE);
  }
  
  /* Verify file open */
  if (!myFile)
  {
     printf("File open error\n");
     exit(1);
  }
}
