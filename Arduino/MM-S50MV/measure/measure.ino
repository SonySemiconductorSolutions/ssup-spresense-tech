/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "MM-S50MV.h"
#include "data_storage.h"

DataStorageClass* datastorage;
static  int ledr = 0;
static  int ledg = 0;
static  int ledb = 0;

static  int32_t distance_3d[8][4] = {0};
static  int32_t *ptr_distance_3d = NULL;

static  uint16_t lightquantity_3d[8][4] = {0};
static  uint16_t *ptr_lightquantity_3d = NULL;

static uint16_t lightquantity_1d = 0;
static int32_t distance_1d = 0;
  
void setup()
{
  Serial.begin(115200);
  MMS50MV.begin();

  MMS50MV.skip(MMS50MV_DATA_SIZE);
  delay(500);
  /* sync mode. */
  MMS50MV.set(MMS50MV_CMD_MODE, MMS50MV_MODE_SYNC);
  
  delay(500);
  MMS50MV.skip(MMS50MV_DATA_SIZE);
  /* sync. */
  MMS50MV.sync();

  /* normal mode. */
  MMS50MV.set(MMS50MV_CMD_MODE, MMS50MV_MODE_NOMAL);
  delay(500);

  /* 256frames/s */
  MMS50MV.set(0x10, 0);
  delay(500);
  MMS50MV.skip(MMS50MV_DATA_SIZE);
  datastorage=DataStorageClass::GetInstance();
  datastorage->Initialize(1);  
}

void loop()
{
  lightquantity_1d = MMS50MV.get1p();
  
  distance_1d = MMS50MV.get();

  if ( distance_1d < 0 ) {
    return;
  }
  
  ptr_distance_3d = (int32_t *)distance_3d;
  MMS50MV.get3d(ptr_distance_3d);

  ptr_lightquantity_3d = (uint16_t *)lightquantity_3d;
  MMS50MV.get3p(ptr_lightquantity_3d);
  
  datastorage->SaveData(distance_1d, lightquantity_1d, ptr_distance_3d, ptr_lightquantity_3d);
  
  /* LED color */
  int i = distance_1d& 0xff;
  switch (distance_1d>> 8) {
    default:
      ledr = ledg = ledb = 0;
      break;
    case    6:
      ledr = 0;
      ledg = 0;
      ledb = 255 - i;
      break;
    case    5:
      ledr = 0;
      ledg = 255 - i;
      ledb = 255;
      break;
    case    4:
      ledr = 0;
      ledg = 255;
      ledb = i;
      break;
    case    3:
      ledr = 255 - i;
      ledg = 255;
      ledb = 0;
      break;
    case    2:
      ledr = 255;
      ledg = i;
      ledb = 0;
      break;
    case    1:
      ledr = 255;
      ledg = 0;
      ledb = 255 - i;
      break;
    case    0:
      ledr = 255;
      ledg = 255 - i;
      ledb = 255;
      break;
    }
  MMS50MV.led(ledr,ledg,ledb);
  delay((datastorage->GetInterval())*1000);
}
