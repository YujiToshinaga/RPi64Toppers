# RPi64Toppers

Raspberry Pi 3 (AArch64)でTOPPERS/FMPを動かす

## 必要なもの

 - Raspberry Pi 3 + 電源
 
 - SDカード  
  FAT32フォーマットされたSDカードを用意する  
  空きが数十MBあれば十分である
 
 - USBシリアル変換ケーブル  
  シリアルコンソールで通信するのに使用する  　
  開発ではTTL-232R-3V3を使用した  
  さらに、ターミナルアプリ（Tera Termなど）をインストールしておく

 - コンパイラ  
  以下からaarch64用コンパイラを入手する  
  https://www.linaro.org/downloads/  
  開発ではWindows(cygwin用)の以下Versionのものを使用した  
  gcc-linaro-6.3.1-2017.02-i686-mingw32_aarch64-elf  
  DownLoadして適当な場所に解凍したら```<解凍したディレクトリ>/bin/aarch64-elf-gcc```をpathを通す

## ビルド

このリポジトリを取得してfmp.binをビルドする
```
cd RPi64Toppers/fmp
mkdir test
cd test
perl ../configure -T rpi_arm64_gcc
make fmp.bin
```

## セットアップ

### bootcode.bin, start.elf

以下からbootcode.binとstart.elfをダウンロードする  
https://github.com/raspberrypi/firmware/tree/master/boot

### config.txt

このリポジトリからconfig.txtを取得する

### fmp.bin

ビルドしたfmp.binを使用する

### SDカードの準備

FAT32フォーマットされたSDカード直下に  
以下ファイルを置いてRaspberry Piに挿す

 - bootcode.bin
 - start.elf
 - config.txt
 - fmp.bin

### シリアルコンソールの接続

Raspberry PiのTXD1, RXD1をシリアルデバイスと接続する  
ボーレートは115200bpsにする

TTL-232R-3V3を使用する場合

RPiのピン | 結線 | TTL-232R-3V3のピン
---|---|---
GPIO14(TXD1) | - | Yellow(RXD)
GPIO15(RXD1) | - | Orange(TXD)
Ground | - | Ground |

## 起動

Raspberry Piの電源を入れる

