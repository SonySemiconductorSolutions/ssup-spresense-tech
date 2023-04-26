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

#define TIMEOUT                 (10000)    //unit:ms
#define SEND_MESSAGE_SIZE       (1024)
#define RECEIVE_MESSAGE_SIZE    (20*1024)
#define TESTCOUNTMAX            (10)

/* サーバーアドレスに変更してください */
#define HTTP_HOST_NAME          "192.168.2.9"
#define HTTP_HOST_HEADER        "Host: "
/* HTTP GETのサーバー上のファイルパスにを変更してください */
#define HTTP_GET_FILE_PATH      "GET /myexample.com/test1.json HTTP/1.1"

#define HTTP_HOST_PORT          (80)
#define HTTP_CONNECTION_CLOSE   "Connection: close"

#endif /*_CONFIG_H_*/
