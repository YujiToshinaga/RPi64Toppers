# Binary Loader for Raspberry Pi AArch64

PCでビルドした実行ファイルを，
Raspberry PiにUSB-シリアルケーブルで転送して，実行するためのツール．

Raspberry PiでTOPPERSカーネルを動かす際，
いちいちSDカードを抜き差ししてファイルロードするか，
または，JTAG機材を接続してダウンロードが必要となる．
このツールを用いると，
USBシリアルケーブル一本でバイナリ転送と実行が可能にになる．
デバッグ時のビルド＆トライを繰り返す際に便利である．

まだ開発中のため，masterブランチ未投入である．
developブランチをcheckoutして利用可能である．

## 概要

 - シリアル経由でバイナリ転送 -> ジャンプ -> 実行
 - 転送は単純なXMODEM protocol
 - Raspberry Pi 3のARMv8 AArch64モードで動作
 - 4コアでマルチコア実行
 - ジャンプ後は例外レベル1ノンセキュア(EL1NS)、キャッシュ，MMUはOFF

## 必要な機材

 - Raspberry Pi 3 + 電源
 
 - microSDカード + カードリーダー  
 FAT32フォーマットされたmicroSDカードを用意する  
 数十MB程度の空きがあれば十分である
 
 - USBシリアル変換ケーブル  
 シリアルコンソールで通信するために使用する  
 TTL-232R-3V3を使用した

 - PC（Windows / Linuxどちらでも可）  
 ビルド，および動作に使用する

## 開発環境の構築

 - コンパイラ  
 以下からフリーのARMv8 AArch64用コンパイラを入手する  
 https://www.linaro.org/downloads/  
 開発では以下Versionのものを使用した  
 gcc-linaro-6.3.1-2017.02-i686_aarch64-elf  
 ダウンロードして適当な場所に解凍したら
 ```<解凍したディレクトリ>/gcc-linaro-6.3.1-2017.02-i686_aarch64-elf/bin```
 にpathに通す

 - ターミナルアプリ  
 Tera Termを利用した
 Tera TermはXMODEMファイル送信機能がついている

 - その他ツール  
 おそらく以下パッケージが必要となる  
 make, git, gcc-core

## ビルド

このリポジトリを取得してloader.binをビルドする
```
git clone https://github.com/YujiToshinaga/RPi64Toppers.git
cd RPi64Toppers
git checkout develop
cd loader
make
```

## 動作準備

### SDカードの準備

FAT32フォーマットされたSDカード直下に  
以下の4ファイルを置いてRaspberry Piに挿す

 - bootcode.bin, start.elf  
 以下から```bootcode.bin```と```start.elf```をダウンロードする  
 https://github.com/raspberrypi/firmware/tree/master/boot

 - config.txt  
 このリポジトリの```RPi64Toppers/loader/config.txt```を使用する
 
 - loader.bin  
 ビルドした```RPi64Toppers/loader/loader.bin```を使用する

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

## 実行

Tera Termを開く
Raspberry Piの電源を入れる
コンソール上でyを押す
Tera Termの 「ファイル」 -> 「転送」 -> 「XMODEM」 -> 「送信」から
転送実行したいバイナリを選ぶ

しばらく待つと転送したバイナリの実行を開始する．

