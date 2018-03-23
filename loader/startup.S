#define MACRO_ONLY
#include "loader.h"

//#define DEBUG_CORE0
//#define DEBUG_JTAG
//#define DEBUG_MU
//#define DEBUG_STUB
//#define DEBUG_SECURE

    /*
     *  エントリポイント
     */
    .global	_entry
    .section .startup, "ax"
    .type _entry, %function
_entry:
    b     _loader_entry

    .global	_loader_entry
    .section .loader_startup, "ax"
    .type _loader_entry, %function
_loader_entry:

#ifdef DEBUG_CORE0
    /*
     *  コア0だけ実行する
     */
dbgcore0:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    cmp   x0, xzr
    b.eq  dbgcore0_exit
dbgcore0_sleep:
    wfi
    b     dbgcore0_sleep
dbgcore0_exit:
#endif /* DEBUG_CORE0 */

#ifdef DEBUG_JTAG
    /*
     *  JTAGの有効化する
     */
dbgjtag:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    cmp   x0, xzr
    b.ne  dbgjtag_exit
dbgjtag_gpfsel:
    ldr   x0, =GPIO_GPFSEL2
    ldr   w1, [x0]
    mov   w3, #7            /* FSEL22 <= ALT4 */
    lsl   w3, w3, #6
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #6
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL23 <= ALT4 */
    lsl   w3, w3, #9
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #9
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL24 <= ALT4 */
    lsl   w3, w3, #12
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #12
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL25 <= ALT4 */
    lsl   w3, w3, #15
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #15
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL26 <= ALT4 */
    lsl   w3, w3, #18
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #18
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL27 <= ALT4 */
    lsl   w3, w3, #21
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #3
    lsl   w3, w3, #21
    orr   w1, w1, w3
    str   w1, [x0]
    dmb   sy
dbgjtag_gppud:
    ldr   x0, =GPIO_GPPUD
    ldr   w1, =0x00000000
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgjtag_gppud_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgjtag_gppud_loop
dbgjtag_gppudclk0:
    ldr   x0, =GPIO_GPPUDCLK0
    ldr   w1, =0x0fc00000
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgjtag_gppudclk0_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgjtag_gppudclk0_loop
dbgjtag_clear_gppudclk0:
    ldr   x0, =GPIO_GPPUDCLK0
    ldr   w1, =0x00000000
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgjtag_clear_gppudclk0_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgjtag_clear_gppudclk0_loop
dbgjtag_exit:
#endif /* DEBUG_JTAG */

#ifdef DEBUG_MU
    /*
     *  Mini Uartを有効化する
     *  config.txtにenalbe_uart=1を記載しても良い
     */
dbgmu:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    cmp   x0, xzr
    b.ne  dbgmu_exit
dbgmu_gpfsel:
    ldr   x0, =GPIO_GPFSEL1
    ldr   w1, [x0]
    mov   w3, #7            /* FSEL14 <= ALT5 */
    lsl   w3, w3, #12
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #2
    lsl   w3, w3, #12
    orr   w1, w1, w3
    mov   w3, #7            /* FSEL15 <= ALT5 */
    lsl   w3, w3, #15
    mvn   w3, w3
    and   w1, w1, w3
    mov   w3, #2
    lsl   w3, w3, #15
    orr   w1, w1, w3
    str   w1, [x0]
    dmb   sy
dbgmu_gppud:
    ldr   x0, =GPIO_GPPUD
    ldr   w1, =0x00000000
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgmu_gppud_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgmu_gppud_loop
dbgmu_gppudclk0:
    ldr   x0, =GPIO_GPPUDCLK0
    ldr   w1, =0x0fc00000
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgmu_gppudclk0_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgmu_gppudclk0_loop
dbgmu_clear_gppudclk0:
    ldr   x0, =GPIO_GPPUDCLK0
    mov   w1, #0x0
    str   w1, [x0]
    dmb   sy
    mov   w2, #150
dbgmu_clear_gppudclk0_loop:
    nop
    sub   w2, w2, #1
    cmp   w2, wzr
    bne   dbgmu_clear_gppudclk0_loop
dbgmu_regcfg:
    ldr   x0, =AUX_ENABLES
    ldr   w1, [x0]
    orr   w1, w1, #AUX_ENABLES_MU_EN_BIT
    str   w1, [x0]
    ldr   x0, =MU_BAUD
    mov   w1, #((250000000 / (115200 * 8)) - 1)
    str   w1, [x0]
    dmb   sy
dbgmu_test:
    ldr   x1, =MU_LSR
    ldr   w0, [x1]
    mov   w2, #MU_LSR_TX_EMPTY_BIT
    and   w0, w0, w2
    cmp   w0, #MU_LSR_TX_EMPTY_BIT
    b.ne  dbgmu_test
    ldr   x1, =MU_IO
    mov   w0, #0x41
    str   w0, [x1]
dbgmu_exit:
#endif /* DEBUG_MU */

#ifdef DEBUG_STUB
    /*
     *  stubを配置する
     */
dbgstub_loop:
    b     dbgstub_loop
#endif /* DEBUG_STUB */

    /*
     *  現在のELによって処理を変える
     */
    mrs   x0, CurrentEL
    and   x0, x0, #(CURRENTEL_EL_MASK)
    cmp   x0, #(CURRENTEL_EL_EL3)
    b.eq  start_el3
    cmp   x0, #(CURRENTEL_EL_EL2)
    b.eq  start_el2
    cmp   x0, #(CURRENTEL_EL_EL1)
    b.eq  start_el1

    /*
     *  EL3の処理をする
     */
start_el3:

#ifdef DEBUG_SECURE
    mov w0, #(SCR_RW_BIT | SCR_ST_BIT)
#else /* DEBUG_SECURE */
    mov w0, #(SCR_RW_BIT | SCR_NS_BIT)
#endif /* DEBUG_SECURE */
    msr scr_el3, x0
    isb

    mrs   x0, actlr_el3
    orr   x0, x0, ACTLR_CPUEC_AC_BIT
    msr   actlr_el3, x0
    isb

    // Timerの周波数を1MHzにする
//    freqency = rd_word(CNTFID0);
    ldr   w0, =1000000
    msr   cntfrq_el0, x0
    isb

drop_el2:
#ifdef DEBUG_SECURE
    adr   x0, start_el1
    msr   elr_el3, x0
    mov   x0, #(DAIF_F_BIT | DAIF_I_BIT | MODE_AARCH64_EL1H)
    msr   spsr_el3, x0
#else /* DEBUG_SECURE */
    adr   x0, start_el2
    msr   elr_el3, x0
    mov   x0, #(DAIF_F_BIT | DAIF_I_BIT | MODE_AARCH64_EL2H)
    msr   spsr_el3, x0
#endif /* DEBUG_SECURE */
    eret

    /*
     *  EL2の処理をする
     */
start_el2:

    mov x0, #(HCR_RW_BIT)
    msr hcr_el2, x0
    isb

    mrs   x0, actlr_el2
    orr   x0, x0, ACTLR_CPUEC_AC_BIT
    msr   actlr_el2, x0
    isb

    // Physical Counter, Physical TimerをEL1NSとEL0NSからアクセス可能にする
    mov   x0, #(CNTHCTL_EL1PCEN_BIT | CNTHCTL_EL1PCTEN_BIT)
    msr   cnthctl_el2, x0
    isb

    // Virtual Counterのオフセットを0にする
    msr   cntvoff_el2, xzr
    isb

drop_el1:
    adr   x0, start_el1
    msr   elr_el2, x0
    mov   x0, #(DAIF_F_BIT | DAIF_I_BIT | MODE_AARCH64_EL1H)
    msr   spsr_el2, x0
    eret

    /*
     *  EL1の処理をする
     */
start_el1:

    mrs   x0, s3_1_c15_c2_1
    orr   x0, x0, CPUECTLR_SMPEN_BIT
    msr   s3_1_c15_c2_1, x0
    isb

set_loader_bss:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    cmp   x0, xzr
    b.ne  set_loader_bss_end
set_loader_bss_core0:
    ldr   x1, =__LOADER_BSS_START__
    ldr   x2, =__LOADER_BSS_END__
    cmp   x1, x2
    b.hs  set_loader_bss_end
set_loader_bss_loop:
    str   xzr, [x1], #8
    cmp   x1, x2
    b.lo  set_loader_bss_loop
set_loader_bss_end:

set_loader_cached_bss:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    cmp   x0, xzr
    b.ne  set_loader_cached_bss_end
set_loader_cached_bss_core0:
    ldr   x1, =__LOADER_CACHED_BSS_START__
    ldr   x2, =__LOADER_CACHED_BSS_END__
    cmp   x1, x2
    b.hs  set_loader_cached_bss_end
set_loader_cached_bss_loop:
    str   xzr, [x1], #8
    cmp   x1, x2
    b.lo  set_loader_cached_bss_loop
set_loader_cached_bss_end:

//set_data:
//    mrs   x0, mpidr_el1
//    ldr   x1, =MPIDR_AFF0_MASK
//    and   x0, x0, x1
//    cmp   x0, xzr
//    b.ne  set_data_end
//set_data_core0:
//    ldr   x1, =__ROM_DATA_START__
//    ldr   x3, =__ROM_DATA_END__
//    cmp   x1, x3
//    b.hs  set_data_end
//    ldr   x2, =__DATA_START__
//set_data_loop:
//    ldr   x0, [x1], #8
//    str   x0, [x2], #8
//    cmp   x1, x3
//    b.lo  set_data_loop
//set_data_end:

    /*
     *  loader関数へジャンプする
     */
set_loader_stack:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    add   x0, x0, #1
    ldr   x1, =STACK_CORE_SIZE
    mul   x0, x1, x0
    ldr   x1, =(_loader_stack)
    add   x0, x1, x0
    mov   sp, x0
    mov   x29, xzr          // フレームポインタを初期化する
jump_to_loader:
    bl    loader

    /*
     *  loader_cachedへジャンプする
     */
set_loader_cached_stack:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    add   x0, x0, #1
    ldr   x1, =STACK_CORE_SIZE
    mul   x0, x1, x0
    ldr   x1, =(_loader_cached_stack)
    add   x0, x1, x0
    mov   sp, x0
    mov   x29, xzr          // フレームポインタを初期化する
jump_to_loader_cached:
    bl    loader_cached

    /*
     *  loader_exit関数へジャンプする
     */
set_loader_exit_stack:
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF0_MASK
    and   x0, x0, x1
    add   x0, x0, #1
    ldr   x1, =STACK_CORE_SIZE
    mul   x0, x1, x0
    ldr   x1, =(_loader_stack)
    add   x0, x1, x0
    mov   sp, x0
    mov   x29, xzr          // フレームポインタを初期化する
jump_to_loader_exit:
    bl    loader_exit

    /*
     *  ロードされたセクションにジャンプする
     */
jump_to_loaded_section:
    ldr   x0, =LOAD_ADDR
    blr   x0

panic:
    b     panic

    /*
     *  loader stack
     */
    .global	_loader_stack
    .align STACK_ALIGN
    .section .loader_stack, "aw", %nobits
_loader_stack:
    .space STACK_SIZE, 0

    /*
     *  loader cached stack
     */
    .global	_loader_cached_stack
    .align STACK_ALIGN
    .section .loader_cached_stack, "aw", %nobits
_loader_cached_stack:
    .space STACK_SIZE, 0
