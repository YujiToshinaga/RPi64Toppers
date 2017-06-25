# RPi64Toppers

## セットアップ

### bootcode.bin, start.elf

以下からbootcode.binとstart.elfをダウンロードする  
https://github.com/raspberrypi/firmware/tree/master/boot

### config.txt

このリポジトリからconfig.txtを取得する

### fmp.bin

このリポジトリを取得してfmp.binをビルドする
```
cd fmp/build/sample
make fmp.bin
```

### SDカードの準備

SDカードをFAT32でフォーマットする  
以下ファイルをSDカード直下に置いてRaspberry Piに挿入する

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

