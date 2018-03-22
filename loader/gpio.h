#ifndef GPIO_H
#define GPIO_H

#define GPIO_HIGH       0
#define GPIO_LOW        1

#define GPIO_FSEL_IN    0
#define GPIO_FSEL_OUT   1
#define GPIO_FSEL_0     2
#define GPIO_FSEL_1     3
#define GPIO_FSEL_2     4
#define GPIO_FSEL_3     5
#define GPIO_FSEL_4     6
#define GPIO_FSEL_5     7

extern void gpio_init();
extern void gpio_fsel(int n, int sel);
extern void gpio_set(int n, int set);
extern int gpio_get(int n);

#endif /* GPIO_H */
