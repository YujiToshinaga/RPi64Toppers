#include "stddef.h"
#include "aarch64.h"
#include "bcm283x.h"

/*
 *  プロセッサID（0オリジン）の取得
 */
int prc_id(void)
{
    uint64_t tmp;
    int id;

    MPIDR_EL1_READ(tmp);
    id = (int)(tmp & MPIDR_AFF0_MASK);

    return id;
}
