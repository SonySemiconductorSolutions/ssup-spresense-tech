/*
 *  eltres_sample.ino - 5回ペイロードを送信して停止するサンプルプログラム
 *  Copyright 2021 CREATIVE JAPAN, LTD.
 */

#include <Arduino.h>
#include "sensor_app.h"

void Sensor_appClass::initialize(unsigned int time_interval)
{
  set_interval(time_interval);
}

void Sensor_appClass::set_interval(unsigned int time_interval)
{
  interval = time_interval;
}

void Sensor_appClass::set_temputrue(double temp_act, uint8_t payload[16])
{
  payload[0] = ID_ENVIRONMENT;
  
  uint8_t *ptr = payload;
  ptr++;
    
  *(float *)ptr = (float)temp_act;
  revStr(ptr, 4);
  return;
}

void Sensor_appClass::set_pressure(double pressure, uint8_t payload[16])
{
  payload[0] = ID_ENVIRONMENT_2;
  
  uint8_t *ptr = payload;
  ptr += 1;
  
  *(float *)ptr = (float)pressure;
  revStr(ptr, 4);
  return;
}

/**
 * @brief ペイロードへ緯度経度を格納
 * @param gga_info ELTRESアドオンボード用ライブラリから取得できるGGA情報
 * @param payload_data ペイロード
 */
void Sensor_appClass:: set_lat_lon(eltres_board_gga_info *gga_info, uint32_t gnss_time, uint8_t payload_data[16]) {
  String lat_string = String((char*)gga_info->m_lat);
  String lon_string = String((char*)gga_info->m_lon);
  int index;
  
  payload_data[0] = ID_GPS;
  // 緯度設定
  index = 0;
  payload_data[1] = (uint8_t)(((lat_string.substring(index,index+1).toInt() << 4)
                + lat_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  payload_data[2] = (uint8_t)(((lat_string.substring(index,index+1).toInt() << 4)
                + lat_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  index += 1;   // skip "."
  payload_data[3] = (uint8_t)(((lat_string.substring(index,index+1).toInt() << 4)
                + lat_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  payload_data[4] = (uint8_t)(((lat_string.substring(index,index+1).toInt() << 4)
                + lat_string.substring(index+1,index+2).toInt()) & 0xff);
  // 経度設定
  index = 0;
  payload_data[5] = (uint8_t)(lon_string.substring(index,index+1).toInt() & 0xff);
  index += 1;
  payload_data[6] = (uint8_t)(((lon_string.substring(index,index+1).toInt() << 4)
                + lon_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  payload_data[7] = (uint8_t)(((lon_string.substring(index,index+1).toInt() << 4)
                + lon_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  index += 1;   // skip "."
  payload_data[8] = (uint8_t)(((lon_string.substring(index,index+1).toInt() << 4)
                + lon_string.substring(index+1,index+2).toInt()) & 0xff);
  index += 2;
  payload_data[9] = (uint8_t)(((lon_string.substring(index,index+1).toInt() << 4)
                + lon_string.substring(index+1,index+2).toInt()) & 0xff);

  payload_data[10] = ( gnss_time >> 24 ) & 0xff;
  payload_data[11] = ( gnss_time >> 16 ) & 0xff;
  payload_data[12] = ( gnss_time >> 8 ) & 0xff;
  payload_data[13] = ( gnss_time ) & 0xff;
  
  payload_data[15] = gga_info->m_pos_status & 0xff;
}

unsigned int Sensor_appClass::get_interval(void)
{
  return interval;
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
