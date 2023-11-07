/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */

//#define USE_SDCARD

#include <Arduino.h>
#include <File.h>

#ifdef USE_SDCARD
#include <SDHCI.h>
#else
#include <eMMC.h>
#endif

#include "Time_app.hpp"

#define EMMC_POWER_PIN 26
#define TESTCOUNTMAX (10)
#define FILE_NAME "test_write.txt"
#define FILE_TXT_NAME "test"
#define FILE_TXT_EXT ".txt"
uint32_t file_count_list[] = {  0,
//                                100,
//                                2*100,
//                                4*100,
//                                8*100,
//                                16*100,
//                                32*100
                             };                            
const uint16_t FILE_COUNT_SIZE = sizeof(file_count_list) / sizeof(uint32_t);

#ifdef USE_SDCARD
SDClass SD;  /**< SDClass object */ 
#else
#endif

File myFile; /**< File object */ 

#define BUFFER_SIZE 512*1024
uint32_t buffer_size_list[] = { 256,
                                512,
                                1024,
                                2*1024,
                                4*1024,
                                8*1024,
                                16*1024,
                                32*1024,
                                64*1024,
                                128*1024,
                                256*1024,
                                512*1024
                              };
uint8_t* file_buffer;
Time_appClass *time_app;
const uint16_t BUFFER_COUNT = sizeof(buffer_size_list) / sizeof(uint32_t);

void creatFile(uint32_t count) {
  printf("start create %d files.\n", count);
#ifdef USE_SDCARD
    while (!SD.begin()) {
      ; /* wait until SD card is mounted. */
    }
#else
    /* Initialize eMMC */
    eMMC.begin(EMMC_POWER_PIN);
#endif

  for (int i = 0; i < count; i++) {
    char file_name_txt[32] = {0};
    snprintf(file_name_txt, sizeof(file_name_txt), "%s%07d%s", FILE_TXT_NAME, i, FILE_TXT_EXT);
 #ifdef USE_SDCARD
    /* open the file for reading */
    myFile = SD.open(file_name_txt, FILE_WRITE);
#else
    /* open the file for reading */
    myFile = eMMC.open(file_name_txt, FILE_WRITE);
#endif
    
    if (myFile) {
      myFile.println("testing 1, 2, 3.");
     }
     myFile.close();
  }
#ifdef USE_SDCARD
#else
    /* Finalize eMMC */
    eMMC.end();
#endif
  printf("create %d files over. and test start...\n", count);
}

void delFile(uint32_t count) {
  printf("start delete %d files.\n", count);
#ifdef USE_SDCARD
    while (!SD.begin()) {
      ; /* wait until SD card is mounted. */
    }
#else
    /* Initialize eMMC */
    eMMC.begin(EMMC_POWER_PIN);
#endif

  for (int i = 0; i < count; i++) {
    char file_name_txt[32] = {0};
    snprintf(file_name_txt, sizeof(file_name_txt), "%s%07d%s", FILE_TXT_NAME, i, FILE_TXT_EXT);
#ifdef USE_SDCARD
    if (SD.exists(file_name_txt)){
      SD.remove(file_name_txt);
    }
#else
    if (eMMC.exists(file_name_txt)){
      eMMC.remove(file_name_txt);
    }
#endif
  }
  printf("delete %d files over.\n", count);
}

void test(uint32_t count) {
//  creatFile(count);
  for (int i = 0; i < (BUFFER_COUNT * TESTCOUNTMAX); i++) {
#ifdef USE_SDCARD
    while (!SD.begin()) {
      ; /* wait until SD card is mounted. */
    }    
    if (SD.exists(FILE_NAME))
    {
//      printf("Remove existing file [%s].\n", FILE_NAME);
      SD.remove(FILE_NAME);
    }
    /* Re-open the file for reading */
    myFile = SD.open(FILE_NAME, FILE_WRITE);
#else
    /* Initialize eMMC */
    eMMC.begin(EMMC_POWER_PIN);
    if (eMMC.exists(FILE_NAME))
    {
      eMMC.remove(FILE_NAME);
    }
    /* Re-open the file for reading */
    myFile = eMMC.open(FILE_NAME, FILE_WRITE);
#endif
    uint32_t buf_size = buffer_size_list[i/TESTCOUNTMAX];
    file_buffer = (uint8_t *)malloc(buf_size);
    if (file_buffer == NULL)
      {
        printf("ERROR: Fail to allocate buffer size=%d\n", buf_size);
        return;
      }
    memset(file_buffer, 0x55, buf_size);
    
    /* start speed measurement */
    time_app->start();  
//     int ret = myFile.print(file_buffer);//Not recommended for use because this API speed is low
     uint32_t ret = myFile.write(file_buffer, buf_size);//speed is fast
      /* end speed measurement */
      time_app->end(ret);
      /* Close the file */
      myFile.close();
      free(file_buffer);
#ifdef USE_SDCARD
#else
    /* Finalize eMMC */
    eMMC.end();
#endif
  }
//  delFile(count);
}

void setup() {

  /* Open serial communications and wait for port to open */
  Serial.begin(115200);
  while (!Serial) {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }
  Serial.print("write file name is:");
  Serial.println(FILE_NAME);
  time_app = Time_appClass::getInstance();
  time_app->setTimeCount(TESTCOUNTMAX);
}

/**
 * @brief Run repeatedly.
 * 
 * @details Does not do anything.
 */
void loop() {
  for (int i = 0; i < FILE_COUNT_SIZE; i++) {
    test(file_count_list[i]);
  }
  exit(0);
}
