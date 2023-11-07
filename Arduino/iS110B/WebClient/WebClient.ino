/*
 *  HTTPClient.ino - GainSpan WiFi Module Control Program
 *  Copyright 2019 Norikazu Goto
 *
 *  This work is free software; you can redistribute it and/or modify it under the terms 
 *  of the GNU Lesser General Public License as published by the Free Software Foundation; 
 *  either version 2.1 of the License, or (at your option) any later version.
 *
 *  This work is distributed in the hope that it will be useful, but without any warranty; 
 *  without even the implied warranty of merchantability or fitness for a particular 
 *  purpose. See the GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along with 
 *  this work; if not, write to the Free Software Foundation, 
 *  Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
*/


#include "Time_app.hpp"
#include "WifiClient_app.hpp"

extern uint8_t ESCBuffer[];
extern uint32_t ESCBufferCnt;

bool printWebData = false;
char server_cid = 0;
char httpsrvr_ip[16];

const uint16_t TESTCOUNTMAX = 10;
Time_appClass *time_app;
WifiClient_appClass *wifiClient_app;
uint32_t byteCount = 0;
ATCMD_RESP_E resp;
const uint32_t BUFF_SIZE = 1500;
byte buffer[BUFF_SIZE];

void parse_httpresponse(char *message)
{
	char *p;
	
	if( (p=strstr( message, "200 OK\r\n" )) != NULL ){
		ConsolePrintf( "Response : %s\r\n", p+8 );
	}
}

void setup() {

	time_app = Time_appClass::getInstance();
  wifiClient_app = WifiClient_appClass::getInstance();

  time_app->setTimeCount(TESTCOUNTMAX);

  /* wifi connect, initialize,etc */
  wifiClient_app->initialize();
}

ATCMD_RESP_E getData(uint32_t* size)
{
  ATCMD_RESP_E resp;
  uint16_t len;
  
  wifiClient_app->httpConfig( HTTP_HEADER_CONNECTION, "close" );
  wifiClient_app->httpConfig( HTTP_HEADER_HOST, HTTP_SRVR_IP );

  wifiClient_app->httpOpen( &server_cid, HTTP_SRVR_IP, HTTP_PORT );
  
  /* start speed measurement */
  time_app->start();
  resp = wifiClient_app->httpSend( server_cid, HTTP_METHOD_GET, 10, FILENAME, "", 0 );
  
  if( ATCMD_RESP_BULK_DATA_RX == resp ) {
    if( Check_CID( server_cid ) ) {
    // parse_httpresponse( (char *)(ESCBuffer+1) );
    }
  }
  else {
    ConsoleLog( "?? Unexpected HTTP Response ??" );
    ConsolePrintf( "Response Code : %d\r\n", resp );
    len = strlen(ESCBuffer) + 6;
  }

  while(1){
    if( Get_GPIO37Status() ){
      resp = wifiClient_app->available(buffer, &len);
      *size += len;
      if( ATCMD_RESP_OK == resp ){
        
        /* AT+HTTPSEND command is done */
        break;
      }
    }
  }
  return resp;
}

/* the loop function runs over and over again forever */
void loop() {
  
  ConsoleLog( "Start HTTP Client");
  if( ATCMD_RESP_OK == getData(&byteCount) ) {
    wifiClient_app->httpClose( server_cid );
    time_app->end(byteCount);
    byteCount = 0;
    ConsoleLog( "Socket Closed" );
  }
  else {
    ConsoleLog( "getData error\n" );
  }
}
