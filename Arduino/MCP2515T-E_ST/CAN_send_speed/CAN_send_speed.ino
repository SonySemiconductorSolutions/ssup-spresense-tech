/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <SPI.h>
#include <File.h>
#include <mcp_can.h>
#include "Time_app.hpp"

#define FILE_PATH "/mnt/spif/VIDEO001.RGB"

/* File object */
File myFile;
/* Set CS to pin 19 */
MCP_CAN CAN0(19);
Time_appClass *time_app;

const uint16_t TESTCOUNTMAX = 10;
byte buf[200*1024] = {0};
unsigned long tLen = 0;
unsigned long rLen = 0;

void setup()
{
  Serial.begin(115200);

  /* Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled. */
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK)
    Serial.println("MCP2515 Initialized Successfully!");
  else
    Serial.println("Error Initializing MCP2515...");
  /* Change to normal mode to allow messages to be transmitted */
  CAN0.setMode(MCP_NORMAL);
  /* Re-open the file for reading */
  myFile = File(FILE_PATH);
  if (myFile) {
    tLen = myFile.available();
    rLen = tLen;
    /* read the remaining to buffer */
    myFile.read(buf, rLen);

    Serial.print("tLen = ");
    Serial.println(tLen);
    myFile.close();
  } else {
    Serial.print(F("SD Card: error on opening file"));
  }
  time_app = Time_appClass::getInstance();
  time_app->setTimeCount(TESTCOUNTMAX);
}

void loop()
{
  rLen = tLen;
  unsigned int count = 0;
  time_app->start();
  /* send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send */
  while ( rLen / 8 )
  {
    byte sndStat = CAN0.sendMsgBuf(0x100, 0, 8, &buf[count * 8]);
    if(sndStat == CAN_OK){
    } else {
      Serial.print("sndStat = ");
      Serial.println(sndStat);
      Serial.println("Error Sending Message...");
    }
    count++;
    rLen -= 8;
  }
  if (rLen)
  {
    byte sndStat = CAN0.sendMsgBuf(0x100, 0, rLen, buf[count * 8]);
    if(sndStat == CAN_OK){
    } else {
      Serial.print("sndStat = ");
      Serial.println(sndStat);
      Serial.println("Error Sending Message...");
    }
  }
  time_app->end(tLen*8);
}
