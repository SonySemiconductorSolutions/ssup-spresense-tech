/*
 *  UsbMsc.ino - Example to Open eMMC on the PC as USB Mass Storage
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <eMMC.h>
// eMMCの電源をコントロールする端子
#define POWER_1V8_ENABLE  26     /* JP1 1: CTS(D27)  3: I2S_BCK(D26) */

void setup() {
  Serial.begin(115200);

  pinMode(POWER_1V8_ENABLE, OUTPUT);
  digitalWrite(POWER_1V8_ENABLE, HIGH);
  delay(100);                                /* delay 無しだとエラーとなる */

  /* Initialize eMMC */
  while (!eMMC.begin()) {
    /* wait until SD card is mounted. */
    Serial.println("Insert eMMC.");
    sleep(1);
  }

  /* Start USB MSC */
  if (eMMC.beginUsbMsc()) {
    Serial.println("USB MSC Failure!");
  } else {
    Serial.println("*** USB MSC Prepared! ***");
    Serial.println("Connect Extension Board USB to PC.");
  }
}

void loop() {
}
