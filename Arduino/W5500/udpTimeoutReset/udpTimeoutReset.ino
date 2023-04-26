/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <stdio.h> 
// ----------------------------------------------------------------------- //
//Add-onボード用インクルード
#include <SPI.h>
#include <Ethernet.h>
#include <M24C64.h>
#include <EthernetUdp.h>

// ----------------------------------------------------------------------- //
//Add-onボード搭載EEPROMへのアクセス用
M24C64 eep;
// ----------------------------------------------------------------------- //

// ----------------------------------------------------------------------- //
//Add-onボード用のSPIはSPI5となるため、本ライブラリ使用時は以下の定義が必要(socket.cpp内にて使用)
//参考：https://developer.sony.com/develop/spresense/docs/arduino_developer_guide_ja.html#_spi_%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA
#define USE_SPRESENSE_SPI5
#define MESSAGE_SIZE 8
// ----------------------------------------------------------------------- //
uint8_t message[MESSAGE_SIZE];
uint8_t count = 0;
uint16_t timeout_count = 0;

byte mac[] = {
  0x70, 0xB3, 0xD5, 0x1F, 0xB0, 0xA6
};
IPAddress ip(192, 168, 1, 11);
unsigned int localPort = 58888;

IPAddress remote(192, 168, 1, 9);
int pClocalPort = 58888;

EthernetUDP Udp;
uint32_t now_ms = 0;
uint32_t pre_s = 0;
uint32_t now_s = 0;
const uint16_t TIMEOUT_COUNT_MAX = 50;

bool isI2C(uint8_t address)
{
  Wire.beginTransmission(address);
  if (Wire.endTransmission() == 0)
  {
    return true;
  }
  return false;
}

void initialize() {
  ////////////////////////////////////////////////////////////////////////////////
  // W5500-Ether用の初期化方法
  digitalWrite(PIN_D21, LOW); //W5500_Eth RESET# = HIGH
  delay(1);                 //RESET should be held low at least 500 us for W5500
  digitalWrite(PIN_D21, HIGH);//W5500_Eth RESET# = HIGH
  Ethernet.init(19);// use I2S_DIN pin for W5500 CS pin
  // W5500-Ether用のMACアドレス取得処理
  // I2C device scan
  Serial.print(F("I2C Devices(0x) : "));
  Wire.begin();
  //I2C spec. have reserved address!! these scanning escape it address.
  for (uint8_t ad = 0x08; ad < 0x77; ad++)
  {
    if (isI2C(ad))
    {
      Serial.print(ad, HEX);
      Serial.write(' ');
    }
  }
  Serial.write('\n');
  //EEPROM MAC ADDRESS read
  eep.init(0x57);
  Serial.println("MAC read from on board eeprom.");
  for(int i=0;i<6;i++)
  {
    mac[i] = eep.read(i);
    Serial.print(mac[i],HEX);
    Serial.print(":");
  }
  Serial.println("");
//////////////////////////////////////////////////////////////////////////////////

  Serial.println("W5500 reset done.");
  
  Ethernet.begin(mac, ip);
  
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1);
    }
  }
  if (Ethernet.linkStatus() == LinkOFF) {
    Serial.println("Ethernet cable is not connected.");
  }

  Serial.print("connecting to ");
  Serial.print(remote);
  Serial.println("...");
  
  Udp.begin(localPort);
}

void sendMessage(uint16_t count_max) {
    int ret = 0;
    timeout_count = 0; 
  // W5500デバイス起因のtimeoutを再送する場合は以下のように
  // endPacket関数の戻り値で判定する(2022/8/9改修済みライブラリ版で適用)
  // 以下はあくまで簡易的な再送実装
  do{
    Udp.beginPacket(remote,pClocalPort);
    Udp.write(message, sizeof(message));
    ret = Udp.endPacket();//ret==0, W5500 timeout error.
    if (ret == 0) {
      timeout_count++;
      Serial.print("UDP send timeout and retry count = ");
      Serial.println(timeout_count);
      if (timeout_count >= count_max) {
        Serial.println("UDP send timeout and reinitialize.");
        initialize();
        return;
      }
    }
  }while(ret == 0);  
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }
  initialize();
}

void loop() {
    pre_s = now_ms/1000;
    now_ms = millis();
    now_s = now_ms/1000;
    
    if(count < 255){
      message[0] = count;
    } else {
      count = 0;
    }
    for (int i = 1; i < MESSAGE_SIZE; i++) {
        message[i] = i;
    }
    
    sendMessage(TIMEOUT_COUNT_MAX);
    
    delay(100);
    count ++;
    if (pre_s != now_s && 0 == now_s%60 ) {
      Serial.print("UDP send ok and duration is:");
      Serial.print(now_s/60);
      Serial.println(" minutes-------------------");
    }
}
