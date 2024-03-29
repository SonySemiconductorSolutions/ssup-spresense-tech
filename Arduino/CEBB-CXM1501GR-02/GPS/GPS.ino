/*
 *  eltres_sample.ino - 5回ペイロードを送信して停止するサンプルプログラム
 *  Copyright 2021 CREATIVE JAPAN, LTD.
 */
/**
 * サンプルプログラムは、下記のようにLEDを用いて状態を表現します。
 *   LED0：[プログラム状態] 起動中：点灯、正常・異常終了：消灯
 *   LED1：[GNSS電波状態] 受信中：点灯、未受信：消灯
 *   LED2：[ELTRES状態] 送信中：点灯、待機中：消灯
 *   LED3：[エラー状態] GNSS受信エラー：点滅、ELTRESエラー：点灯、エラー無し：消灯
 * サンプルプログラムは、以下のように動作します。
 *   ELTRES送信を5回実施して、正常終了状態でループします。
 *   ペイロードには送信回数を格納します。
 *   ※GNSS受信エラー発生時はGNSS電波受信するまで待ち続けます。（LED0は点灯、LED3は点滅）
 *   ※ELTRESエラー発生時は異常終了状態でループします。（LED0は消灯、LED3は点灯）
 */
#include <EltresAddonBoard.h>
#include "sensor_app.h"

// PIN定義：LED(プログラム状態)
#define LED_RUN PIN_LED0
// PIN定義：LED(GNSS電波状態)
#define LED_GNSS PIN_LED1
// PIN定義：LED(ELTRES状態)
#define LED_SND PIN_LED2
// PIN定義：LED(エラー状態)
#define LED_ERR PIN_LED3

// プログラム内部状態：初期状態
#define PROGRAM_STS_INIT      (0)
// プログラム内部状態：起動中
#define PROGRAM_STS_RUNNING   (1)
// プログラム内部状態：終了処理中
#define PROGRAM_STS_STOPPING  (2)
// プログラム内部状態：終了
#define PROGRAM_STS_STOPPED   (3)

// プログラム内部状態
int program_sts = PROGRAM_STS_INIT;
// GNSS電波受信タイムアウト（GNSS受信エラー）発生フラグ
bool gnss_recevie_timeout = false;
// 点滅処理で最後に変更した時間
uint64_t last_change_blink_time = 0;
// イベント通知での送信直前通知（5秒前）受信フラグ
bool event_send_ready = false;
// イベント通知でのアイドル状態受信フラグ
bool event_idle = false;
// 送信回数
int send_count = 0;
const int SEND_COUNT = 4;
// ペイロードデータ格納場所
uint8_t payload[16];

Sensor_appClass *sensor_app;
static eltres_board_gga_info g_gga_info = {0};
static uint32_t gnss_time;
/**
 * @brief イベント通知受信コールバック
 */
void eltres_event_cb(eltres_board_event event) {
  switch (event) {
  case ELTRES_BOARD_EVT_GNSS_TMOUT:
    // GNSS電波受信タイムアウト
    Serial.println("gnss wait timeout error.");
    gnss_recevie_timeout = true;
    break;
  case ELTRES_BOARD_EVT_IDLE:
    // アイドル状態
    Serial.println("waiting sending timings.");
    digitalWrite(LED_SND, LOW);
    event_idle = true;
    break;
  case ELTRES_BOARD_EVT_SEND_READY:
    // 送信直前通知（5秒前）
    Serial.println("Shortly before sending, so setup payload if need.");
    event_send_ready = true;
    break;
  case ELTRES_BOARD_EVT_SENDING:
    // 送信開始
    Serial.println("start sending.");
    digitalWrite(LED_SND, HIGH);
    break;
  case ELTRES_BOARD_EVT_GNSS_UNRECEIVE:
    // GNSS電波未受信
    Serial.println("gnss wave has not been received.");
    digitalWrite(LED_GNSS, LOW);
    break;
  case ELTRES_BOARD_EVT_GNSS_RECEIVE:
    // GNSS電波受信
    Serial.println("gnss wave has been received.");
    digitalWrite(LED_GNSS, HIGH);
    gnss_recevie_timeout = false;
    break;
  case ELTRES_BOARD_EVT_FAULT:
    // 内部エラー発生
    Serial.println("internal error.");
    break;
  }
}

/**
 * @brief GGA情報受信コールバック
 */
void gga_event_cb(eltres_board_gga_info *gga_info) {
  Serial.print("[gga]");
  if (gga_info->m_pos_status) {
    // 測位状態
    // GGA情報をシリアルモニタへ出力
    g_gga_info = *gga_info;
    Serial.print("utc: ");
    Serial.println((const char *)gga_info->m_utc);
    Serial.print("lat: ");
    Serial.print((const char *)gga_info->m_n_s);
    Serial.print((const char *)gga_info->m_lat);
    Serial.print(", lon: ");
    Serial.print((const char *)gga_info->m_e_w);
    Serial.println((const char *)gga_info->m_lon);
    Serial.print("pos_status: ");
    Serial.print(gga_info->m_pos_status);
    Serial.print(", sat_used: ");
    Serial.println(gga_info->m_sat_used);
    Serial.print("hdop: ");
    Serial.print(gga_info->m_hdop);
    Serial.print(", height: ");
    Serial.print(gga_info->m_height);
    Serial.print(" m, geoid: ");
    Serial.print(gga_info->m_geoid);
    Serial.println(" m");
  } else {
    // 非測位状態
    // "invalid data"をシリアルモニタへ出力
    Serial.println("invalid data.");
  }
}

void create_payload() {
  memset(payload, 0, sizeof(payload));
         
  uint8_t *ptr = (uint8_t *)payload;
 
  // GNSS時刻(epoch秒)の取得
  gnss_time = 0;
  EltresAddonBoard.get_gnss_time(&gnss_time);
  Serial.print("gnss time: ");
  Serial.print(gnss_time);
  Serial.println(" sec");
  // GPS設定
  sensor_app->set_lat_lon(&g_gga_info, gnss_time, ptr);  
  
  Serial.print("sample payload:");
  for ( int i = 0; i < 16; i++ ) {
    Serial.print(payload[i], HEX);
    Serial.print(",");
  }
  Serial.println("");
  return;
}

/**
 * @brief setup()関数
 */
void setup() {
  // シリアルモニタ出力設定
  Serial.begin(115200);

  // LED初期設定
  pinMode(LED_RUN, OUTPUT);
  digitalWrite(LED_RUN, HIGH);
  pinMode(LED_GNSS, OUTPUT);
  digitalWrite(LED_GNSS, LOW);
  pinMode(LED_SND, OUTPUT);
  digitalWrite(LED_SND, LOW);
  pinMode(LED_ERR, OUTPUT);
  digitalWrite(LED_ERR, LOW);

  sensor_app = Sensor_appClass::getInstance();

  // ELTRES起動処理
  eltres_board_result ret = EltresAddonBoard.begin(ELTRES_BOARD_SEND_MODE_1MIN,eltres_event_cb, gga_event_cb);
  if (ret != ELTRES_BOARD_RESULT_OK) {
    // ELTRESエラー発生
    digitalWrite(LED_RUN, LOW);
    digitalWrite(LED_ERR, HIGH);
    program_sts = PROGRAM_STS_STOPPED;
    Serial.print("cannot start eltres board (");
    Serial.print(ret);
    Serial.println(").");
  } else {
    // 正常
    program_sts = PROGRAM_STS_RUNNING;
  }
}

/**
 * @brief loop()関数
 */
void loop() {
  int32_t remaining;

  switch (program_sts) {
    case PROGRAM_STS_RUNNING:
      // プログラム内部状態：起動中
      if (gnss_recevie_timeout) {
        // GNSS電波受信タイムアウト（GNSS受信エラー）時の点滅処理
        uint64_t now_time = millis();
        if ((now_time - last_change_blink_time) >= 1000) {
          last_change_blink_time = now_time;
          bool set_value = digitalRead(LED_ERR);
          bool next_value = (set_value == LOW) ? HIGH : LOW;
          digitalWrite(LED_ERR, next_value);
        }
      } else {
        digitalWrite(LED_ERR, LOW);
      }

      if (event_send_ready) {
        // 送信直前通知時の処理
        event_send_ready = false;
        send_count += 1;
        // 送信ペイロードの作成
        create_payload();
        // 送信ペイロードの設定
        EltresAddonBoard.set_payload(payload);
      }
      
      if (event_idle) {
        // 送信完了時の処理
        event_idle = false;
        if (send_count > SEND_COUNT) {
           // 規定回数
           program_sts = PROGRAM_STS_STOPPING;
           Serial.println("stop sending.");
           break;
        }    
        // 次送信までの残り時間の取得
        EltresAddonBoard.get_remaing_time(&remaining);
        Serial.print("remaining time: ");
        Serial.print(remaining);
        Serial.println(" sec");
      }
      break;
      
    case PROGRAM_STS_STOPPING:
      // プログラム内部状態：終了処理中
      // ELTRES停止処理(注意：この処理を行わないとELTRESが自動送信し続ける)
      EltresAddonBoard.end();
      digitalWrite(LED_RUN, LOW);
      digitalWrite(LED_GNSS, LOW);
      program_sts = PROGRAM_STS_STOPPED;
      break;
      
    case PROGRAM_STS_STOPPED:
      // プログラム内部状態：終了
      break;
  }
  // 次のループ処理まで100ミリ秒待機
  delay(100);
}
