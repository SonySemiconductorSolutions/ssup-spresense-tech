/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2022 Sony Semiconductor Solutions Corporation.
 */

#ifndef _CONFIG_H_
#define _CONFIG_H_

/*-------------------------------------------------------------------------*
 * Configration
 *-------------------------------------------------------------------------*/
#define SSID                    "your wifi ssid"
#define PASSWORD                "your passowrd"

#define TIMEOUT                 (10000)    //unit:ms
#define SEND_MESSAGE_SIZE       (1024)
#define RECEIVE_MESSAGE_SIZE    (200*1024)
#define TESTCOUNTMAX            (10)

#define HTTP_HOST_NAME        "server address"
#define HTTP_FILE_PATH        "get file path"
#define HTTP_HOST_PORT        (80)
#define HTTP_METHOD           "GET"
#define HTTP_VERSION          "HTTP/1.1"

#endif /*_CONFIG_H_*/
