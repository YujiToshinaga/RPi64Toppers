/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2017 by Embedded and Real-Time Systems Laboratory
 *              Graduate School of Information Science, Nagoya Univ., JAPAN
 *
 *  上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
 *  ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
 *  変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
 *  (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
 *      権表示，この利用条件および下記の無保証規定が，そのままの形でソー
 *      スコード中に含まれていること．
 *  (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
 *      用できる形で再配布する場合には，再配布に伴うドキュメント（利用
 *      者マニュアルなど）に，上記の著作権表示，この利用条件および下記
 *      の無保証規定を掲載すること．
 *  (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
 *      用できない形で再配布する場合には，次のいずれかの条件を満たすこ
 *      と．
 *    (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
 *        作権表示，この利用条件および下記の無保証規定を掲載すること．
 *    (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
 *        報告すること．
 *  (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
 *      害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
 *      また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
 *      由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
 *      免責すること．
 *
 *  本ソフトウェアは，無保証で提供されているものである．上記著作権者お
 *  よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
 *  に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
 *  アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
 *  の責任を負わない．
 *
 *  @(#) $Id: start.S 1148 2016-01-08 08:15:59Z ertl-toshinaga $
 */

/*
 *  カーネル用のスタートアップモジュール(RPI-ARM64用)
 */
#define TOPPERS_MACRO_ONLY
#define UINT_C(val)     (val)       /* uint_t型の定数を作るマクロ */
#define ULONG_C(val)    (val)       /* ulong_t型の定数を作るマクロ */

#include "kernel_impl.h"
#include "target_asm.inc"

/*
 *  同期用のマジックナンバー
 */
#define MAGIC_START 0x87654321

/*
 *  startのデバッグ
 */
//#define DEBUG_CORE0
#define DEBUG_JTAG
//#define DEBUG_STUB

    ATEXT
    AALIGN(2)
    AGLOBAL(start)
ALABEL(start)

#ifdef DEBUG_CORE0
    /*
     *  コア0だけ実行
     */
ALABEL(barrier)
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF_ALL_MASK
    and   x0, x0, x1
    cmp   x0, 0x0
    b.eq  barrier_core0
ALABEL(barrier_sleep)
    wfi
    b     barrier_sleep
ALABEL(barrier_core0)
#endif /* DEBUG_CORE0 */

#ifdef DEBUG_JTAG
    /*
     *  JTAGの有効化
     */
ALABEL(jtagcfg)
    mrs   x0, mpidr_el1
    ldr   x1, =MPIDR_AFF_ALL_MASK
    and   x0, x0, x1
    cmp   x0, 0x0
    b.ne  jtagcfg_exit
ALABEL(jtagcfg_gpfsel)
    ldr   x2, =0x3f200008
    ldr   w0, [x2]
    mov   w1, #7            /* FSEL22 <= ALT4 */
    lsl   w1, w1, #6
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #6
    orr   w0, w0, w1
    mov   w1, #7            /* FSEL23 <= ALT4 */
    lsl   w1, w1, #9
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #9
    orr   w0, w0, w1
    mov   w1, #7            /* FSEL24 <= ALT4 */
    lsl   w1, w1, #12
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #12
    orr   w0, w0, w1
    mov   w1, #7            /* FSEL25 <= ALT4 */
    lsl   w1, w1, #15
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #15
    orr   w0, w0, w1
    mov   w1, #7            /* FSEL26 <= ALT4 */
    lsl   w1, w1, #18
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #18
    orr   w0, w0, w1
    mov   w1, #7            /* FSEL27 <= ALT4 */
    lsl   w1, w1, #21
    mvn   w1, w1
    and   w0, w0, w1
    mov   w1, #3
    lsl   w1, w1, #21
    orr   w0, w0, w1
    str   w0, [x2]
    dmb   sy

ALABEL(jtagcfg_gppud)
    ldr   x2, =0x3f200094
    mov   w0, #0
    str   w0, [x2]
    dmb   sy
    mov   x1, #150
ALABEL(jtagcfg_gppud_loop)
    nop
    sub   x1, x1, #1
    cmp   x1, #0
    bne   jtagcfg_gppud_loop

ALABEL(jtagcfg_gppudclk0)
    ldr   x2, =0x3f200098
    ldr   w0, =0x0FC00000
    str   w0, [x2]
    dmb   sy
    mov   x1, #150
ALABEL(jtagcfg_gppudclk0_loop)
    nop
    sub   x1, x1, #1
    cmp   x1, #0
    bne   jtagcfg_gppudclk0_loop

ALABEL(jtagcfg_clear_gppudclk0)
    ldr   x2, =0x3f200098
    mov   w0, #0
    str   w0, [x2]
    dmb   sy
    mov   x1, #150
ALABEL(jtagcfg_clear_gppudclk0_loop)
    nop
    sub   x1, x1, #1
    cmp   x1, #0
    bne   jtagcfg_clear_gppudclk0_loop
ALABEL(jtagcfg_exit)
#endif /* DEBUG_JTAG */

#ifdef DEBUG_STUB
ALABEL(stub_loop)
    b     stub_loop
#endif /* DEBUG_STUB */

    /*
     *  FIQ / IRQの禁止
     */
    mov   x0, #(DAIF_F_BIT AOR DAIF_I_BIT)
    msr   daif, x0

    /*
     *  現在のELによって処理を変える
     */
    mrs   x0, CurrentEL
    and   x0, x0, #(CURRENTEL_EL_MASK)
#ifdef INIT_MONITOR
    cmp   x0, #(CURRENTEL_EL_EL3)
    b.eq  start_el3
#endif /* INIT_MONITOR */
#ifdef INIT_HYPERVISOR
    cmp   x0, #(CURRENTEL_EL_EL2)
    b.eq  start_el2
#endif /* INIT_HYPERVISOR */
    cmp   x0, #(CURRENTEL_EL_EL1)
    b.eq  start_el1

#ifdef INIT_MONITOR
    /*
     *  EL3の処理
     */
ALABEL(start_el3)

    /*
     *  EL3の設定
     *  EL2 をAArch64に設定，セキュアタイマ，セキュリティ状態の設定
     */
//#ifdef TOPPERS_SAFEG_SECURE
//    mov w0, #(SCR_RW_BIT | SCR_ST_BIT)
//#else /* TOPPERS_SAFEG_SECURE */
    mov w0, #(SCR_RW_BIT | SCR_NS_BIT)
//#endif /* TOPPERS_SAFEG_SECURE */
    msr scr_el3, x0
    isb

    /*
     *  スタックポインタと，フレームポインタの初期化
     */
    my_istkpt x0, x1        /* FMP */
    mov   sp, x0
    mov   x29, xzr          /* フレームポインタ(r29) */

    /*
     *  target_mon_initializeの呼び出し
     *
     *  EL3で行う初期化を実行
     */
    bl    target_mon_initialize

    /*
     *  EL2へドロップ
     */
ALABEL(drop_el2)
//#ifdef TOPPERS_SAFEG_SECURE
//    adr   x0, start_el1
//    msr   elr_el3, x0
//    mov   x0, #(DAIF_F_BIT AOR DAIF_I_BIT AOR MODE_AARCH64_EL1H)
//    msr   spsr_el3, x0
//#else /* TOPPERS_SAFEG_SECURE */
    adr   x0, start_el2
    msr   elr_el3, x0
    mov   x0, #(DAIF_F_BIT AOR DAIF_I_BIT AOR MODE_AARCH64_EL2H)
    msr   spsr_el3, x0
//#endif /* TOPPERS_SAFEG_SECURE */
    eret
#endif /* INIT_MONITOR */

#ifdef INIT_HYPERVISOR
    /*
     *  EL2の処理
     */
ALABEL(start_el2)

    /*
     *  EL2の設定
     *  EL1とEL0 をAArch64に設定
     */
    mov x0, #(HCR_RW_BIT)
    msr hcr_el2, x0
    isb

    /*
     *  スタックポインタと，フレームポインタの初期化
     */
    my_istkpt x0, x1        /* FMP */
    mov   sp, x0
    mov   x29, xzr          /* フレームポインタ(r29) */

    /*
     *  target_hyp_initializeの呼び出し
     *
     *  EL2で行う初期化を実行
     */
    bl    target_hyp_initialize

    /*
     *  EL1へドロップ
     */
ALABEL(drop_el1)
    adr   x0, start_el1
    msr   elr_el2, x0
    mov   x0, #(DAIF_F_BIT AOR DAIF_I_BIT AOR MODE_AARCH64_EL1H)
    msr   spsr_el2, x0
    eret
#endif /* INIT_HYPERVISOR */

    /*
     *  EL1の処理
     */
ALABEL(start_el1)

    /*
     *  スタックポインタと，フレームポインタの初期化
     */
    my_istkpt x0, x1        /* FMP */
    mov   sp, x0
    mov   x29, xzr          /* フレームポインタ(r29) */

    /*
     *  hardware_init_hookの呼び出し （0でない場合）
     *
     *  ターゲットハードウェアに依存して必要な初期化処理がある場合
     *  は，hardware_init_hook という関数を用意する
     */
    ldr   x0, =hardware_init_hook
    cmp   x0, #0x00
    b.eq  start_1
    blr   x0

ALABEL(start_1)

    /*
     *  マスタプロセッサ以外は初期化待ち
     */
    my_core_index x0, x1
    cmp   x0, #(TOPPERS_MASTER_PRCID - 1)
    b.ne  slave_wait

    /*
     *  マスタプロセッサは変数を初期化
     */
    ldr   x0, =start_sync
    str   wzr, [x0]

#ifndef TOPPERS_OMIT_BSS_INIT
    /*
     *  bssセクションのクリア
     */
    ldr   x1, =BSS_START
    ldr   x2, =BSS_END
    cmp   x1, x2
    b.hs  start_3
ALABEL(start_2)
    str   xzr, [x1], #8
    cmp   x1, x2
    b.lo  start_2
#endif /* TOPPERS_OMIT_BSS_INIT */

ALABEL(start_3)

#ifndef TOPPERS_OMIT_DATA_INIT
    /*
     *  dataセクションの初期化(ROM化対応)
     *  IDATA_START から IDATA_END までを，DATA_START 以降に
     *  コピーする
     */
    ldr   x1, =IDATA_START
    ldr   x3, =IDATA_END
    cmp   x1, x3
    b.hs  start_5
    ldr   x2, =DATA_START
ALABEL(start_4)
    ldr   x0, [x1], #8
    str   x0, [x2], #8
    cmp   x1, x3
    b.lo  start_4
#endif /* TOPPERS_OMIT_DATA_INIT */

    /*
     *  software_init_hook を呼出し（0 でない場合）
     *
     *  ソフトウェア環境（特にライブラリ）に依存して必要な初期化処
     *  理がある場合は，software_init_hook という関数を用意すれば
     *  よい．
     */
ALABEL(start_5)
    ldr   x0, =software_init_hook
    cmp   x0, xzr
    b.eq  start_6
    blr   x0

    /*
     *  target_mprc_initialize の呼び出し
     *
     *  sta_ker 以前にマスタプロセッサのみで行う必要のある初期化を実行
     */
ALABEL(start_6)
    bl    target_mprc_initialize

    /*
     *  カーネルを起動する
     */
ALABEL(start_7)
    ldr   x0, =start_sync
    ldr   w1, =MAGIC_START
    mov   x2, xzr
ALABEL(start_8)
    str   w1, [x0, x2]
    add   x2, x2, #4
    cmp   x2, #(TNUM_PRCID * 4)
    b.ne  start_8

    /* 全コアに通知 */
    sev

    /* マスタプロセッサ用のstart_syncを初期化 */
    my_core_index x1, x3
    lsl   x1, x1, #3
    str   w2, [x0, x1]
    bl    sta_ker

    /*
     *  マスタプロセッサ以外の初期化待ちルーチン
     */
ALABEL(slave_wait)
    my_core_index x0, x1
    lsl   x0, x0, #2
    ldr   x1, =start_sync
ALABEL(slave_wait_1)
    ldr   w2, =MAGIC_START
    ldr   w3, [x1, x0]
    cmp   w3, w2
    b.ne  slave_wait_1
    str   wzr, [x1, x0]
    bl    sta_ker
