# RPi64Toppers

Raspberry Pi 64bitモード向けTOPPERSリアルタイムカーネル

## 概要

 - TOPPERS/FMPカーネルを移植 (https://www.toppers.jp)
 - Raspberry Pi 3のARMv8 AArch64モードで動作
 - 4コアで動作
 - キャッシュ，MMUはON
 - 各コアのARM Generic Timerを使用
 - Raspberry PiのMini UARTを使用
 - 例外レベル3(EL3)でブートし，例外レベル1ノンセキュア(EL1NS)でカーネル動作

制限事項

 - 割込み優先度の概念は未実装


## 必要な機材

 - Raspberry Pi 3 + 電源
 
 - microSDカード + カードリーダー  
 FAT32フォーマットされたmicroSDカードを用意する  
 数十MB程度の空きがあれば十分である
 
 - USBシリアル変換ケーブル  
 シリアルコンソールで通信するために使用する  
 TTL-232R-3V3を使用した  

 - Windows PC  
 Cygwinを用いてビルドを行う  
 説明は省くがLinux PCでも可

## 開発環境の構築

 - Cygwin  
 WindwosPCにCygwinをインストールする  
 おそらく以下パッケージが必要  
 make, perl, git, gcc-core, gcc-g++  

 - コンパイラ  
 以下からフリーのARMv8 AArch64用コンパイラを入手する  
 https://www.linaro.org/downloads/  
 開発ではWindows用の以下Versionのものを使用した  
 gcc-linaro-6.3.1-2017.02-i686-mingw32_aarch64-elf  
 DownLoadして適当な場所に解凍したら
 ```<解凍したディレクトリ>/gcc-linaro-6.3.1-2017.02-i686-mingw32_aarch64-elf/bin/aarch64-elf-gcc```
 にpathに通す

 - ターミナルアプリ  
 シリアルコンソールで通信するためにTera Termなどをインストールしておく

## ビルド

このリポジトリを取得してfmp.binをビルドする
```
git clone https://github.com/YujiToshinaga/RPi64Toppers.git
cd RPi64Toppers/fmp
mkdir build
cd build
perl ../configure -T rpi_arm64_gcc
make fmp.bin
```

## 動作準備

### SDカードの準備

FAT32フォーマットされたSDカード直下に  
以下の4ファイルを置いてRaspberry Piに挿す

 - bootcode.bin, start.elf  
 以下から```bootcode.bin```と```start.elf```をダウンロードする  
 https://github.com/raspberrypi/firmware/tree/master/boot

 - config.txt  
 このリポジトリの```RPi64Toppers/config.txt```を使用する
 
 - fmp.bin  
 ビルドした```fmp.bin```を使用する

### シリアルコンソールの接続

Raspberry PiのGPIO14(TXD1), GPIO15(RXD1), Groundをシリアルデバイスと接続する  
ボーレートは115200bpsを設定する

Raspberry Piのピン配置  
https://www.raspberrypi.org/documentation/usage/gpio-plus-and-raspi2/README.md

TTL-232R-3V3を使用する場合

RPiのピン | 結線 | TTL-232R-3V3のピン
---|---|---
GPIO14(TXD1) | - | Yellow(RXD)
GPIO15(RXD1) | - | Orange(TXD)
Ground | - | Ground |

## 起動

Raspberry Piの電源を入れる

