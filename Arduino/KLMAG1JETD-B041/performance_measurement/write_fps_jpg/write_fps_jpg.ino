/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */

//#define USE_SDCARD

#ifdef USE_SDCARD
#include <SDHCI.h>
#else
#include <eMMC.h>
#endif
#include <stdio.h>  /* for sprintf */

#include <Camera.h>

#define BAUDRATE                (115200)
#define FRAMES_NUM              (100) /* 画像枚数 */
#define FILENAME_LEN            (32)
#define DATA_FOLDER             "photo_" // 保存先の共通フォルダー名
#define FILE_NUM_THRESHOLD      200 // 200枚ごと保存先変更
#define EMMC_POWER_PIN 26

#ifdef USE_SDCARD
SDClass theSD;  /**< SDClass object */ 
#else
#endif
int frame_count = 0;
float pre_millis = 0.0;
float now_millis = 0.0;

CAM_VIDEO_FPS fps_list[] = { CAM_VIDEO_FPS_5,
                             CAM_VIDEO_FPS_6,
                             CAM_VIDEO_FPS_7_5,
                             CAM_VIDEO_FPS_15,
                             CAM_VIDEO_FPS_30,
                             CAM_VIDEO_FPS_60,
                             CAM_VIDEO_FPS_120
                             };
float fps_print[] = {5, 6, 7.5, 15, 30, 60, 120};
int res_list[][2] = { {CAM_IMGSIZE_QQVGA_H, CAM_IMGSIZE_QQVGA_V},
                                {CAM_IMGSIZE_QVGA_H, CAM_IMGSIZE_QVGA_V},
                                {CAM_IMGSIZE_VGA_H, CAM_IMGSIZE_VGA_V},
                                {CAM_IMGSIZE_HD_H, CAM_IMGSIZE_HD_V},
                                {CAM_IMGSIZE_QUADVGA_H, CAM_IMGSIZE_QUADVGA_V},
                                {CAM_IMGSIZE_FULLHD_H, CAM_IMGSIZE_FULLHD_V},
                                {CAM_IMGSIZE_3M_H, CAM_IMGSIZE_3M_V},
                                {CAM_IMGSIZE_5M_H, CAM_IMGSIZE_5M_V}
                             };
const uint16_t FPS_LIST_SIZE = sizeof(fps_list) / sizeof(CAM_VIDEO_FPS);
const uint16_t RES_LIST_SIZE = sizeof(res_list) / sizeof(res_list[0]);
bool is_test_over = false;                            
/**
   Print error message
*/

void printError(enum CamErr err)
{
  Serial.print("Error: ");
  switch (err)
  {
    case CAM_ERR_NO_DEVICE:
      Serial.println("No Device");
      break;
    case CAM_ERR_ILLEGAL_DEVERR:
      Serial.println("Illegal device error");
      break;
    case CAM_ERR_ALREADY_INITIALIZED:
      Serial.println("Already initialized");
      break;
    case CAM_ERR_NOT_INITIALIZED:
      Serial.println("Not initialized");
      break;
    case CAM_ERR_NOT_STILL_INITIALIZED:
      Serial.println("Still picture not initialized");
      break;
    case CAM_ERR_CANT_CREATE_THREAD:
      Serial.println("Failed to create thread");
      break;
    case CAM_ERR_INVALID_PARAM:
      Serial.println("Invalid parameter");
      break;
    case CAM_ERR_NO_MEMORY:
      Serial.println("No memory");
      break;
    case CAM_ERR_USR_INUSED:
      Serial.println("Buffer already in use");
      break;
    case CAM_ERR_NOT_PERMITTED:
      Serial.println("Operation not permitted");
      break;
    default:
      break;
  }
}


/**
   Callback from Camera library when video frame is captured.
*/

void CamCB(CamImage img)
{
  int save_folder_num = 0;
  /* Check the img instance is available or not. */

  if (img.isAvailable())
  {
    frame_count++;

    if (frame_count > FRAMES_NUM) {
      is_test_over = true;
      return;
    }

    /* Create file name */

    char filepath[FILENAME_LEN] = {0};
    
    // frame_countは1から600までのため、保存先を区別するsave_folder_numを0から2までにする
    save_folder_num = int((frame_count - 1) / FILE_NUM_THRESHOLD);

    // folder_nameはphoto_0, photo_1, photo_2
    String folder_name = DATA_FOLDER + (String)save_folder_num;

#ifdef USE_SDCARD
    // フォルダーがなければ作る
    if (theSD.exists(folder_name) == false) {
      theSD.mkdir(folder_name);
    }
#else
    if (eMMC.exists(folder_name) == false) {
      eMMC.mkdir(folder_name);
    }
#endif

    snprintf(filepath, FILENAME_LEN, "%s%d/HFR%04d.JPG", DATA_FOLDER, save_folder_num, frame_count);

    Serial.print("Save taken picture as ");
    Serial.print(filepath);

    // FPSの計算
    now_millis = millis();
    float fps = 1000 / (now_millis - pre_millis);
    Serial.print(", FPS: ");
    Serial.print(fps, 2);
    pre_millis = now_millis;
    
    Serial.println("");

#ifdef USE_SDCARD
    theSD.remove(filepath);
    File myFile = theSD.open(filepath, FILE_WRITE);
#else
    eMMC.remove(filepath);
    File myFile = eMMC.open(filepath, FILE_WRITE);
#endif

    myFile.write(img.getImgBuff(), img.getImgSize());
    myFile.close();
  }
  else
  {
    Serial.print("Failed to get video stream image\n");
  }
}

/**
   @brief Initialize camera
*/
void setup()
{
  /* Open serial communications and wait for port to open */

  Serial.begin(BAUDRATE);
  while (!Serial)
  {
    ; /* wait for serial port to connect. Needed for native USB port only */
  }
  
#ifdef USE_SDCARD
  /* Initialize SD */
  while (!theSD.begin())
  {
    /* wait until SD card is mounted. */
    Serial.println("Insert SD card.");
  }
#else
    /* Initialize eMMC */
    eMMC.begin(EMMC_POWER_PIN);
#endif

  Serial.println("Press any key to start.");
  while (!Serial.available());
}

/**
   @brief No procedure in shooting JPEG 
   sequence. At last, finalize Camera library.
*/

bool test(CAM_VIDEO_FPS fps, int h, int v, int divisor)
{
    CamErr err;
    /* Shooting JPEG sequence without frame's gap requires multiple buffers. */
    Serial.println("Stop streaming");
    theCamera.startStreaming(false);
    theCamera.end();
    frame_count = 0; /*For prevention of repeating end process */
    Serial.println("Prepare camera");
    err = theCamera.begin(2,
                          fps,
                          h,
                          v,
                          CAM_IMAGE_PIX_FMT_JPG,
                          divisor);
    if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
      return false;
    }
  
    /* Start video stream.
       If received video stream data from camera device,
        camera library call CamCB.
    */
    Serial.println("Start streaming");
    err = theCamera.startStreaming(true, CamCB);
    if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
      return false;
    }
    while (is_test_over != true) {
    sleep(1);
  }
  is_test_over = false;
  return true;
}

void loop()
{
  for (int i = 0; i < RES_LIST_SIZE; i++) {
    for (int j = 0; j < FPS_LIST_SIZE; j++) {
      printf("test start with res_list:[%d,%d], fps:%.1f\n", res_list[i][0], res_list[i][1], fps_print[j]);
      int divisor;

      //CAM_IMGSIZE_FULLHD RES以上、かつFPSが15以下の場合、メモリが不足のため、divisorを調整する
      if (i > 4 && i < 7 && j < 4) {
        divisor = 14;
      }
      else if (i >= 7 && j < 4) {
        divisor = 28;
      }
      else {
        divisor = 7;
      }
      bool ret = test(fps_list[j], res_list[i][0], res_list[i][1], divisor);      
      if (ret == false) {
        continue;
       }
    }
  }
  printf("Test is over!");
  exit(0);
}
