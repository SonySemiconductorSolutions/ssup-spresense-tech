/*
 *  recorder_wav.ino - Recorder example application for WAV(PCM)
 *  Copyright 2018 Sony Semiconductor Solutions Corporation
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
 */

#include <Audio.h>
#include <eMMC.h>

#include <arch/board/board.h>

#define RECORD_FILE_NAME "Sound"
#define FILE_PATH_LEN 128
// eMMCの電源をコントロールする端子
#define POWER_1V8_ENABLE  26     /* JP1 1: CTS(D27)  3: I2S_BCK(D26) */

AudioClass *theAudio;

File myFile;

bool ErrEnd = false;

static uint16_t file_count = 0;

/**
 * @brief Audio attention callback
 *
 * When audio internal error occurc, this function will be called back.
 */

static void audio_attention_cb(const ErrorAttentionParam *atprm)
{
  puts("Attention!");
  
  if (atprm->error_code >= AS_ATTENTION_CODE_WARNING)
    {
      ErrEnd = true;
   }
}

/**
 * @brief create record file
 *
 * create the record file to emmc card.
 */
 
static void create_filename(File *record_file, uint16_t count)
{
  char file_path[FILE_PATH_LEN];

  snprintf(file_path, sizeof(file_path), "%s%02d.wav",RECORD_FILE_NAME, count);
  if (eMMC.exists(file_path))
    {
      printf("Remove existing file [%s].\n", file_path);
      eMMC.remove(file_path);
    }

  *record_file = eMMC.open(file_path, FILE_WRITE);
  /* Verify file open */
  if (!myFile)
    {
      printf("File open error\n");
      exit(1);
    }

  printf("Open! [%s]\n", file_path);
}
/**
 * @brief Setup recording of mp3 stream to file
 *
 * Select input device as microphone <br>
 * Initialize filetype to stereo wav with 48 Kb/s sampling rate <br>
 * Open RECORD_FILE_NAME file in write mode
 */

/* Sampling rate
 * Set 16000 or 48000
 */

static const uint32_t recoding_sampling_rate = 192000;

/* Number of input channels
 * Set either 1, 2, or 4.
 */

static const uint8_t  recoding_cannel_number = 1;

/* Audio bit depth
 * Set 16 or 24
 */

static const uint8_t  recoding_bit_length = 16;

/* Recording time[second] */

static const uint32_t recoding_time = 60*60;

/* Recording devide time[second] */

static const int32_t devide_time = 10*60;

/* Bytes per second */

static const int32_t recoding_byte_per_second = recoding_sampling_rate *
                                                recoding_cannel_number *
                                                recoding_bit_length / 8;

/* Total recording size */

static const int32_t recoding_size = recoding_byte_per_second * recoding_time;

/* Devide recording size */

static const uint32_t devide_size = recoding_byte_per_second * devide_time;

/**
 * divide record file
 *
 * devide record file with divide_second and read frames to the file.
 */
 
err_t divide_file(uint32_t total_second, uint32_t divide_second, uint32_t recording_size_now) 
{
  err_t err;
  
  /* devide size condition */
  /* Read frames to record in another file */
  if (recording_size_now > devide_size * file_count)
    {
      theAudio->writeWavHeader(myFile);
      myFile.close();
      create_filename(&myFile, file_count++);
    }
  /* Read frames to record in file */
  err = theAudio->readFrames(myFile);
  return err;
}

void setup()
{
  /* 1.8V Enable */
  Serial.println("1.8V Eanble");
  pinMode(POWER_1V8_ENABLE, OUTPUT);
  digitalWrite(POWER_1V8_ENABLE, HIGH);
  delay(100);                                /* delay 無しだとエラーとなる */

  /* Initialize eMMC */
  while (!eMMC.begin()) {
    /* wait until emmc card is mounted. */
    Serial.println("Insert eMMC.");
    sleep(1);
  }
  
  theAudio = AudioClass::getInstance();

  theAudio->begin(audio_attention_cb);

  puts("initialization Audio Library");
  /* Set clock mode to Hi-Res */
  theAudio->setRenderingClockMode(AS_CLKMODE_HIRES);
  /* Select input device as microphone */
  theAudio->setRecorderMode(AS_SETRECDR_STS_INPUTDEVICE_MIC, 0, 500*1024);

  /* Search for WAVDEC codec in "/mnt/emmc/BIN" directory */
  theAudio->initRecorder(AS_CODECTYPE_WAV,
                         "/mnt/emmc/BIN",
                         recoding_sampling_rate,
                         recoding_bit_length,
                         recoding_cannel_number);
  puts("Init Recorder!");

  create_filename(&myFile, file_count++);

  theAudio->writeWavHeader(myFile);
  puts("Write Header!");

  theAudio->startRecorder();
  puts("Recording Start!");

}

void loop() 
{
  err_t err;
  uint32_t recording_size_now = 0;
  
  recording_size_now = theAudio->getRecordingSize();
  
  /* recording end condition */
  if (recording_size_now > recoding_size)
    {
      theAudio->stopRecorder();
      sleep(1);
      err = theAudio->readFrames(myFile);

      goto exitRecording;
    }


  /* Read frames to record in file */
  err = divide_file(recoding_time, devide_time, recording_size_now);

  if (err != AUDIOLIB_ECODE_OK)
    {
      printf("File End! =%d\n",err);
      theAudio->stopRecorder();
      goto exitRecording;
    }

  if (ErrEnd)
    {
      printf("Error End\n");
      theAudio->stopRecorder();
      goto exitRecording;
    }

  /* This sleep is adjusted by the time to write the audio stream file.
   * Please adjust in according with the processing contents
   * being processed at the same time by Application.
   *
   * The usleep() function suspends execution of the calling thread for usec
   * microseconds. But the timer resolution depends on the OS system tick time
   * which is 10 milliseconds (10,000 microseconds) by default. Therefore,
   * it will sleep for a longer time than the time requested here.
   */

//  usleep(10000);

  return;

exitRecording:

  theAudio->closeOutputFile(myFile);
  myFile.close();
  
  theAudio->setReadyMode();
  theAudio->end();
  
  puts("End Recording");
  exit(1);

}
