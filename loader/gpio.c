#include "stddef.h"
#include "bcm283x.h"
#include "gpio.h"

void gpio_init()
{
    wr_word(GPIO_GPPUD, 0);
}

void gpio_fsel(int n, int fsel)
{
    uint32_t tmp;
    uint32_t sel;
    int i;

    switch (fsel) {
        case GPIO_FSEL_IN:
            sel = 0;
            break;
        case GPIO_FSEL_OUT:
            sel = 1;
            break;
        case GPIO_FSEL_0:
            sel = 4;
            break;
        case GPIO_FSEL_1:
            sel = 5;
            break;
        case GPIO_FSEL_2:
            sel = 6;
            break;
        case GPIO_FSEL_3:
            sel = 7;
            break;
        case GPIO_FSEL_4:
            sel = 3;
            break;
        case GPIO_FSEL_5:
            sel = 2;
            break;
        default:
            sel = 0;
            break;
    }

    if ((n >= 0) && (n < 10)) {
        tmp = rd_word(GPIO_GPFSEL0);
        tmp &= ~(0x7 << (3 * (n - 0)));
        tmp |= sel << (3 * (n - 0));
        wr_word(GPIO_GPFSEL0, tmp);
    } else if ((n >= 10) && (n < 20)) {
        tmp = rd_word(GPIO_GPFSEL1);
        tmp &= ~(0x7 << (3 * (n - 10)));
        tmp |= sel << (3 * (n - 10));
        wr_word(GPIO_GPFSEL1, tmp);
    } else if ((n >= 20) && (n < 30)) {
        tmp = rd_word(GPIO_GPFSEL2);
        tmp &= ~(0x7 << (3 * (n - 20)));
        tmp |= sel << (3 * (n - 20));
        wr_word(GPIO_GPFSEL2, tmp);
    } else if ((n >= 30) && (n < 40)) {
        tmp = rd_word(GPIO_GPFSEL3);
        tmp &= ~(0x7 << (3 * (n - 30)));
        tmp |= sel << (3 * (n - 30));
        wr_word(GPIO_GPFSEL3, tmp);
    } else if ((n >= 40) && (n < 50)) {
        tmp = rd_word(GPIO_GPFSEL4);
        tmp &= ~(0x7 << (3 * (n - 40)));
        tmp |= sel << (3 * (n - 40));
        wr_word(GPIO_GPFSEL4, tmp);
    } else if ((n >= 50) && (n <= 53)) {
        tmp = rd_word(GPIO_GPFSEL5);
        tmp &= ~(0x7 << (3 * (n - 50)));
        tmp |= sel << (3 * (n - 50));
        wr_word(GPIO_GPFSEL5, tmp);
    }

    for (i = 0; i < 150; i++) {
        asm volatile("nop");
    }

    if ((n >= 0) && (n < 32)) {
        wr_word(GPIO_GPPUDCLK0, (0x1 << (n - 0)));
    } else if ((n >= 32) && (n <= 53)) {
        wr_word(GPIO_GPPUDCLK1, (0x1 << (n - 32)));
    }

    for (i = 0; i < 150; i++) {
        asm volatile("nop");
    }

    if ((n >= 0) && (n < 32)) {
        wr_word(GPIO_GPPUDCLK0, 0);
    } else if ((n >= 32) && (n <= 53)) {
        wr_word(GPIO_GPPUDCLK1, 0);
    }
}

void gpio_set(int n, int set)
{
    if ((n >= 0) && (n < 32)) {
        if (set == GPIO_HIGH) {
            wr_word(GPIO_GPSET0, (0x1 << (n - 0)));
        } else {
            wr_word(GPIO_GPCLR0, (0x1 << (n - 0)));
        }
    } else if ((n >= 32) && (n <= 53)) {
        if (set == GPIO_HIGH) {
            wr_word(GPIO_GPSET1, (0x1 << (n - 32)));
        } else {
            wr_word(GPIO_GPCLR1, (0x1 << (n - 32)));
        }
    } 
}

int gpio_get(int n)
{
    uint32_t get;
    int ret;

    if ((n >= 0) && (n < 32)) {
        get = rd_word(GPIO_GPLEV0);
        if (get & (0x1 << (n - 0))) {
            ret = GPIO_HIGH;
        } else {
            ret = GPIO_LOW;
        }
    } else if ((n >= 32) && (n <= 53)) {
        get = rd_word(GPIO_GPLEV1);
        if (get & (0x1 << (n - 32))) {
            ret = GPIO_HIGH;
        } else {
            ret = GPIO_LOW;
        }
    } 

    return ret;
}

