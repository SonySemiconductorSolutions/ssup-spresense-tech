#include "Arduino.h"
#include "SPI.h"
#include "Wire.h"

#include "bmi270.h"

#define BMI270_CS   SS
#define BMI270_INT1 22

#ifndef ARDUINO_ARCH_SPRESENSE
#error "Board selection is wrong!!"
#endif
#ifdef SUBCORE
#error "Core selection is wrong!!"
#endif

#include <ScoreSrc.h>
#include <SFZSink.h>

// SFZ音源ファイルを指定します。
SFZSink sink("SawLpf.sfz");

// 演奏する楽譜ファイルを指定します。
// フォルダを指定すると、そのフォルダの中にある楽譜ファイルを演奏します。
ScoreSrc inst("SCORE/aogeba.mid", sink);

/*! Earth's gravity in m/s^2 */
#define GRAVITY_EARTH  (9.80665f)

/* Callback function prototypes for the BMI270 Sensor API */
int8_t bmi2_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t bmi2_spi_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len);
int8_t bmi2_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len);
int8_t bmi2_i2c_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len);
void bmi2_delay_us(uint32_t period);

/* Other functions */
void panic_led_trap(void);
int8_t configure_sensor(struct bmi2_dev *dev);
void bmi2_intr1_callback(void);
void print_rslt(int8_t rslt);

/* Static variables */
static struct bmi2_dev bmi2;
static volatile bool bmi2_intr_recvd = false;
static volatile uint32_t last_time_us = 0;

/*Const variables*/
//range of BMI270
const int bmi2_acc_range = 4;
const int bmi2_gyro_range = 2000;

/*Global variables */
bool g_change_flag = false;
int g_time_before;          //現在の経過時間保存用

typedef enum CH_TRACK{
  PREV, 
  NEXT, 
  UNKNOUN
} ch_track;

/*music class*/
class Music
{
public:
  const int selectable_min = 2;   //最小トラック番号
  const int selectable_max = 7;   //最大トラック番号
  const int conductor = 0;        //コンダクタートラック
  const int time_wait = 100000;   //トラック変更操作を受け付ける時間の間隔(μs)
  int count = selectable_min;
  int dps_z;
  ch_track m_ch_track = UNKNOUN;
  
  void m_start(void);
  void m_judge_change_shake(void);
  void m_change_track(void);
  void m_restart_music(void);
  void m_led_express_track(void);
} music;

//音楽の再生
void Music::m_start(void)
{
  inst.setParam(ScoreFilter::PARAMID_TRACK_MASK, 0x00000000); // 再生対象トラックをクリアする
  inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, conductor);        // 第1トラック(コンダクタートラック)を再生対象に追加する
  inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, selectable_min);        // 第3トラック(再生したいトラック)を再生対象に追加する
  inst.begin();
}

//振った時のトラック変更判定（1トラックずつ）
void Music::m_judge_change_shake(void) {
  m_ch_track = UNKNOUN;
  
  //スマホ持ちで振った場合、強弱の変化がgyro_zに一番現れたため、gyro_zを閾値に採用
  //データを見る限りだと|gyro_z|が600を閾値にしたほうがいいかも
  if(600.0 < dps_z){
    m_ch_track = PREV;
  }
  else if(dps_z < -600.0){
    m_ch_track = NEXT;
  }
}

//トラック変更操作
void Music::m_change_track(void)
{
  if (m_ch_track == NEXT) {
    if (count == selectable_max) {
      count = selectable_min;
    }
    else {
      count = count + 1;
    }
  }
  else  if (m_ch_track == PREV) {
    if (count == selectable_min) {
      count = selectable_max;
    }
    else {
      count = count - 1;
    }
  }
}

//曲の停止→トラックの変更→曲の再生
void Music::m_restart_music(void)
{
  inst.sendStop();        //曲の一時停止
  inst.update();
  inst.setParam(ScoreFilter::PARAMID_TRACK_MASK, 0x00000000); // 再生対象トラックをクリアする
  inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, conductor);        // 第1トラック(コンダクタートラック)を再生対象に追加する
  inst.setParam(ScoreFilter::PARAMID_ENABLE_TRACK, count);        // 変更後のトラックを再生対象に追加する
  inst.setScoreIndex(0);    //曲をリロードして再生位置をリセットする
  inst.update();
  inst.sendContinue();      //曲の再開
  inst.update();
}

//再生トラック番号をLEDで表現する機能
void Music::m_led_express_track(void){
  digitalWrite(LED0, ((0x01&count) != 0? HIGH: LOW));
  digitalWrite(LED1, (((0x01<<1)&count) != 0? HIGH: LOW));
  digitalWrite(LED2, (((0x01<<2)&count) != 0? HIGH: LOW));
  digitalWrite(LED3, (((0x01<<3)&count) != 0? HIGH: LOW));
}

/*function in setup*/
void serial_init(void);
void pinmode_init(void);
void bmi2_set(void);

/*function in loop*/
void led_express_track(void);
void track_change_shake(int);

void setup(void)
{
  //Serialの初期化
  serial_init();

  //音楽再生機能
  music.m_start();

  while (!Serial.available()); // Wait for an input to proceed
  
  //pinModeの初期化
  pinmode_init();

  //BMI270の設定
  bmi2_set();

}

//Serialの初期化
void serial_init(void){
  Serial.begin(115200);
}

//pinModeの初期化
void pinmode_init(void){
  //pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED0, OUTPUT);
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
}

//BMI270の設定
void bmi2_set(void){
  int8_t l_rslt;
  
  Wire.begin();
  bmi2.dev_id = BMI2_I2C_SEC_ADDR;
  bmi2.read = bmi2_i2c_read;
  bmi2.write = bmi2_i2c_write;
  bmi2.delay_us = bmi2_delay_us;
  bmi2.intf = BMI2_I2C_INTF;
  bmi2.read_write_len = 30; // Limitation of the Wire library
  bmi2.config_file_ptr = NULL; // Use the default BMI270 config file

  l_rslt = bmi270_init(&bmi2);
  print_rslt(l_rslt);

  attachInterrupt(BMI270_INT1, bmi2_intr1_callback, RISING);

  l_rslt = configure_sensor(&bmi2);
  print_rslt(l_rslt);
}

/*!
 * @brief This function converts lsb to meter per second squared for 16 bit accelerometer at
 * range 2G, 4G, 8G or 16G.
 */
static float lsb_to_mps2(int16_t val, float range, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (GRAVITY_EARTH * val * range) / half_scale;
}

/*!
 * @brief This function converts lsb to degree per second for 16 bit gyro at
 * range 125, 250, 500, 1000 or 2000dps.
 */
static float lsb_to_dps(int16_t val, float dps, uint8_t bit_width)
{
    float half_scale = ((float)(1 << bit_width) / 2.0f);

    return (dps / ((half_scale))) * (val);
}


void loop(void)
{
  inst.update();
  
  if (bmi2_intr_recvd)
  {
    bmi2_intr_recvd = false;
    digitalWrite(LED_BUILTIN, LOW); // Flash the LED to show activity

    struct bmi2_sens_data sensor_data;
    int8_t rslt = bmi2_get_sensor_data(&sensor_data, &bmi2);
    print_rslt(rslt);

    music.dps_z = lsb_to_dps(sensor_data.gyr.z, bmi2_gyro_range, bmi2.resolution);

    //前回のトラック変更から1秒たったら処理する
    if (g_change_flag == false){

      //強く振った時の加速度によるトラック変更判定
      music.m_judge_change_shake();
      
      //経過秒数によるトラック変更制限
      if (music.m_ch_track != UNKNOUN) {
        
        //デバッグ
        Serial.println();
        Serial.print("Track Change");
        Serial.println();

        //フラグを立てて、任意の秒数が経過するまでトラック変更しないようにする
        g_change_flag = true;

        //現在の経過時間を保存
        g_time_before = last_time_us;
        
        //トラック変更部分
        music.m_change_track();

        //曲の変更
        music.m_restart_music();
        
      }
    }

     //トラック変更してからの秒数の計測⇒1秒経過でトラック変更可能にする
     if (music.time_wait < last_time_us - g_time_before) {
      g_change_flag = false;
     }
      
     //デバッグ
     Serial.println();
     Serial.print(music.count);
     Serial.println();

     //現在のトラック番号が分かるようにLEDで表現する
     music.m_led_express_track();

     inst.update();
     
    }

    inst.update();
}

int8_t configure_sensor(struct bmi2_dev *dev)
{
  int8_t rslt;
  uint8_t sens_list[2] = { BMI2_ACCEL, BMI2_GYRO };

  struct bmi2_int_pin_config int_pin_cfg;
  int_pin_cfg.pin_type = BMI2_INT1;
  int_pin_cfg.int_latch = BMI2_INT_NON_LATCH;
  int_pin_cfg.pin_cfg[0].lvl = BMI2_INT_ACTIVE_HIGH;
  int_pin_cfg.pin_cfg[0].od = BMI2_INT_PUSH_PULL;
  int_pin_cfg.pin_cfg[0].output_en = BMI2_INT_OUTPUT_ENABLE;
  int_pin_cfg.pin_cfg[0].input_en = BMI2_INT_INPUT_DISABLE;

  struct bmi2_sens_config sens_cfg[2];
  sens_cfg[0].type = BMI2_ACCEL;
  sens_cfg[0].cfg.acc.bwp = BMI2_ACC_OSR2_AVG2;
  sens_cfg[0].cfg.acc.odr = BMI2_ACC_ODR_100HZ;
  sens_cfg[0].cfg.acc.filter_perf = BMI2_PERF_OPT_MODE,
  sens_cfg[0].cfg.acc.range = BMI2_ACC_RANGE_4G;

  sens_cfg[1].type = BMI2_GYRO;
  sens_cfg[1].cfg.gyr.filter_perf = BMI2_PERF_OPT_MODE;
  sens_cfg[1].cfg.gyr.bwp = BMI2_GYR_OSR2_MODE;
  sens_cfg[1].cfg.gyr.odr = BMI2_GYR_ODR_100HZ;
  sens_cfg[1].cfg.gyr.range = BMI2_GYR_RANGE_2000;
  sens_cfg[1].cfg.gyr.ois_range = BMI2_GYR_OIS_2000;

  rslt = bmi2_set_int_pin_config(&int_pin_cfg, dev);
  if (rslt != BMI2_OK)
    return rslt;

  rslt = bmi2_map_data_int(BMI2_DRDY_INT, BMI2_INT1, dev);
  if (rslt != BMI2_OK)
    return rslt;

  rslt = bmi2_set_sensor_config(sens_cfg, 2, dev);
  if (rslt != BMI2_OK)
    return rslt;

  rslt = bmi2_sensor_enable(sens_list, 2, dev);
  if (rslt != BMI2_OK)
    return rslt;

  return rslt;
}

int8_t bmi2_spi_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if ((data == NULL) || (len == 0))
    return -1;
  digitalWrite(dev_id, LOW);
  SPI.transfer(reg_addr);
  for (uint16_t i = 0; i < len; i++)
  {
    data[i] = SPI.transfer(0xff);
  }
  digitalWrite(dev_id, HIGH);
  return 0;
}

int8_t bmi2_spi_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len)
{
  if ((data == NULL) || (len == 0))
    return -1;
  digitalWrite(dev_id, LOW);
  SPI.transfer(reg_addr);
  for (uint16_t i = 0; i < len; i++)
    SPI.transfer(data[i]);
  digitalWrite(dev_id, HIGH);
  return 0;
}

int8_t bmi2_i2c_read(uint8_t dev_id, uint8_t reg_addr, uint8_t *data, uint16_t len)
{
  if ((data == NULL) || (len == 0) || (len > 32)) {
    return -1;
  }
  uint8_t bytes_received;

  Wire.beginTransmission(dev_id);
  Wire.write(reg_addr);
  if (Wire.endTransmission() == 0) {
    bytes_received = Wire.requestFrom(dev_id, len);
    // Optionally, throw an error if bytes_received != len
    for (uint16_t i = 0; i < bytes_received; i++)
    {
      data[i] = Wire.read();
    }
  } else {
    return -1;
  }

  return 0;
}

int8_t bmi2_i2c_write(uint8_t dev_id, uint8_t reg_addr, const uint8_t *data, uint16_t len)
{
    
  if ((data == NULL) || (len == 0) || (len > 32)) {
    return -1;
  }

  Wire.beginTransmission(dev_id);
  Wire.write(reg_addr);
  for (uint16_t i = 0; i < len; i++)
  {
    Wire.write(data[i]);
  }
  if (Wire.endTransmission() != 0) {
    return -1;
  }

  return 0;
}

void bmi2_delay_us(uint32_t period)
{
  delayMicroseconds(period);
}

void bmi2_intr1_callback(void)
{
  bmi2_intr_recvd = true;
  last_time_us = micros();
}

void panic_led_trap(void)
{
  while (1)
  {
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
  }
}

void print_rslt(int8_t rslt)
{
  switch (rslt)
  {
    case BMI2_OK: return; /* Do nothing */ break;
    case BMI2_E_NULL_PTR:
      Serial.println("Error [" + String(rslt) + "] : Null pointer");
      panic_led_trap();
      break;
    case BMI2_E_COM_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Communication failure");
      panic_led_trap();
      break;
    case BMI2_E_DEV_NOT_FOUND:
      Serial.println("Error [" + String(rslt) + "] : Device not found");
      panic_led_trap();
      break;
    case BMI2_E_OUT_OF_RANGE:
      Serial.println("Error [" + String(rslt) + "] : Out of range");
      panic_led_trap();
      break;
    case BMI2_E_ACC_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid accel configuration");
      panic_led_trap();
      break;
    case BMI2_E_GYRO_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid gyro configuration");
      panic_led_trap();
      break;
    case BMI2_E_ACC_GYR_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid accel/gyro configuration");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_SENSOR:
      Serial.println("Error [" + String(rslt) + "] : Invalid sensor");
      panic_led_trap();
      break;
    case BMI2_E_CONFIG_LOAD:
      Serial.println("Error [" + String(rslt) + "] : Configuration loading error");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_PAGE:
      Serial.println("Error [" + String(rslt) + "] : Invalid page ");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_FEAT_BIT:
      Serial.println("Error [" + String(rslt) + "] : Invalid feature bit");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_INT_PIN:
      Serial.println("Error [" + String(rslt) + "] : Invalid interrupt pin");
      panic_led_trap();
      break;
    case BMI2_E_SET_APS_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Setting advanced power mode failed");
      panic_led_trap();
      break;
    case BMI2_E_AUX_INVALID_CFG:
      Serial.println("Error [" + String(rslt) + "] : Invalid auxilliary configuration");
      panic_led_trap();
      break;
    case BMI2_E_AUX_BUSY:
      Serial.println("Error [" + String(rslt) + "] : Auxilliary busy");
      panic_led_trap();
      break;
    case BMI2_E_SELF_TEST_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Self test failed");
      panic_led_trap();
      break;
    case BMI2_E_REMAP_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Remapping error");
      panic_led_trap();
      break;
    case BMI2_E_GYR_USER_GAIN_UPD_FAIL:
      Serial.println("Error [" + String(rslt) + "] : Gyro user gain update failed");
      panic_led_trap();
      break;
    case BMI2_E_SELF_TEST_NOT_DONE:
      Serial.println("Error [" + String(rslt) + "] : Self test not done");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_INPUT:
      Serial.println("Error [" + String(rslt) + "] : Invalid input");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_STATUS:
      Serial.println("Error [" + String(rslt) + "] : Invalid status");
      panic_led_trap();
      break;
    case BMI2_E_CRT_ERROR:
      Serial.println("Error [" + String(rslt) + "] : CRT error");
      panic_led_trap();
      break;
    case BMI2_E_ST_ALREADY_RUNNING:
      Serial.println("Error [" + String(rslt) + "] : Self test already running");
      panic_led_trap();
      break;
    case BMI2_E_CRT_READY_FOR_DL_FAIL_ABORT:
      Serial.println("Error [" + String(rslt) + "] : CRT ready for DL fail abort");
      panic_led_trap();
      break;
    case BMI2_E_DL_ERROR:
      Serial.println("Error [" + String(rslt) + "] : DL error");
      panic_led_trap();
      break;
    case BMI2_E_PRECON_ERROR:
      Serial.println("Error [" + String(rslt) + "] : PRECON error");
      panic_led_trap();
      break;
    case BMI2_E_ABORT_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Abort error");
      panic_led_trap();
      break;
    case BMI2_E_GYRO_SELF_TEST_ERROR:
      Serial.println("Error [" + String(rslt) + "] : Gyro self test error");
      panic_led_trap();
      break;
    case BMI2_E_GYRO_SELF_TEST_TIMEOUT:
      Serial.println("Error [" + String(rslt) + "] : Gyro self test timeout");
      panic_led_trap();
      break;
    case BMI2_E_WRITE_CYCLE_ONGOING:
      Serial.println("Error [" + String(rslt) + "] : Write cycle ongoing");
      panic_led_trap();
      break;
    case BMI2_E_WRITE_CYCLE_TIMEOUT:
      Serial.println("Error [" + String(rslt) + "] : Write cycle timeout");
      panic_led_trap();
      break;
    case BMI2_E_ST_NOT_RUNING:
      Serial.println("Error [" + String(rslt) + "] : Self test not running");
      panic_led_trap();
      break;
    case BMI2_E_DATA_RDY_INT_FAILED:
      Serial.println("Error [" + String(rslt) + "] : Data ready interrupt failed");
      panic_led_trap();
      break;
    case BMI2_E_INVALID_FOC_POSITION:
      Serial.println("Error [" + String(rslt) + "] : Invalid FOC position");
      panic_led_trap();
      break;
    default:
      Serial.println("Error [" + String(rslt) + "] : Unknown error code");
      panic_led_trap();
      break;
  }
}
