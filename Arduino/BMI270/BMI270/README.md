<div align="right">
<a href="https://developer.sony.com/ja/develop/ssup/"><img src="../../../images/SSUPLOGO2.png" width="150"></a>
</div>

# BMI270サンプルプログラム

BMI270センサーから3軸加速度や3軸ジャイロのデータを取得して、FlashメモリやSDカードにcsvの形式で保存するサンプルプログラムとなります。データ取得間隔は秒単位で設定できます。

## 動作を確認したときの環境

### 開発環境
- PC : Ubuntu 20.04 または Windows 11
- Arduino IDE: v2.1.0
- Spresense Arduino v3.0.1

### 使用デバイス
- Spresense Main Board
- BMI270

### ライブラリ
- [BMI270 library](https://github.com/BoschSensortec/BMI270-Sensor-API)

## 事前準備
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)に記載の手順に従って環境を構築します。なお、Spresense Arduino環境インストール済みの場合は実施不要です。
- [BMI270 library](https://github.com/BoschSensortec/BMI270-Sensor-API)をインストールします。Spresense Arduinoライブラリのインストール方法は[こちら](https://github.com/SonySemiconductorSolutions/ssup-spresense-internal/blob/main/FAQ.md#arduino%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA%E3%82%92%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E3%81%99%E3%82%8B%E6%96%B9%E6%B3%95)をご参照ください。既にインストール済みの場合は実施不要です。

## ビルド方法
1. [Arduinoソースコードビルド方法](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html#_led_%E3%81%AE%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81%E3%82%92%E5%8B%95%E3%81%8B%E3%81%97%E3%81%A6%E3%81%BF%E3%82%8B)を参照して、[BMI270.ino](./BMI270.ino)をArduino IDEで開いてマイコンボードに書き込む ボタンをクリックして、スケッチのコンパイルと書き込みを行います。
2. スケッチの書き込みが完了するまで待ちます。
3. スケッチの書き込みが完了すると自動的にリセットしてプログラムが起動します。

## サンプルプログラム

### 動作例

|Acc|Accelerometer_X(m/s2)|Accelerometer_Y(m/s2)|Accelerometer_Z(m/s2)|Gyro|Gyro_X(°/s)|Gyro_Y(°/s)|Gyro_Z(°/s)|
|----|----|----|----|----|----|----|----|
|a:|0.12|-0.30|9.68	|g:|-0.03|0.06|-0.06|
|a:|0.12|-0.31|9.67	|g:|-0.02|0.04|-0.08|
|a:|0.11|-0.31|9.68	|g:|-0.04|0.05|-0.08|

### 使用方法
Arduino IDEのシリアルモニタを開いて、データをどちらに保存するかを選択して、Enterキーを押すだけです。

|シリアルモニタを開く|保存場所を選択する|csvファイルを確認する|
|----|----|----|
|![シリアルモニタを開く](images/シリアルモニタを開く.PNG)|![保存場所を選択する](images/保存場所を選択する.png)|![csvファイルを確認する](images/csvファイルを確認する.png)|

(注1)起動するとデータのファイル書き込みが続きます。終了するには[Spresenseの[RST] Reset button](https://developer.sony.com/develop/spresense/docs/introduction_ja.html)を押してください。<br/>
(注2)データを取得再開する場合、前のファイルを上書きするため、ファイルをバックアップしてからデータを取得してください。

### 操作方法
Arduino IDEのシリアルモニタを開いている状態で、以下のキーを入力することで操作が可能
|入力キー|動作|
|----|----|
|1＋Enterキー|データをSDカードに保存する|
|2＋Enterキー|データをFlashメモリに保存する|

### FlashメモリとSDカード共通の確認方法
[Zmodem を使ってPCにファイル転送](https://developer.sony.com/develop/spresense/docs/sdk_tutorials_ja.html#_tips_zmodem)に記載の手順に従ってファイルをPCで確認できます。

### SDカードのみの確認方法
手動でSDカードをPCに差し込み確認できます。

## 変更履歴
|リリース日|変更点|
|----|----|
|2023/04/28|初版|
