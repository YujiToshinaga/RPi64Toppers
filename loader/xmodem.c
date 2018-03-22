#include "stddef.h"
#include "serial.h"

#define XMODEM_SOH 0x01
#define XMODEM_STX 0x02
#define XMODEM_EOT 0x04
#define XMODEM_ACK 0x06
#define XMODEM_NAK 0x15
#define XMODEM_CAN 0x18
#define XMODEM_EOF 0x1a
#define XMODEM_BLK_SIZE 128

int xmodem_read_blk(uint8_t *pbuf);

void xmodem_ready()
{
    serial_put(XMODEM_NAK);
}

int xmodem_receiving(uint8_t *pbuf)
{
    int read_size;
    int size;
    char c;

    size = 0;
    for ( ; ; ) {
        c = serial_get();
        if (c == XMODEM_EOT) {
            serial_put(XMODEM_ACK);
            break;
        } else if (c == XMODEM_SOH) {
            read_size = xmodem_read_blk(pbuf);
            if (read_size == 0) {
                serial_put(XMODEM_NAK);
            } else {
                serial_put(XMODEM_ACK);
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

int xmodem_read_blk(uint8_t *pbuf)
{
    uint8_t c, block_num, check_sum;
    int i;
    
    block_num  = serial_get();
    block_num ^= serial_get();

    if (block_num != 0xff) {
        return 0;
    }

    check_sum = 0;
    for (i = 0; i < XMODEM_BLK_SIZE; i++) {
        c = serial_get();
        wr_byte((uint64_t)pbuf, c);
        check_sum += c;
        pbuf++;
    }

    c = serial_get();
    if (check_sum != c) {
        return 0;
    }

    return XMODEM_BLK_SIZE;
}

