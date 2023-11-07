/*****************************************************************************
  BM1422AGMV.ino

 Copyright (c) 2018 ROHM Co.,Ltd.

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:

 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
******************************************************************************/
#include <Wire.h>
#include "BM1422AGMV.h"

BM1422AGMV bm1422agmv(BM1422AGMV_DEVICE_ADDRESS_0F);

/* 環境磁場、センサーy軸正方向と磁北方向のなす角度 */ 
float px = 0.0;
float py = 0.0;
float angle = 0.0;

void setup() {
  byte rc;
  
  Serial.begin(115200);
  Serial.println("Rohm BM1422AGMV Magnet Sensor sample");
  Wire.begin();
  rc = bm1422agmv.init();
  if (rc != 0) {
    Serial.println(F("BM1422AGMV initialization failed"));
    Serial.flush();
  }

  /* メニュー表示、環境磁場を計測・入力する */ 
  display_menu();
}

void loop() {
  float magnet[3];

  /* 磁場の強さを測定する */ 
  get_magnetic_field_strength(magnet);

  /* xy平面の磁場データと環境磁場のデータでセンサーy軸正方向と磁北方向のなす角度を求める */ 
  angle = cal_angle_between_magnetic_north(magnet[0], magnet[1], px, py);

  /* 方位を表示する */ 
  get_direction(angle);

  /* 2秒間隔 */ 
  delay(2000);
}

/**
 * サンプル説明、メニュー表示
 */
void display_menu() {
  /* Display Menu */ 
  Serial.println();
  Serial.print("本サンプルはSPRESENSE-SENSOR-EVK-701に内蔵する地磁気センサーBM1422GMVを利用し、\n");
  Serial.print("地磁気センサーが測定したx軸とy軸方向の磁場の強さで簡易の方位を計測するサンプルです。\n");
  Serial.print("サンプルを実行するとき、必ずセンサーを水平的に放置した上で作業を行ってください。\n");
  Serial.println();
  Serial.println("サンプルの実行手順：");
  Serial.println("1. 環境磁場の計測または入力");
  Serial.println("2. 環境磁場の計測または入力後、センサーを水平的に回し、方位を計測します");
  Serial.println();
  Serial.println("環境磁場を計測しますか？");
  Serial.println("[y]: Yes");
  Serial.println("[n]: No");

  /* User Menu Selection */ 
  int input;
  String mag_env;

  while (true) {
    while (!Serial.available());
    input = Serial.read();
    while (Serial.available()) {
      Serial.read(); // Discard inputs except for 1st character
    }

    /* "y"を検出した場合 */ 
    if (input == 'y') {
      Serial.println();
      Serial.println("環境磁場を計測します。センサーを水平的に放置し、ゆっくり一周回してください。");
      Serial.println("中止したい場合は「stop」を入力してください。");

      /* 環境磁場を計測する */ 
      get_environment_magnetic_field_strength();
      break;
    } 

    /* "n"を検出した場合 */ 
    else if (input == 'n') {
      Serial.println();
      Serial.print("xy平面の環境磁場の強さを入力してください。");
      Serial.print("（フォーマット：XX.YY。X,Yは数字で、小数は入力しなくても可）\n");
      
      /* pxの入力を検知する */ 
      Serial.print("Px: ");
      while (!Serial.available());
      if(Serial.available()){
        mag_env = Serial.readString();  
        px = mag_env.toFloat();
      }
      Serial.print(px);
      Serial.print("[μT]\n");

      /* pyの入力を検知する */ 
      Serial.print("Py: ");
      while (!Serial.available());
      if(Serial.available()){
        mag_env = Serial.readString();  
        py = mag_env.toFloat();
      }
      Serial.print(py);
      Serial.print("[μT]\n\n");
      break;
    } else {
      if ((input != '\n') && (input != '\r'))
        Serial.write(input);
    }
  }
}

/**
 * 測定した磁場のデータを取得する
 * @param magnet[3] - 3軸の磁場の強さを格納する配列
 */
void get_magnetic_field_strength(float magnet[3]) {
  byte rc;
  float mag[3];
  rc = bm1422agmv.get_val(mag);

  if (rc == 0) {
    for (int i = 0; i < 3; i++){
      magnet[i] = mag[i];
    }
  }
}

/**
 * 環境磁場の強さを計測する
 */
void get_environment_magnetic_field_strength() {
  float maxX = 0.0;
  float maxY = 0.0;
  float minX = 0.0;
  float minY = 0.0;
  float raw_mag[3];
  bool stopFlg = false;
  String command;

  /* maxX, minX, maxY, minYの初期値を与える */ 
  get_magnetic_field_strength(raw_mag);
  maxX = raw_mag[0];
  minX = raw_mag[0];
  maxY = raw_mag[1];
  minY = raw_mag[1];

  /* シリアルポートから"stop"を検出しない限り、繰り返す */ 
  while(!stopFlg){
    if(Serial.available()){
      command = Serial.readString();  
    }
    if(command == "stop") {
      stopFlg = true;
      break;
    } else {
      get_magnetic_field_strength(raw_mag);
      delay(500);
    }

    /* maxX, minX, maxY, minYを随時更新する */ 
    if (raw_mag[0] > maxX){ maxX = raw_mag[0]; }
    if (raw_mag[0] < minX){ minX = raw_mag[0]; }
    if (raw_mag[1] > maxY){ maxY = raw_mag[1]; }
    if (raw_mag[1] < minY){ minY = raw_mag[1]; }
  }

  /* 環境磁場(px, py)を求める */ 
  px = (maxX + minX) / 2;
  py = (maxY + minY) / 2;
  
  Serial.println();
  Serial.println("環境磁場の計測を停止しました。");
  Serial.print("Px: ");
  Serial.print(px);
  Serial.print("[μT], Py: ");
  Serial.print(py);
  Serial.print("[μT]\n");
  Serial.println();
}

/**
 * センサーy軸正方向と磁北方向のなす角度を求める
 * @param magX - センサーが測定したX軸方向の磁場の強さ
 * @param magY - センサーが測定したY軸方向の磁場の強さ
 * @param mag_px - 計測・入力したX軸方向の環境磁場の強さ
 * @param mag_py - 計測・入力したX軸方向の環境磁場の強さ
 * @return cal_angle - 算出されたセンサーy軸正方向と磁北方向のなす角度
 */
float cal_angle_between_magnetic_north(float magX, float magY, float mag_px, float mag_py){
  /** 
   * debug用
   * float cal_angle = atan2(magX + 36.98, magY + 35.85) * 180.0 / PI;
   * この計算式については資料を参考してください
   * 結果はdegree形式で出力する
   */
  float cal_angle = atan2(magX - px, magY - py) * 180.0 / PI;

  return cal_angle;
}

/**
 * 求めた角度で16方位に当てはめ、その角度と該当方位を文字で表示する
 * @param raw_angle - 角度
 */
void get_direction(float raw_angle){
  float interval = 360 / 32;

  Serial.print("角度: ");
  Serial.print(raw_angle);
  Serial.print("°,\t方位: ");
  
  if(-1 * interval < raw_angle && raw_angle <= 1 * interval) { Serial.println("北"); }
  else if(1 * interval < raw_angle && raw_angle <= 3 * interval) { Serial.println("北北西"); }
  else if(3 * interval < raw_angle && raw_angle <= 5 * interval) { Serial.println("北西"); }
  else if(5 * interval < raw_angle && raw_angle <= 7 * interval) { Serial.println("西北西"); }
  else if(7 * interval < raw_angle && raw_angle <= 9 * interval) { Serial.println("西"); }
  else if(9 * interval < raw_angle && raw_angle <= 11 * interval) { Serial.println("西南西"); }
  else if(11 * interval < raw_angle && raw_angle <= 13 * interval) { Serial.println("南西"); }
  else if(13 * interval < raw_angle && raw_angle <= 15 * interval) { Serial.println("南南西"); }
  else if(-15 * interval < raw_angle && raw_angle <= -13 * interval) { Serial.println("南南東"); }
  else if(-13 * interval < raw_angle && raw_angle <= -11 * interval) { Serial.println("南東"); }
  else if(-11 * interval < raw_angle && raw_angle <= -9 * interval) { Serial.println("東南東"); }
  else if(-9 * interval < raw_angle && raw_angle <= -7 * interval) { Serial.println("東"); }
  else if(-7 * interval < raw_angle && raw_angle <= -5 * interval) { Serial.println("東北東"); }
  else if(-5 * interval < raw_angle && raw_angle <= -3 * interval) { Serial.println("北東"); }
  else if(-3 * interval < raw_angle && raw_angle <= -1 * interval) { Serial.println("北北東"); }
  else { Serial.println("南"); }
}
