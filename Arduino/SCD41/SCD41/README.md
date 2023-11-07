# CO2(SCD41)サンプルプログラム

CO2(SCD41)センサーからCO2濃度、温湿度のデータを取得して、FlashメモリやSDカードにcsvの形式で保存するサンプルプログラムとなります。データ取得間隔は秒単位で設定できます。
※データが準備できるまではおよそ5秒かかるため、取得間隔は5秒以上に設定してください。

## 動作確認したときの環境

### 開発環境
- PC: Ubuntu 18.04
- Arduino IDE v1.8.13
- Spresense Arduino v2.6.0

### 使用デバイス
- Spresense Main Board
- Spresense 用 CO2センサー Add-on ボード

### ライブラリ
- [CO2(SCD41) Dependencies library](https://github.com/Sensirion/arduino-core)
- [CO2(SCD41) library](https://github.com/Sensirion/arduino-i2c-scd4x)

## 事前準備
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)に記載の手順に従って環境を構築します。なお、Spresense Arduino環境インストール済みの場合は実施不要です。
- [CO2(SCD41) Dependencies library](https://github.com/Sensirion/arduino-core)および[CO2(SCD41) library](https://github.com/Sensirion/arduino-i2c-scd4x)をインストールします。Spresense Arduinoライブラリのインストール方法は[こちら](https://github.com/SonySemiconductorSolutions/ssup-spresense-internal/blob/main/FAQ.md#arduino%E3%83%A9%E3%82%A4%E3%83%96%E3%83%A9%E3%83%AA%E3%82%92%E3%82%A4%E3%83%B3%E3%82%B9%E3%83%88%E3%83%BC%E3%83%AB%E3%81%99%E3%82%8B%E6%96%B9%E6%B3%95)をご参照ください。既にインストール済みの場合は実施不要です。

## ビルド方法
1. [Arduinoソースコードビルド方法](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html#_led_%E3%81%AE%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81%E3%82%92%E5%8B%95%E3%81%8B%E3%81%97%E3%81%A6%E3%81%BF%E3%82%8B)を参照して、[SCD41.ino](./SCD41.ino)をArduino IDEで開いてマイコンボードに書き込む ボタンをクリックして、スケッチのコンパイルと書き込みを行います。
2. スケッチの書き込みが完了するまで待ちます。
3. スケッチの書き込みが完了すると自動的にリセットしてプログラムが起動します。

## サンプルプログラム

### 動作例

|Time|CO2(SCD41)(ppm)|Temperature(℃)|Humidity(%)|
|----|----|----|----|
|2022/05/31/13:56:33|514|30.53|44.66|
|2022/05/31/13:56:38|515|30.37|45.14|
|2022/05/31/13:56:43|506|30.06|46.06|

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
- 2022/12/21に特に問題がないことを動作確認済み。

### SDカードのみの確認方法
手動でSDカードをPCに差し込み確認できます。

## 変更履歴
|バージョン|リリース日|変更概要|
|----|----|----|
|v0.1|2022.06.03|初版|
|v0.2|2022.12.23|Spresense Arduino:v2.6.0で動作確認及びドキュメント更新|
