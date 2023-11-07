/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 *
 * Copyright 2023 Sony Semiconductor Solutions Corporation.
 */
 
#include <SDHCI.h>
#include <MP.h>
#include <MPMutex.h>
#include <stdio.h>
#include <Camera.h>
#include "sensor_app.hpp"

#define  GET_BMI270_DATA
#define  TOTAL_PICTURE_COUNT     (1000)
#define  BAUDRATE  115200
#define  MSGLEN      6

int take_picture_count = 0;
sensor_appClass *sensor_app;

#ifdef GET_BMI270_DATA
/* Multi-core parameters */
const int CONECTIVITY_CORE = 1;
struct MyPacket {
  volatile int status; /* 0:ready, 1:busy */
  float message[MSGLEN];
};
#endif

/**
 * Print error message
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
 * Callback from Camera library when video frame is captured.
 */

void CamCB(CamImage img)
{

  /* Check the img instance is available or not. */

  if (img.isAvailable())
    {

      /* If you want RGB565 data, convert image data format to RGB565 */

      img.convertPixFormat(CAM_IMAGE_PIX_FMT_RGB565);

      /* You can use image data directly by using getImgSize() and getImgBuff().
       * for displaying image to a display, etc. */

      /**
       * Serial.print("Image data size = ");
       * Serial.print(img.getImgSize(), DEC);
       * Serial.print(" , ");
       * Serial.print("buff addr = ");
       * Serial.print((unsigned long)img.getImgBuff(), HEX);
       * Serial.println("");
      */
    }
  else
    {
      Serial.println("Failed to get video stream image");
    }
}

void preCamera()
{
  CamErr err;
  Serial.begin(BAUDRATE);
  while (!Serial)
    {
      ; 
    }

  sensor_app = sensor_appClass::getInstance();
  sensor_app->initialize(1);

  /* begin() without parameters means that
   * number of buffers = 1, 30FPS, QVGA, YUV 4:2:2 format */
  Serial.println("Prepare camera");
  err = theCamera.begin();
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Start video stream.
   * If received video stream data from camera device,
   *  camera library call CamCB.
   */

  Serial.println("Start streaming");
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Auto white balance configuration */

  Serial.println("Set Auto white balance parameter");
  err = theCamera.setAutoWhiteBalanceMode(CAM_WHITE_BALANCE_DAYLIGHT);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
 
  /* Set parameters about still picture.
   * In the following case, QUADVGA and JPEG.
   */

  Serial.println("Set still picture format");
  err = theCamera.setStillPictureImageFormat(
     CAM_IMGSIZE_QUADVGA_H,
     CAM_IMGSIZE_QUADVGA_V,
     CAM_IMAGE_PIX_FMT_JPG);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }
}

void setup() {
  RTC.begin();
  RtcTime compiledDateTime(__DATE__, __TIME__);
  RTC.setTime(compiledDateTime);
  preCamera();
	/* initialize digital pin LED_BUILTIN as an output. */
	pinMode(LED0, OUTPUT);
	digitalWrite(LED0, LOW);   // turn the LED off (LOW is the voltage level)
#ifdef GET_BMI270_DATA
  /* Launch SubCore */
  MP.begin(CONECTIVITY_CORE);
#endif
}

// the loop function runs over and over again forever
void loop() {
  if (take_picture_count < TOTAL_PICTURE_COUNT)
    {
      Serial.println("call takePicture()");
      CamImage img = theCamera.takePicture();

      if (img.isAvailable())
        {
          /* save picture with file name: */
          sensor_app->save_data(&img);
        }
      else
        {
          Serial.println("Failed to take picture");
        }
    }
  else if (take_picture_count == TOTAL_PICTURE_COUNT)
    {
      Serial.println("End.");
      theCamera.end();
    }

  take_picture_count++;
  
#ifdef GET_BMI270_DATA
  int      ret;
  int      subid;
  int8_t   msgid;
  MyPacket *packet;
  /* Receive message from SubCore */
  for (subid = 1; subid <= 1; subid++) {
    ret = MP.Recv(&msgid, &packet, subid);
    if (ret > 0) {
      sensor_app->save_data(packet->message[0], packet->message[1], packet->message[2], packet->message[3], packet->message[4], packet->message[5]);
      for (int i = 0; i < MSGLEN; i++) {
        printf("%.2f,", packet->message[i]);
      }
      printf("\n");
      
      /* status -> ready */
      packet->status = 0;
    }
  }
#endif
  delay(sensor_app->get_interval() * 1000);
}
