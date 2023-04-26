/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */
 
#include <SPI.h>
#include <Ethernet.h>
#include <M24C64.h>
#include "config.h"
#include "Time_app.hpp"

M24C64 eep;
#define USE_SPRESENSE_SPI5
#define ARDUINO_ARCH_SPRESENSE

/* Enter a MAC address and IP address for your controller below.
   The IP address will be dependent on your local network: */
byte mac[] = {
  0x70, 0xB3, 0xD5, 0x1F, 0xB0, 0xA6
};
/* クライアントのIPをサーバーのIPと同じネットワークに変更してください */
IPAddress ip(192, 168, 2, 11);
EthernetClient client;
bool printWebData = false;
byte buffer[RECEIVE_MESSAGE_SIZE];
uint32_t len = 0;
uint32_t byteCount = 0;
uint16_t testCount = 0;
Time_appClass *time_app;

bool isI2C(uint8_t address)
{
  Wire.beginTransmission(address);
  if (Wire.endTransmission() == 0)
  {
    return true;
  }
  return false;
}

void setup() {
  Serial.begin(115200);
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }

  time_app = Time_appClass::getInstance();
  time_app->setTimeCount(TESTCOUNTMAX);
  // W5500-Ether用の初期化方法
  digitalWrite(21, LOW);//W5500_Eth RESET# = HIGH
  delay(500);
  digitalWrite(21, HIGH);//W5500_Eth RESET# = HIGH
  Serial.println("W5500 reset done.");

  /* W5500-Ether用のMACアドレス取得処理
     I2C device scan */
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

  /* start the Ethernet connection and the server */
  Ethernet.begin(mac, ip);

  /* Check for Ethernet hardware present */
  if (Ethernet.hardwareStatus() == EthernetNoHardware) {
    Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
    while (true) {
      delay(1); // do nothing, no point running without Ethernet hardware
    }
  }  
  delay(1000);
  Serial.print("connecting to ");
  Serial.print(HTTP_HOST_NAME);
  Serial.println("...");

  getData();
}

void getData() {
  /* if you get a connection, report back via serial: */
  while(1)
  {
    if (client.connect(HTTP_HOST_NAME, HTTP_HOST_PORT))
    {
      break;
    }
    else
    {
      /* if you didn't get a connection to the server: */
      Serial.println("connection failed and try agagin...");
      sleep(1);
    }
  }
  
  Serial.print("connected to ");
  Serial.println(client.remoteIP());
  /* Make a HTTP request: */
  client.println(HTTP_GET_FILE_PATH);
  client.println(HTTP_HOST_HEADER HTTP_HOST_NAME);

  client.println(HTTP_CONNECTION_CLOSE);
  client.println();
  time_app->start();
}

void loop() {
  /* if there are incoming bytes available
     from the server, read them and print them: */
  len = client.available();
  if (len > 0) {
    if (len > RECEIVE_MESSAGE_SIZE) len = RECEIVE_MESSAGE_SIZE;
    client.read(buffer, len);
    if (printWebData) {
      Serial.write(buffer, len); // show in the serial monitor (slows some boards)
    }
    byteCount = byteCount + len;
  }

  /* if the server's disconnected, stop the client: */
  if (!client.connected()) {
    client.stop();
    time_app->end(byteCount*8);
    byteCount = 0;
    /* do nothing forevermore: */
    if (testCount >= TESTCOUNTMAX) {
      exit(0);
    }
    else {
      getData();
    }
  }
}
