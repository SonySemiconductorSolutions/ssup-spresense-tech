/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include "sensor_app.h"

void Sensor_appClass::set_pressure(double pressure, uint8_t payload[16])
{
  payload[0] = ID_ENVIRONMENT_2;
  
  uint8_t *ptr = payload;
  ptr += 1;
  
  *(float *)ptr = (float)pressure;
  revStr(ptr, 4);
  return;
}

void Sensor_appClass::revStr(uint8_t* str, int str_size){
    int size = sizeof(str);
    int i,j;
    char tmp = {0};
    
    for(i = 0, j = size - 1; i < size / 2; i++, j--){
        tmp = str[i];
        str[i] = str[j];
        str[j] = tmp;
    }   
    return;    
}
