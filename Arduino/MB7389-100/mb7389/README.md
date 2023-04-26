<div align="right">
<a href="https://developer.sony.com/ja/develop/ssup/"><img src="../../../images/SSUPLOGO2.png" width="150"></a>
</div>

# MB7389サンプルプログラム

[MB7389-100](https://www.maxbotix.com/ultrasonic_sensors/mb7389.htm)のセンサーから距離データを取得して、FlashメモリやSDカードにcsvの形式で保存するサンプルプログラムです。データ取得間隔は秒単位で設定できます。なお、本サンプルはanalog voltageと pulse width２つの方法をサポートします。
  - デフォルトはpulse widthのサンプルとなります。
  - analog voltageを使う場合は #define USE_ANALOG_VOLTAGE を定義してください。
  - analog voltageと pulse widthは物理接続方法が異なるため、それぞれの接続方法は[参考資料](../docs/MB7389-100_Summary.pdf)を参照してください。

## 動作確認したときの環境

### 開発環境
- PC: Ubuntu 18.04
- Arduino IDE:v1.8.13
- Spresense Arduino:v2.6.0

### 使用デバイス
- Spresense Main Board
- Spresense 拡張Board
- MB7389-100超音波センサー
- ジャンパー線
- ヘッダーピン(※MB7389-100超音波センサーにはんだ付け必要

## 事前準備
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)に記載の手順に従って環境を構築してください。なお、Spresense Arduino環境インストール済みの場合は実施不要です。

## ビルド方法
1. [Arduinoソースコードビルド方法](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html#_led_%E3%81%AE%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81%E3%82%92%E5%8B%95%E3%81%8B%E3%81%97%E3%81%A6%E3%81%BF%E3%82%8B)を参照して、[mb7389.ino](./mb7389.ino)をArduino IDEで開いてマイコンボードに書き込むボタンをクリックして、スケッチのコンパイルと書き込みを行います。
2. スケッチの書き込みが完了するまで待ちます。
3. スケッチの書き込みが完了すると自動的にリセットしてプログラムが起動します。

## サンプルプログラム

### 動作例

|取得項目|値の例|
|----|----|
|Time	|Distance(mm)|
|2022/07/28/15:27:43|	537|
|2022/07/28/15:27:44|	538|
|2022/07/28/15:27:45|	537|

### 使用方法
Arduino IDEのシリアルモニタを開いて、データをどちらに保存するかを選択してEnterキーを押してください。

|シリアルモニタを開く|保存場所を選択する|csvファイルを確認する|
|----|----|----|
|![シリアルモニタを開く](images/シリアルモニタを開く.png)|![保存場所を選択する](images/保存場所を選択する.png)|![csvファイルを確認する](images/csvファイルを確認する.png)|

(注1)データをずっとファイルに書き込み続けます。終了するには[Spresenseの[RST] Reset button](https://developer.sony.com/develop/spresense/docs/introduction_ja.html)を押してください。<br/>
(注2)データを取得再開する場合、前のファイルを上書きするため、ファイルをバックアップしてからデータを取得してください。

### 操作方法
Arduino IDEのシリアルモニタを開いている状態で、以下のキーを入力することで操作が可能です。
|入力キー|動作|
|----|----|
|1＋Enterキー|データをSDカードに保存する|
|2＋Enterキー|データをFlashメモリに保存する|

### FlashメモリとSDカード共通
[Zmodem を使ってPCにファイル転送](https://developer.sony.com/develop/spresense/docs/sdk_tutorials_ja.html#_tips_zmodem)に記載の手順に従ってファイルをPCで確認できます。
- 2022/12/15に特に問題がないことを動作確認済み。

### SDカードのみ
手動でSDカードをPCに差し込み確認できます。

## 参考ページ
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)
- [Zmodem を使ってPCにファイル転送](https://developer.sony.com/develop/spresense/docs/sdk_tutorials_ja.html#_tips_zmodem)

## 変更履歴
|リリース日|変更点|
|----|----|
|2022/07/29|初版|
|2022/12/16|Spresense Arduino:v2.6.0にて動作確認済み|
