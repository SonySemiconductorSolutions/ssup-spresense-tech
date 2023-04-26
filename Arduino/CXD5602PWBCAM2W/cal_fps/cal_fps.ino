/*
 *  hfr_jpg.ino - Shooting 5 seconds JPEG sequence with QVGA 120FPS
 *  Copyright 2019 Sony Semiconductor Solutions Corporation
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 *
 *  This is a test app for the camera library.
 *  This library can only be used on the Spresense with the FCBGA chip package.
 */

#include <SDHCI.h>

/* for sprintf */
#include <stdio.h> 

#include <Camera.h>
#include <RTC.h>
#include "led.h"

#define BAUDRATE                (115200)
#define FRAMES_NUM              (200) /* 120FPS * 5 seconds */
#define FILENAME_LEN            (16)

SDClass  theSD;
int frame_count = 0;
RtcTime time_now;
RtcTime time_end;

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
      led_update(frame_count++);

      if (frame_count > FRAMES_NUM)
        {
          return;
        }
     }
  else
    {
      Serial.print("Failed to get video stream image\n");
    }
}

/**
 * @brief Initialize camera
 */
void setup()
{
  CamErr err;

  /* Open serial communications and wait for port to open */

  Serial.begin(BAUDRATE);

  /* wait for serial port to connect. Needed for native USB port only */
  while (!Serial)
    {
      ; 
    }

  /* Shooting JPEG sequence without frame's gap requires multiple buffers. */

  Serial.println("Prepare camera");
  err = theCamera.begin(2,
                        CAM_VIDEO_FPS_30,
                        CAM_IMGSIZE_QVGA_H,
                        CAM_IMGSIZE_QVGA_V,
                        CAM_IMAGE_PIX_FMT_YUV422,
                        7);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  /* Start video stream.
   * If received video stream data from camera device,
   *  camera library call CamCB.
   */
  time_now = RTC.getTime();
  Serial.print("now time:");
  Serial.println(time_now.unixtime());
  err = theCamera.startStreaming(true, CamCB);
  if (err != CAM_ERR_SUCCESS)
    {
      printError(err);
    }

  led_init(FRAMES_NUM);
  Serial.println("Start streaming");
}

/**
 * @brief No procedure in shooting JPEG sequence. At last, finalize Camera library.
 */

void loop()
{
  if (frame_count > FRAMES_NUM)
    {
      Serial.println("Stop streaming");
      time_end = RTC.getTime();
      Serial.print("end time:");
      Serial.println(time_end.unixtime());
  
      Serial.print("diff time:");
      uint32_t diff = time_end.unixtime() - time_now.unixtime();
      Serial.println(diff);
      Serial.print("FPS:");
      Serial.println(FRAMES_NUM/diff);
      theCamera.startStreaming(false);
      theCamera.end();
      
      /*For prevention of repeating end process */
      frame_count = 0; 
    }
}
