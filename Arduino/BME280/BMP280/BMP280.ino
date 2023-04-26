/*
   SPDX-License-Identifier: LGPL-2.1-or-later

   Copyright 2022 Sony Semiconductor Solutions Corporation.
*/

#include <Wire.h>
#include <SSCI_BME280.h>
#include "BMP280_app.hpp"

SSCI_BME280 bme280;
uint8_t i2c_addr = 0x76;        /**< I2C Address  */
BMP280_appClass *BMP280_app;

void setup()
{
  uint8_t oversampling_temp = 1;        /**< Temperature oversampling x 1  */
  uint8_t oversampling_pressure = 1;    /**< Pressure oversampling x 1     */
  uint8_t oversampling_humidity = 1;    /**< Humidity oversampling x 1     */
  uint8_t bme280mode = 3;               /**< Normal mode                   */
  uint8_t t_standby = 5;                /**< Tstandby 1000ms               */
  uint8_t filter = 0;                   /**< Filter off                    */
  uint8_t spi_3wire_enable = 0;         /**< 3-wire SPI Disable            */

  Serial.begin(115200);
  Wire.begin();
  BMP280_app = BMP280_appClass::getInstance();

  /* Set every 3 senconds to get data */
  BMP280_app->initialize(3);

  bme280.setMode(i2c_addr, oversampling_temp, oversampling_pressure, oversampling_humidity, bme280mode, t_standby, filter, spi_3wire_enable);
  bme280.readTrim();
}

void loop()
{
  double temperature, pressure, humidity;
  bme280.readData(&temperature, &pressure, &humidity);
  BMP280_app->save_data(temperature, pressure);
  delay((BMP280_app->get_interval()) * 1000);
}
