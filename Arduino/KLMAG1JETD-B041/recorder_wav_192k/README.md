<div align="right">
<a href="https://developer.sony.com/ja/develop/ssup/"><img src="../../../images/SSUPLOGO2.png" width="150"></a>
</div>

# recorder_wav_192kサンプルプログラム

本サンプルは[Spresense 用 eMMC(KLMAG1JETD-B041) AddOnボード](https://nextstep.official.ec/items/66602892)を使用して、マイクからの音声を192KHz,MONO,16bit,1時間で録音して、10分ごとに分割し、eMMC に WAV ファイルとして記録します。

## 動作確認したときの環境
### 開発環境
- PC: Ubuntu 18.04
- Arduino IDE:v1.8.13
- Spresense Arduino:v2.6.0

### 使用デバイス
- Spresense メインボード
- Spresense 拡張ボード
- Spresense 用 eMMC(KLMAG1JETD-B041) AddOnボード
- 録音用マイク

## 事前準備
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)に記載の手順に従って環境を構築します。なお、Spresense Arduino環境インストール済みの場合は実施不要です。

## ビルド方法
1. [Arduinoソースコードビルド方法](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html#_led_%E3%81%AE%E3%82%B9%E3%82%B1%E3%83%83%E3%83%81%E3%82%92%E5%8B%95%E3%81%8B%E3%81%97%E3%81%A6%E3%81%BF%E3%82%8B)を参照して、
[recorder_wav_192k.ino](./recorder_wav_192k.ino)をArduino IDEで開いてマイコンボードに書き込む ボタンをクリックして、スケッチのコンパイルと書き込みを行います。
2. スケッチの書き込みが完了するまで待ちます。
3. スケッチの書き込みが完了すると自動的にリセットしてプログラムが起動します。

## サンプルプログラム

### 動作例

|高いサンプリングの音声録音(192KHz,MONO,16bit,1時間録音して、10分ごとに分割して録音する)|
|----|
|![recorder](images/recorder.png)|

### 操作方法
特に不要。

### 確認方法
- [UsbMsc](../UsbMsc/)をご参照ください。

## 参考ページ
- [Spresense Arduino スタートガイド](https://developer.sony.com/develop/spresense/docs/arduino_set_up_ja.html)
- [Spresense 用 eMMC(KLMAG1JETD-B041) AddOnボード](https://nextstep.official.ec/items/66602892)
- [WAV 形式で録音する](https://developer.sony.com/develop/spresense/docs/arduino_tutorials_ja.html#_wav_recorder)

## 変更履歴
|リリース日|変更点|
|----|----|
|2022/11/11|初版|
|2022/12/16|Spresense Arduino:v2.6.0にて動作確認済み|
