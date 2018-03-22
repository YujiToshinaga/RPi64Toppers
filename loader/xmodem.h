#ifndef XMODEM_H
#define XMODEM_H

extern void xmodem_ready();
extern int xmodem_receiving(uint8_t *pbuf);
extern uint64_t xmodem_read_blk(uint8_t *pbuf);

#endif /* XMODEM_H */
