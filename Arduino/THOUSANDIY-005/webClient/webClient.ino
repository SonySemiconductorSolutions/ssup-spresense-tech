/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "ESP8266.h"
#include "Time_app.hpp"
#include "config.h"

ESP8266 wifi;
Time_appClass *time_app;
char sendMessage[SEND_MESSAGE_SIZE];

bool isPrint = false;
uint32_t byteCount = 0;
uint32_t len = 0;
uint16_t testCount = 0;
byte receiveBuffer[RECEIVE_MESSAGE_SIZE] = {0};

void getData() {
    memset(receiveBuffer, 0, RECEIVE_MESSAGE_SIZE);
    byteCount = 0;

    if (wifi.createTCP(HTTP_HOST_NAME, HTTP_HOST_PORT)) {
        Serial.print("create tcp ok\r\n");
    } else {
        Serial.print("create tcp err\r\n");
    }
    
    snprintf(sendMessage, sizeof(sendMessage), "%s %s %s\r\nHost: %s\r\nConnection: close\r\n\r\n",\
                  HTTP_METHOD, HTTP_FILE_PATH, HTTP_VERSION, HTTP_HOST_NAME);

    time_app->start();
    
    wifi.send((const uint8_t*)sendMessage, strlen(sendMessage));
    
    while(1) {
        len = wifi.recv(receiveBuffer + byteCount, RECEIVE_MESSAGE_SIZE - byteCount, TIMEOUT);
        if (len > 0) {
            byteCount += len;
        }
        else {
              time_app->end(byteCount);
              
              /* if you want to print the log, please set isPrint = true. */
              if (isPrint) {  
                for(uint32_t i = 0; i < byteCount; i++) {
                  Serial.print((char)receiveBuffer[i]);
                }
              }
            break;
        }
    }
}

void initialize(void)
{
    Serial.begin(115200);
    Serial.print("setup begin\r\n");

    wifi.begin(Serial2, 115200);

    Serial.print("FW Version:");
    Serial.println(wifi.getVersion().c_str());

    if (wifi.setOprToStationSoftAP()) {
        Serial.print("to station + softap ok\r\n");
    } else {
        Serial.print("to station + softap err\r\n");
    }

    if (wifi.joinAP(SSID, PASSWORD)) {
        Serial.print("Join AP success\r\n");

        Serial.print("IP:");
        Serial.println( wifi.getLocalIP());       
    } else {
        Serial.print("Join AP failure\r\n");
    }
    
    if (wifi.disableMUX()) {
        Serial.print("single ok\r\n");
    } else {
        Serial.print("single err\r\n");
    }
    
    Serial.print("setup end\r\n");
}

void setup(void)
{
  initialize();
    
  time_app = Time_appClass::getInstance();
  time_app->setTimeCount(TESTCOUNTMAX); 
}

void loop(void)
{
  if ( testCount < TESTCOUNTMAX )
  { 
    getData();
    testCount++;
  } else {
    exit(0);
  }
}
