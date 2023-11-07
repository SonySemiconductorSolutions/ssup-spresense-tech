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
#include "DownloadTime.hpp"

#ifndef HttpClient_HPP
#define HttpClient_HPP

#define  CONSOLE_BAUDRATE  115200

/*--------------------------------------------------------------------------*/
/**
 * @class HttpClient
 * @brief HttpClient Library Class Definitions.
 */
class HttpClient
{
public:
  /**
   * To avoid create multiple instance
   */
  static HttpClient* getInstance()
  {
    static HttpClient instance;
    return &instance;
  }
 
  void begin();
  ATCMD_RESP_E getSpeed();

private:
  HttpClient()
  {
  }
  ~HttpClient() 
  {
  }

  ATCMD_RESP_E open( char *cid, char *host, char *port );
  ATCMD_RESP_E config( ATCMD_HTTP_HEADER_E param, char *val );
  ATCMD_RESP_E send( char cid, ATCMD_HTTP_METHOD_E type, uint8_t timeout, char *page, char *msg, uint32_t size );
  ATCMD_RESP_E receive(uint8_t* buffer, uint16_t* len);
  ATCMD_RESP_E close( char cid );

  /* private member */
  TelitWiFi gs2200;
  TWIFI_Params gsparams;
  DownloadTime *downloadTime;
  const uint16_t TESTCOUNTMAX = 10;
};

// #endif //__cplusplus

/* HttpClient_HPP */
#endif
