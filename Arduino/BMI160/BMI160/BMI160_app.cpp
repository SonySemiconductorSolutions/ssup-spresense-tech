/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "BMI160_app.hpp"

void BMI160_appClass::initialize(unsigned int time_interval)
{
  display_menu();
  create_path();
  save_header();
  set_interval(time_interval);
}

void BMI160_appClass::save_data(int ax, int ay, int az, float gx, float gy, float gz)
{
  char data[1024];
  char data_time[32];

  open_file(false);
  rtc = RTC.getTime();
  sprintf(data_time, "%04d/%02d/%02d/%02d:%02d:%02d%s",rtc.year(), rtc.month(),rtc.day(),rtc.hour(), rtc.minute(),rtc.second(), CSV_FORMAT); 

  char accelerometer_x[32];
  char accelerometer_y[32];
  char accelerometer_z[32];
  sprintf(accelerometer_x, "%d%s", ax, CSV_FORMAT);
  sprintf(accelerometer_y, "%d%s", ay, CSV_FORMAT);
  sprintf(accelerometer_z, "%d%s", az, CSV_FORMAT);

  char gyro_x[32];
  char gyro_y[32];
  char gyro_z[32];
  sprintf(gyro_x, "%f%s", gx, CSV_FORMAT);
  sprintf(gyro_y, "%f%s", gy, CSV_FORMAT);
  sprintf(gyro_z, "%f%s", gz, CSV_FORMAT);

  snprintf(data, sizeof(data), "%s%s%s%s%s%s%s\n", data_time, accelerometer_x, accelerometer_y, accelerometer_z, gyro_x, gyro_y, gyro_z);

  myFile.print(data);
  myFile.close();
}

void BMI160_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

unsigned int BMI160_appClass::get_interval(void)
{
  return interval;
}

void BMI160_appClass::display_menu(void)
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

void BMI160_appClass::create_path(void)
{
  int ret;
  char dir_path[64];
  
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

void BMI160_appClass::save_header(void)
{  
  RTC.begin();
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);

  rtc = RTC.getTime();
  snprintf(file_path, sizeof(file_path), "%s/%04d%02d%02d.csv",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day());
  open_file(true);
  snprintf(header, sizeof(header),
          "Time%sAccelerometer_X(mg)%sAccelerometer_Y(mg)%sAccelerometer_Z(mg)%sGyro_X(°/s)%sGyro_Y(°/s)%sGyro_Z(°/s)", 
           CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT);
  Serial.println(header);
  myFile.println(header);
  myFile.close();
}

void BMI160_appClass::open_file(bool is_delete)
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
