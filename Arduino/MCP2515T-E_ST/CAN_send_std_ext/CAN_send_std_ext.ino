/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <SPI.h>
#include <mcp_can.h>

/* unit: ms */
const uint16_t INTERVEL = 1000;
/* Set CS to pin 19 */
MCP_CAN CAN0(19);
byte data[8] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07};
bool isIDStandard = true;

void setup()
{
  Serial.begin(115200);

  /* Initialize MCP2515 running at 16MHz with a baudrate of 500kb/s and the masks and filters disabled. */
  if(CAN0.begin(MCP_ANY, CAN_500KBPS, MCP_16MHZ) == CAN_OK) {
    Serial.println("MCP2515 Initialized Successfully!");
  }  
  else {
    Serial.println("Error Initializing MCP2515...");
  }
  /* Change to normal mode to allow messages to be transmitted */
  CAN0.setMode(MCP_NORMAL);
}


void loop()
{
  /* send data:  ID = 0x100, Standard CAN Frame, Data length = 8 bytes, 'data' = array of data bytes to send */
  byte sndStat;
  if ( isIDStandard ) {
      sndStat = CAN0.sendMsgBuf(0x100, 8, data);
  } else {
   sndStat = CAN0.sendMsgBuf(0x1000, 1, 8, data);
  }
  if(sndStat == CAN_OK){
    Serial.println("Message Sent Successfully!");
  } else {
    Serial.println("Error Sending Message...");
  }
  Serial.print("sndStat = ");
  Serial.println(sndStat);
  isIDStandard = !isIDStandard;
  /* send data per INTERVEL ms */
  delay(INTERVEL);
}
