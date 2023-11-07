/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#include "HttpClient.hpp"

void HttpClient::begin()
{
  /* initialize digital pin LED_BUILTIN as an output. */
	pinMode(LED0, OUTPUT);
	digitalWrite( LED0, LOW );   // turn the LED off (LOW is the voltage level)
	Serial.begin( CONSOLE_BAUDRATE ); // talk to PC

	/* Initialize SPI access of GS2200 */
	/* please change the iS110B type which you used*/
	 Init_GS2200_SPI_type(iS110B_TypeB);

	/* Initialize AT Command Library Buffer */
	gsparams.mode = ATCMD_MODE_STATION;
	gsparams.psave = ATCMD_PSAVE_DEFAULT;
	if( gs2200.begin( gsparams ) ){
		ConsoleLog( "GS2200 Initilization Fails" );
		while(1);
	}

	/* GS2200 Association to AP */
	if( gs2200.activate_station( AP_SSID, PASSPHRASE ) ){
		ConsoleLog( "Association Fails" );
		while(1);
	}

	/* turn on LED */	
  digitalWrite( LED0, HIGH );

  downloadTime = DownloadTime::getInstance();
  downloadTime->setTimeCount(TESTCOUNTMAX);
}

ATCMD_RESP_E HttpClient::open( char *cid, char *host, char *port )
{
  ATCMD_RESP_E ret;

  ConsoleLog( "Open Socket" );
  do {
    ret = AtCmd_HTTPOPEN(cid, host, port);
  } while (ATCMD_RESP_OK != ret);
  return ret;
}

ATCMD_RESP_E HttpClient::config( ATCMD_HTTP_HEADER_E param, char *val )
{
  ATCMD_RESP_E ret;
  ret = AtCmd_HTTPCONF(param, val);
  return ret;
}

ATCMD_RESP_E HttpClient::send( char cid, ATCMD_HTTP_METHOD_E type, uint8_t timeout, char *page, char *msg, uint32_t size )
{
  ATCMD_RESP_E ret;
  ret = AtCmd_HTTPSEND(cid, type, timeout, page, msg, size);
  return ret;
}

ATCMD_RESP_E HttpClient::close( char cid )
{
  ATCMD_RESP_E ret;
  do {
		ret = AtCmd_HTTPCLOSE( cid );
	} while( ATCMD_RESP_OK != ret && ATCMD_RESP_INVALID_CID != ret );
  return ret;
}

ATCMD_RESP_E HttpClient::receive(uint8_t* buffer, uint16_t* len)
{
	SPI_RESP_STATUS_E s;
	ATCMD_RESP_E resp;
	uint16_t rxDataLen;
	uint8_t *p;
	static uint32_t count1 = 0;

	
	/* Reset the message ID */
	resp = ATCMD_RESP_UNMATCH;
	
	s = WiFi_Read( buffer, &rxDataLen );
	*len = rxDataLen;

	if( s == SPI_RESP_STATUS_TIMEOUT ){
#ifdef ATCMD_DEBUG_ENABLE
		ConsoleLog("SPI: Timeout");
#endif
		return ATCMD_RESP_TIMEOUT;
	}
	
	if( s == SPI_RESP_STATUS_ERROR ){
#ifdef ATCMD_DEBUG_ENABLE

		/* Zero data length, this should not happen */
		ConsoleLog("GS2200 responds Zero data length");
#endif
		return ATCMD_RESP_ERROR;
	}
	
	p = buffer;
	while( rxDataLen-- ){
		
		/* Parse the received data */
		resp = AtCmd_ParseRcvData( p++ );
	}             
#ifdef ATCMD_DEBUG_ENABLE
	ConsolePrintf( "GS Response: %d\r\n", resp );
#endif    
	return resp;
}

ATCMD_RESP_E HttpClient::getSpeed()
{
  ATCMD_RESP_E resp;
  byte buffer[1500];
  char server_cid = 0;
  uint16_t len;
  uint32_t size = 0;
  
  this->config( HTTP_HEADER_CONNECTION, "close" );
  this->config( HTTP_HEADER_HOST, HTTP_SRVR_IP );

  this->open( &server_cid, HTTP_SRVR_IP, HTTP_PORT );
  
  /* start speed measurement */
  downloadTime->start();
  resp = this->send( server_cid, HTTP_METHOD_GET, 10, FILENAME, "", 0 );

  while(1){
    if( Get_GPIO37Status() ){
      resp = this->receive(buffer, &len);
      size += len;
      if( ATCMD_RESP_OK == resp ){
        
        /* AT+HTTPSEND command is done */
        break;
      }
    }
  }
  if ( ATCMD_RESP_OK == resp ){
    this->close( server_cid );
    downloadTime->end(size);
  }
  return resp;
}