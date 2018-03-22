#ifndef SERIAL_H
#define SERIAL_H

extern void serial_init();
extern char serial_getready();
extern void serial_put(char c);
extern char serial_get();

#endif /* SERIAL_H */
