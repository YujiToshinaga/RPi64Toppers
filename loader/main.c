#include "stddef.h"
#include "baremetal.h"
#include "aarch64.h"
#include "bcm283x.h"
#include "printk.h"
#include "serial.h"
#include "gpio.h"
#include "timer.h"
#include "xmodem.h"

#define SYNC_MAGIC 0x87654321

int sync = 0;
int exit_sync = 0;

int main(void)
{
    mmap_t mm;
    uint64_t tmp64;

    // シリアルを有効化する
    if (prc_id() == 0) {
        serial_init();
    }

    if (prc_id() == 0) {
        prints("Binary loader with XMODEM protocol\n");
        printk("    Load point : 0x%08x\n", LOAD_ADDR);
        printk("    Max size   : %d bytes\n", LOAD_SIZE);
        prints("\n");
    }

    // キャッシュ、MMUを有効化する
    cache_disable();

    SCTLR_EL1_READ(tmp64);
    tmp64 |= (SCTLR_SA_BIT | SCTLR_A_BIT);
    SCTLR_EL1_WRITE(tmp64);
    ISB();

    CPUECTLR_EL1_READ(tmp64);
    tmp64 |= CPUECTLR_SMPEN_BIT;
    CPUECTLR_EL1_WRITE(tmp64);
    ISB();

    mmu_mmap_init();

    mm.pa   = MAIN_ADDR;
    mm.va   = mm.pa;
    mm.size = MAIN_SIZE;
    mm.attr = MEM_ATTR_SO;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOADER_ADDR;
    mm.va   = mm.pa;
    mm.size = LOADER_SIZE;
    mm.attr = MEM_ATTR_NML_C;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOAD_TMP_ADDR;
    mm.va   = mm.pa;
    mm.size = LOAD_SIZE;
    mm.attr = MEM_ATTR_NML_C;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOAD_ADDR;
    mm.va   = mm.pa;
    mm.size = LOAD_SIZE;
    mm.attr = MEM_ATTR_SO;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = IO0_ADDR;
    mm.va   = mm.pa;
    mm.size = IO0_SIZE;
    mm.attr = MEM_ATTR_DEV;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = IO1_ADDR;
    mm.va   = mm.pa;
    mm.size = IO1_SIZE;
    mm.attr = MEM_ATTR_DEV;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);

    mmu_init();
    cache_enable();

    // バリア同期する
    if (prc_id() == 0) {
        sync = SYNC_MAGIC;
    } else {
        while (sync != SYNC_MAGIC);
    }

    return 0;
}


int main_exit(void)
{
    cache_disable();
    mmu_term();

    // バリア同期する
    if (prc_id() == 0) {
        exit_sync = SYNC_MAGIC;
    } else {
        while (exit_sync != SYNC_MAGIC);
    }

    return 0;
}

