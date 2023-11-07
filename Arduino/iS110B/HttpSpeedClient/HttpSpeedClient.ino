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

#include "HttpClient.hpp"

HttpClient *httpClient;

void setup() 
{
  httpClient = HttpClient::getInstance();
  /* wifi connect, initialize,etc */
  httpClient->begin();
}

/* the loop function runs over and over again forever */
void loop() 
{  
  ConsoleLog( "Start HTTP Client");
  if( ATCMD_RESP_OK == httpClient->getSpeed() ) {
    ConsoleLog( "Socket Closed" );
  }
  else {
    ConsoleLog( "getSpeed error occurred.\n" );
  }
}
