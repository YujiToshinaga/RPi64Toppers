# RPi64Toppers

## セットアップ

### bootcode.bin, start.elfの取得

以下からbootcode.binとstart.elfをダウンロードする
https://github.com/raspberrypi/firmware/tree/master/boot

### config.txtの用意

このリポジトリからconfig.txtを取得する

### fmp.binのビルド

cd fmp/build/sample
make fmp.bin

### SDカードの準備

SDカードをFAT32でフォーマットする
以下ファイルをSDカード直下に置いてRaspberry Piに挿す

 - bootcode.bin
 - start.elf
 - config.txt
 - fmp.bin

### シリアルコンソールの接続

TTL-232R-3V3を使用する場合
GPIO14(TXD1) - Yellow(RXD)
GPIO15(RXD1) - Orange(TXD)
Ground - Ground

ボーレートは115200bpsになる

## 起動

Raspberry Piの電源を入れる

