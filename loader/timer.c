#include "stddef.h"
#include "bcm283x.h"
#include "aarch64.h"
#include "timer.h"

void timer_init()
{
}

uint64_t timer_get_syscount()
{
    uint64_t pct;

    CNTPCT_EL0_READ(pct);

    return pct;
}
