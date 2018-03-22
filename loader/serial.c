#include "stddef.h"
#include "bcm283x.h"
#include "serial.h"

#define UART_BAUDRATE       115200

void serial_init()
{
    uint32_t baud;
    uint32_t tmp;

    // GPIOをUARTに切り替え
    tmp = rd_word(GPIO_GPFSEL1);
    tmp &= ~(7 << 12);      // GPIO14 ALT5
    tmp |= 2 << 12;
    tmp &= ~(7 << 15);      // GPIO15 ALT5
    tmp |= 2 << 15;
    wr_word(GPIO_GPFSEL1, tmp);
    wr_word(GPIO_GPPUD, 0);
    for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
    wr_word(GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
    for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
    wr_word(GPIO_GPPUDCLK0, 0);

    baud = (250000000 / (UART_BAUDRATE * 8)) - 1;

    wr_word(AUX_ENABLES, rd_word(AUX_ENABLES) | AUX_ENABLES_MU_EN_BIT);
    wr_word(MU_CNTL, 0);
    wr_word(MU_IER, 0);
    wr_word(MU_IIR, MU_IIR_FIFO_CLR);
    wr_word(MU_LCR, MU_LCR_DS_8BIT);
    wr_word(MU_MCR, 0);
    wr_word(MU_BAUD, baud);
    wr_word(MU_CNTL, MU_CNTL_TX_EN_BIT | MU_CNTL_RX_EN_BIT);
}

char serial_getready()
{
    return ((rd_word(MU_LSR) & MU_LSR_DATA_RDY_BIT) == MU_LSR_DATA_RDY_BIT);
}

void serial_put(char c)
{
    while ((rd_word(MU_LSR) & MU_LSR_TX_EMPTY_BIT) != MU_LSR_TX_EMPTY_BIT);
    wr_word(MU_IO, c);
}

char serial_get()
{
    while ((rd_word(MU_LSR) & MU_LSR_DATA_RDY_BIT) != MU_LSR_DATA_RDY_BIT);
    return ((char)(rd_word(MU_IO) & MU_IO_DATA_MASK));
}

