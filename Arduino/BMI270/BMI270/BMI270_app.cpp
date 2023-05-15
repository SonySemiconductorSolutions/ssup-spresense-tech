/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "BMI270_app.hpp"
#include "BMI270_Arduino.h"

void BMI270_appClass::initialize(unsigned int time_interval)
{
  display_menu();
  create_path();
  save_header();
  set_interval(time_interval);
}

void BMI270_appClass::save_data(float ax, float ay, float az, float gx, float gy, float gz)
{
  char data[1024];
  char data_time[32];

  open_file(false);
  rtc = RTC.getTime();
  sprintf(data_time, "%04d/%02d/%02d/%02d:%02d:%02d%s",rtc.year(), rtc.month(),rtc.day(),rtc.hour(), rtc.minute(),rtc.second(), CSV_FORMAT); 

  char accelerometer_x[32];
  char accelerometer_y[32];
  char accelerometer_z[32];
  sprintf(accelerometer_x, "%f%s", ax, CSV_FORMAT);
  sprintf(accelerometer_y, "%f%s", ay, CSV_FORMAT);
  sprintf(accelerometer_z, "%f%s", az, CSV_FORMAT);

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

void BMI270_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

unsigned int BMI270_appClass::get_interval(void)
{
  return interval;
}

void BMI270_appClass::display_menu(void)
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

void BMI270_appClass::create_path(void)
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

void BMI270_appClass::save_header(void)
{  
  RTC.begin();
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);

  rtc = RTC.getTime();
  snprintf(file_path, sizeof(file_path), "%s/%04d%02d%02d.csv",DATA_FOLDER, rtc.year(), rtc.month(),rtc.day());
  open_file(true);
  snprintf(header, sizeof(header),
          "Time%sAccelerometer_X(m/s2)%sAccelerometer_Y(m/s2)%sAccelerometer_Z(m/s2)%sGyro_X(°/s)%sGyro_Y(°/s)%sGyro_Z(°/s)", 
           CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT, CSV_FORMAT);
  Serial.println(header);
  myFile.println(header);
  myFile.close();
}

void BMI270_appClass::open_file(bool is_delete)
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

int8_t BMI270_appClass::configure_sensor(BMI270Class *BMI270, float acc_range, float gyro_range)
{
  int8_t rslt;
  uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };

  struct bmi2_int_pin_config int_pin_cfg;
  int_pin_cfg.pin_type = BMI2_INT1;
  int_pin_cfg.int_latch = BMI2_INT_NON_LATCH;
  int_pin_cfg.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
  int_pin_cfg.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
  int_pin_cfg.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
  int_pin_cfg.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;

  struct bmi2_sens_config config[2];

  /* Configure the type of feature. */
  config[0].type = BMI2_ACCEL;

  /* NOTE: The user can change the following configuration parameters according to their requirement. */
  /* Set Output Data Rate */
  config[0].cfg.acc.odr = BMI2_ACC_ODR_200HZ;

  /* Gravity range of the sensor (+/- 2G, 4G, 8G, 16G). */
  config[0].cfg.acc.range = setAccelerometerRange(acc_range);

  /* The bandwidth parameter is used to configure the number of sensor samples that are averaged
     if it is set to 2, then 2^(bandwidth parameter) samples
     are averaged, resulting in 4 averaged samples.
     Note1 : For more information, refer the datasheet.
     Note2 : A higher number of averaged samples will result in a lower noise level of the signal, but
     this has an adverse effect on the power consumed.
  */
  config[0].cfg.acc.bwp = BMI2_ACC_NORMAL_AVG4;

  /* Enable the filter performance mode where averaging of samples
     will be done based on above set bandwidth and ODR.
     There are two modes
      0 -> Ultra low power mode
      1 -> High performance mode(Default)
     For more info refer datasheet.
  */
  config[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE;

  /* Configure the type of feature. */
  config[1].type = BMI2_GYRO;

  /* The user can change the following configuration parameters according to their requirement. */
  /* Set Output Data Rate */
  config[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;

  /* Gyroscope Angular Rate Measurement Range.By default the range is 2000dps. */
  config[1].cfg.gyr.range = setGyroRange(gyro_range);

  /* Gyroscope bandwidth parameters. By default the gyro bandwidth is in normal mode. */
  config[1].cfg.gyr.bwp = BMI2_GYR_NORMAL_MODE;

  /* Enable/Disable the noise performance mode for precision yaw rate sensing
     There are two modes
      0 -> Ultra low power mode(Default)
      1 -> High performance mode
  */
  config[1].cfg.gyr.noise_perf = BMI2_POWER_OPT_MODE;

  /* Enable/Disable the filter performance mode where averaging of samples
     will be done based on above set bandwidth and ODR.
     There are two modes
      0 -> Ultra low power mode
      1 -> High performance mode(Default)
  */
  config[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;

  /* Set the accel configurations. */
  rslt = BMI270->set_sensor_config(config, 2);
  if (rslt != BMI2_OK) return rslt;

  /* Map data ready interrupt to interrupt pin. */
  rslt = BMI270->map_data_int(BMI2_DRDY_INT, BMI2_INT1);
  if (rslt != BMI2_OK) return rslt;

  rslt = BMI270->sensor_enable(sens_list, 2);
  if (rslt != BMI2_OK) return rslt;

  return rslt;
}

/* BMI160の同名関数を移植する */
uint8_t BMI270_appClass::setAccelerometerRange(float range) {
  uint8_t bmi270_acc_range;

  if (range <= 2) {
    bmi270_acc_range = BMI2_ACC_RANGE_2G;
  } else if (range <= 4) {
    bmi270_acc_range = BMI2_ACC_RANGE_4G;
  } else if (range <= 8) {
    bmi270_acc_range = BMI2_ACC_RANGE_8G;
  } else {
    bmi270_acc_range = BMI2_ACC_RANGE_16G;
  }

  return bmi270_acc_range;
};

/* BMI160の同名関数を移植する */
uint8_t BMI270_appClass::setGyroRange(float range) {
  uint8_t bmi270_gyro_range;

  if (range >= 2000) {
    bmi270_gyro_range = BMI2_GYR_RANGE_2000;
  } else if (range >= 1000) {
    bmi270_gyro_range = BMI2_GYR_RANGE_1000;
  } else if (range >= 500) {
    bmi270_gyro_range = BMI2_GYR_RANGE_500;
  } else if (range >= 250) {
    bmi270_gyro_range = BMI2_GYR_RANGE_250;
  } else {
    bmi270_gyro_range = BMI2_GYR_RANGE_125;
  }

  return bmi270_gyro_range;
};
