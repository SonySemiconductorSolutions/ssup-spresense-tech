/*
 *  MessageHello.ino - MP Example to communicate message strings
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

#include <MP.h>
#include <MPMutex.h>
#include "sensor_app.h"

/* Create a MPMutex object */
MPMutex mutex(MP_MUTEX_ID0);

int8_t msgid;
// 共有メモリアドレス
uint8_t *addr ;
// payload size
#define MEMSIZE      16

#ifdef SUBCORE

#if   (SUBCORE == 1)
#define SUBID "Sub1"
#elif (SUBCORE == 2)
#define SUBID "Sub2"
#elif (SUBCORE == 3)
#define SUBID "Sub3"
#elif (SUBCORE == 4)
#define SUBID "Sub4"
#elif (SUBCORE == 5)
#define SUBID "Sub5"
#endif

Sensor_appClass *sensor_app;

void create_payload()
{
  int ret;
  // CO2、温湿度取得
  sensor_app->get_measurement();

  /* Busy wait until lock the mutex */
  do {
    ret = mutex.Trylock();
  } while (ret != 0);
  
  // 共有メモリにCO2、温湿度設定 
  sensor_app->set_measurement(addr);  
  Serial.print("[Subcore:] send payload:");
  for ( int i = 0; i < 16; i++ ) {
    Serial.print(addr[i], HEX);
    Serial.print(",");
  }
  Serial.println("");
  
  /* Unlock the mutex */
  mutex.Unlock();
  
  return;
}

void setup()
{
  Serial.begin(115200);
  while (!Serial);
  
  MP.begin();
  /* Receive from MainCore */
  MP.Recv(&msgid, &addr);

  /* get sensor instance*/
  sensor_app = Sensor_appClass::getInstance();
  /* sensor initialize*/
  sensor_app->initialize();
}

void loop()
{
  /* Create a message 送信ペイロードの作成 */
  create_payload();
}
#endif
