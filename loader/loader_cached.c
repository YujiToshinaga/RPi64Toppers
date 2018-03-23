#include "loader.h"

/*
 *  XMODEM通信は実行速度が遅いとデータ化けを起こしてしまう
 *  ローダ本体，タイマドライバ、シリアルドライバ、XMODEM通信のみ
 *  キャッシュ領域にも置く
 */

#define SYNC_MAGIC 0x87654321

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a
#define XMODEM_BLK_SIZE 128

uint64_t loader_cached_timer_get_syscount();
void loader_cached_serial_put(char c);
char loader_cached_serial_get();
void loader_cached_xmodem_ready();
int loader_cached_xmodem_receiving(uint8_t *pbuf);
int loader_cached_xmodem_read_blk(uint8_t *pbuf);

int loader_cached_sync = 0;

int loader_cached(void)
{
    int load_size = 0;
    uint64_t cur_tim;
    uint32_t tmp32;
    int i;

    // バイナリロードを行う
    if (prc_id() == 0) {
        // ユーザ操作を待つ
        prints("Press y key.\n");
        while (loader_cached_serial_get() != 'y');
        prints("y\n");
        prints("Send your binary with XMODEM.\n");

        // 10秒待つ
        cur_tim = loader_cached_timer_get_syscount(); // [us]
        while (loader_cached_timer_get_syscount() < (cur_tim + 10 * 1000 * 1000));

        // XMODEM転送を行う
        loader_cached_xmodem_ready();
        load_size = loader_cached_xmodem_receiving((uint8_t *)LOAD_TMP_ADDR);

        // キャッシュ領域から非キャッシュ領域へコピー
        for (i = 0; i < load_size; i = i + 4) {
            tmp32 = rd_word(LOAD_TMP_ADDR + i);
            wr_word(LOAD_ADDR + i, tmp32);
        }
        printk("%d bytes loaded.\n", load_size);
        printk("Run at 0x%08x.\n", LOAD_ADDR);

        // 1秒待つ
        cur_tim = loader_cached_timer_get_syscount(); // [us]
        while (loader_cached_timer_get_syscount() < (cur_tim + 1 * 1000 * 1000));
    }

    // バリア同期する
    if (prc_id() == 0) {
        loader_cached_sync = SYNC_MAGIC;
    } else {
        while (loader_cached_sync != SYNC_MAGIC);
    }

    return 0;
}

uint64_t loader_cached_timer_get_syscount()
{
    uint64_t pct;

    CNTPCT_EL0_READ(pct);

    return pct;
}

void loader_cached_serial_put(char c)
{
    while ((rd_word(MU_LSR) & MU_LSR_TX_EMPTY_BIT) != MU_LSR_TX_EMPTY_BIT);
    wr_word(MU_IO, c);
}

char loader_cached_serial_get()
{
    while ((rd_word(MU_LSR) & MU_LSR_DATA_RDY_BIT) != MU_LSR_DATA_RDY_BIT);
    return ((char)(rd_word(MU_IO) & MU_IO_DATA_MASK));
}

void loader_cached_xmodem_ready()
{
    loader_cached_serial_put(XMODEM_NAK);
}

int loader_cached_xmodem_receiving(uint8_t *pbuf)
{
    int read_size;
    int size;
    char c;

    size = 0;
    for ( ; ; ) {
        c = loader_cached_serial_get();
        if (c == XMODEM_EOT) {
            loader_cached_serial_put(XMODEM_ACK);
            break;
        } else if (c == XMODEM_SOH) {
            read_size = loader_cached_xmodem_read_blk(pbuf);
            if (read_size == 0) {
                loader_cached_serial_put(XMODEM_NAK);
            } else {
                loader_cached_serial_put(XMODEM_ACK);
                pbuf += read_size;
                size += read_size;
            }
        } else {
            size = 0;
            break;
        }
    }

    return size;
}

int loader_cached_xmodem_read_blk(uint8_t *pbuf)
{
    uint8_t c, block_num, check_sum;
    int i;
    
    block_num  = loader_cached_serial_get();
    block_num ^= loader_cached_serial_get();

    if (block_num != 0xff) {
        return 0;
    }

    check_sum = 0;
    for (i = 0; i < XMODEM_BLK_SIZE; i++) {
        c = loader_cached_serial_get();
        wr_byte((uint64_t)pbuf, c);
        check_sum += c;
        pbuf++;
    }

    c = loader_cached_serial_get();
    if (check_sum != c) {
        return 0;
    }

    return XMODEM_BLK_SIZE;
}

