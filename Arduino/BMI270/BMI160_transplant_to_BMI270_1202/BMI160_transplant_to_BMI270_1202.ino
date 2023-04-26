/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "BMI270_Arduino.h"

BMI270Class BMI270;

/* Other functions */
int8_t configure_sensor(struct bmi2_dev *dev);

/* Rangeの設定 */
static const float acc_range = 4.0;
static const float gyro_range = 250.0;

void setup(void)
{
  Serial.begin(115200);

  BMI270.begin(BMI270_I2C, BMI2_I2C_SEC_ADDR);

  configure_sensor();

  Serial.println("Press any key to start.");

  while (!Serial.available());
}

void loop(void)
{
  struct bmi2_sens_float sensor_data;
  BMI270.bmi2_get_sensor_float(&sensor_data);

  Serial.print("a:\t");
  Serial.print(sensor_data.acc.x);
  Serial.print("\t");
  Serial.print(sensor_data.acc.y);
  Serial.print("\t");
  Serial.print(sensor_data.acc.z);
  Serial.print("\t\tg:\t");
  Serial.print(sensor_data.gyr.x);
  Serial.print("\t");
  Serial.print(sensor_data.gyr.y);
  Serial.print("\t");
  Serial.print(sensor_data.gyr.z);
  Serial.println();

  /* 0.1秒間隔 */
  usleep(100 * 1000);

}

/* Sensor configuration */
int8_t configure_sensor()
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
  rslt = BMI270.set_sensor_config(config, 2);
  if (rslt != BMI2_OK) return rslt;

  /* Map data ready interrupt to interrupt pin. */
  rslt = BMI270.map_data_int(BMI2_DRDY_INT, BMI2_INT1);
  if (rslt != BMI2_OK) return rslt;

  rslt = BMI270.sensor_enable(sens_list, 2);
  if (rslt != BMI2_OK) return rslt;

  return rslt;
}

/* BMI160の同名関数を移植する */
uint8_t setAccelerometerRange(float range) {
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
uint8_t setGyroRange(float range) {
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
