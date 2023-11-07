/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "SCD41_app.hpp"

void SCD41_appClass::initialize(unsigned int time_interval)
{
  display_menu();
  create_path();
  save_header();
  set_interval(time_interval);
}

void SCD41_appClass::save_data(uint16_t co2_act, float temp_act, float humidity_act)
{
  char data[1024] = {0};
  char data_time[32] = {0};

  open_file(false);
  rtc = RTC.getTime();
  snprintf(data_time, sizeof(data_time), "%04d/%02d/%02d/%02d:%02d:%02d%s",rtc.year(), rtc.month(),rtc.day(),rtc.hour(), rtc.minute(),rtc.second(), CSV_FORMAT); 

  char co2[32] = {0};
  snprintf(co2, sizeof(co2), "%d%s", co2_act, CSV_FORMAT);
  char temperature[32] = {0};
  snprintf(temperature, sizeof(temperature), "%.2lf%s", temp_act, CSV_FORMAT);
  char humidity[32] = {0};
  snprintf(humidity, sizeof(humidity), "%.2lf%s", humidity_act, CSV_FORMAT);

  snprintf(data, sizeof(data), "%s%s%s%s\n", data_time, co2, temperature, humidity);
  Serial.print(data);
  myFile.print(data);
  myFile.close();
}

void SCD41_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

unsigned int SCD41_appClass::get_interval(void)
{
  return interval;
}

void SCD41_appClass::display_menu(void)
{
  // Display Menu
  Serial.println("Select where to save data?");
  Serial.println(" [1]: SD Card (Insert SD Card on the extension board)");
  Serial.println(" [2]: SPI-Flash");

  // User Menu Selection
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

void SCD41_appClass::create_path(void)
{
  int ret;
  char dir_path[64];
  // If install to SD Card, check whether the SD Card is inserted.
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

void SCD41_appClass::save_header(void)
{  
  RTC.begin();
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);

  rtc = RTC.getTime();
  snprintf(file_path, sizeof(file_path), "%s/%04d%02d%02d.csv",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day());
  open_file(true);
  snprintf(header, sizeof(header), "Time%sCO2(ppm)%sTemperature(℃)%sHumidity(%)", CSV_FORMAT, CSV_FORMAT, CSV_FORMAT);
  Serial.println(header);
  myFile.println(header);
  myFile.close();
}

void SCD41_appClass::open_file(bool is_delete)
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
