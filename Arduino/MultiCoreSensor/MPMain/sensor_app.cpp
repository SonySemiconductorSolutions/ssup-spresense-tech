/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */

#include "sensor_app.hpp"

void sensor_appClass::initialize(unsigned int time_interval)
{
  display_menu();
  create_path(DATA_FOLDER_ISX012);
  create_path(DATA_FOLDER_BMI270);
  save_header();
  set_interval(time_interval);
}

void sensor_appClass::save_data(CamImage *img){

  /* Create file name */
  create_file();

  /* Remove the old file with the same file name as new created file,
  * and create new file.
  */
  open_file(true, file_path_isx012);
  myFile.write(img->getImgBuff(), img->getImgSize());
  myFile.close();
}

void sensor_appClass::save_header(void)
{
  rtc = RTC.getTime();
  snprintf(file_path_bmi270, sizeof(file_path_bmi270), "%s/%04d%02d%02d.csv",DATA_FOLDER_BMI270, rtc.year(), rtc.month(),rtc.day());
  open_file(true, file_path_bmi270);
  snprintf(header, sizeof(header),
          "Time%sAccelerometer_X(m/s2)%sAccelerometer_Y(m/s2)%sAccelerometer_Z(m/s2)%sGyro_X(°/s)%sGyro_Y(°/s)%sGyro_Z(°/s)", 
           CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT);
  Serial.println(header);
  myFile.println(header);
  myFile.close();
}

void sensor_appClass::save_data(float ax, float ay, float az, float gx, float gy, float gz)
{
  char data[1024];
  char data_time[32];

  open_file(false, file_path_bmi270);
  snprintf(data_time, sizeof(data_time), "%04d/%02d/%02d/%02d:%02d:%02d%s",rtc.year(), rtc.month(),rtc.day(),rtc.hour(), rtc.minute(),rtc.second(), CSV_FORMAT); 

  char accelerometer_x[32];
  char accelerometer_y[32];
  char accelerometer_z[32];
  snprintf(accelerometer_x, sizeof(accelerometer_x), "%f%s", ax, CSV_FORMAT);
  snprintf(accelerometer_y, sizeof(accelerometer_y), "%f%s", ay, CSV_FORMAT);
  snprintf(accelerometer_z, sizeof(accelerometer_z), "%f%s", az, CSV_FORMAT);

  char gyro_x[32];
  char gyro_y[32];
  char gyro_z[32];
  snprintf(gyro_x, sizeof(gyro_x), "%f%s", gx, CSV_FORMAT);
  snprintf(gyro_y, sizeof(gyro_y), "%f%s", gy, CSV_FORMAT);
  snprintf(gyro_z, sizeof(gyro_z), "%f%s", gz, CSV_FORMAT);

  snprintf(data, sizeof(data), "%s%s%s%s%s%s%s\n", data_time, accelerometer_x, accelerometer_y, accelerometer_z, gyro_x, gyro_y, gyro_z);

  myFile.print(data);
  myFile.close();
}

void sensor_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

unsigned int sensor_appClass::get_interval(void)
{
  return interval;
}

void sensor_appClass::display_menu(void)
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

void sensor_appClass::create_path(const char* name)
{
  int ret;
  char dir_path[64];
  
  /* If install to SD Card, check whether the SD Card is inserted. */
  if (where == INTO_SD) {
    struct stat buf;    
    for (;;) {
      ret = stat("/mnt/sd0", &buf);
      if (ret) {
        Serial.println("Please insert formatted SD Card.");
        sleep(1);
      } else {
        break;
      }
    }
    snprintf(dir_path, sizeof(dir_path), "%s%s", DATA_MOUNTPT_SDCARD, name);
  } else if (where == INTO_FLASH) {
    snprintf(dir_path, sizeof(dir_path), "%s%s", DATA_MOUNTPT_SPIFLASH, name);
  }

  mkdir(dir_path, 0777);
}

void sensor_appClass::create_file(void)
{
  rtc = RTC.getTime();
  snprintf(file_path_isx012, sizeof(file_path_isx012), "%s/%04d%02d%02d_%02d_%02d_%02d.JPG",DATA_FOLDER_ISX012, rtc.year(), rtc.month(),rtc.day(),rtc.hour(),rtc.minute(),rtc.second());
  Serial.print("Save taken picture as ");
  Serial.println(file_path_isx012);
}

void sensor_appClass::open_file(bool is_delete, const char* file_path)
{
  printf("file_path is %s\n", file_path);
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
