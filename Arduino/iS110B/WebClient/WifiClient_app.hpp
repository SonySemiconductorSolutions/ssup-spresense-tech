/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include <Arduino.h>
#include <GS2200Hal.h>
#include <GS2200AtCmd.h>
#include <TelitWiFi.h>
#include "config.h"

#ifndef WifiClient_appClass_HPP
#define WifiClient_appClass_HPP

#define  CONSOLE_BAUDRATE  115200
extern uint8_t  *RespBuffer[];
extern int   RespBuffer_Index;

/*--------------------------------------------------------------------------*/
/**
 * @class WifiClient_appClass
 * @brief WifiClient_appClass Library Class Definitions.
 */
class WifiClient_appClass
{
public:
  /**
   * To avoid create multiple instance
   */
  static WifiClient_appClass* getInstance()
  {
    static WifiClient_appClass instance;
    return &instance;
  }
 
  void initialize();
  ATCMD_RESP_E available(uint8_t* buffer, uint16_t* len);
  ATCMD_RESP_E httpOpen( char *cid, char *host, char *port );
  ATCMD_RESP_E httpConfig( ATCMD_HTTP_HEADER_E param, char *val );
  ATCMD_RESP_E httpSend( char cid, ATCMD_HTTP_METHOD_E type, uint8_t timeout, char *page, char *msg, uint32_t size );
  ATCMD_RESP_E httpClose( char cid );

private:
  WifiClient_appClass()
  {
  }
  WifiClient_appClass(const WifiClient_appClass&);
  WifiClient_appClass& operator=(const WifiClient_appClass&);
  ~WifiClient_appClass() {
  }

  ATCMD_RESP_E recvResponse(uint8_t* buffer, uint16_t* len);

  /* private member */
  TelitWiFi gs2200;
  TWIFI_Params gsparams;
};

// #endif //__cplusplus

/* WifiClient_appClass_HPP */
#endif
