/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_support.S 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  各レジスタの役割
 *  x8  : pcb
 *  x9  : p_runtsk
 *  x10 : 割込み要因
 *  x11 : spの保存
 *  x12 : 割込み発生前の割込み優先度
 *  x13 : 割込みハンドラのアドレス
 *  x14 : 発生した割込みの割込み優先度
 *  x15 : 例外フレームの先頭
 *  x16 : 例外発生前の割込み優先度マスク
 *  x17 : excpt_nest_count
 */

/*
 *  チップ依存モジュール アセンブリ言語部（BCM283X用）
 */

#define TOPPERS_MACRO_ONLY
#define UINT_C(val)     (val)       /* uint_t型の定数を作るマクロ */
#define ULONG_C(val)    (val)       /* ulong_t型の定数を作るマクロ */
#include "kernel_impl.h"
#include "target_asm.inc"
#include "target_ipi.h"
#include "offset.h"

/*
 *  割込みハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_spx_irq_handler)
ALABEL(cur_spx_irq_handler)
    /*
     *  スクラッチレジスタをスタックへ保存．
     */
    sub   sp, sp, #(16 * 10)
    stp   x0, x1, [sp, #0x00]
    stp   x2, x3, [sp, #0x10]
    stp   x4, x5, [sp, #0x20]
    stp   x6, x7, [sp, #0x30]
    stp   x8, x9, [sp, #0x40]
    stp   x10, x11, [sp, #0x50]
    stp   x12, x13, [sp, #0x60]
    stp   x14, x15, [sp, #0x70]
    stp   x16, x17, [sp, #0x80]
    stp   x18, x30, [sp, #0x90]

    /*
     *  elr(戻り先), spsrをスタックへ保存．
     */
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  カーネル管理の割込みのみを禁止．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  PCBの取得
     */
    my_pcb x8, x0

#if TTYPE_KLOCK != G_KLOCK
    /*
     *  プロセッサ・細粒度ロックの場合はロックを取得しているかチェックして
     *  取得していればロックを解放する．
     */
ALABEL(release_lock)
    ldr   x0, [x8, #PCB_p_firstlock] /* p_firstlock を取得 */
    cmp   x0, xzr               /* NULLなら終了       */
    b.eq  release_lock_end
    dmb sy                      /* data_memory_barrier */
    str   wzr, [x0]
    dsb sy                      /* data_sync_barrier */
    sev
    str   xzr, [x8, #PCB_p_firstlock] /* p_firstlock を NULL へ */
    ldr   x0, [x8, #PCB_p_secondlock] /* p_secondlock を取得 */
    cmp   x0, xzr               /* NULLなら終了       */
    b.eq  release_lock_end
    dmb sy                      /* data_memory_barrier */
    str   wzr, [x0]
    dsb sy                      /* data_sync_barrier */
    sev
    str   xzr, [x8, #PCB_p_firstlock] /* p_secondlock を NULL へ */
ALABEL(release_lock_end)
#endif /* TTYPE_KLOCK != G_KLOCK */

    mov   x11, sp               /* この時点のスタックを復帰のため取得 */

    /*
     *  割り込み要因の判定．
     */
ALABEL(intno_c)					/* コア割込みの判定 */
    my_core_index x0
    ldr   x1, =CORE0_IRQ_SRC
    ldr   w2, [x1, x0, lsl #2]
    mov   w10, #0
ALABEL(intno_c_loop)
    tst   w2, #0x1
    b.ne  intno_c_check
    lsr   w2, w2, #1
    add   w10, w10, #1
    cmp   w10, #32
    b.ne  intno_c_loop
ALABEL(intno_c_check)
    cmp   w10, #IRQNO_GPU
    b.eq  intno_b
    cmp   w10, #32
    b.eq  intno_b
    b     intno_exit
ALABEL(intno_b)					/* ベーシック割込みの判定 */
    mov   w10, #32
    ldr   x1, =IRQ_PEND_B
    ldr   w2, [x1]
ALABEL(intno_b_loop)
    tst   w2, #0x1
    b.ne  intno_b_check
    lsr   w2, w2, #1
    add   w10, w10, #1
    cmp   w10, #64
    b.ne  intno_b_loop
ALABEL(intno_b_check)
    cmp   w10, #IRQNO_PENDING1
    b.eq  intno_1
    cmp   w10, #IRQNO_PENDING2
    b.eq  intno_2
    cmp   w10, #64
    b.eq  intno_1
    b     intno_exit
ALABEL(intno_1)					/* 割込み1の判定 */
    mov   w10, #64
    ldr   x1, =IRQ_PEND_1
    ldr   w2, [x1]
ALABEL(intno_1_loop)
    tst   w2, #0x1
    b.ne  intno_1_check
    lsr   w2, w2, #1
    add   w10, w10, #1
    cmp   w10, #96
    b.ne  intno_1_loop
ALABEL(intno_1_check)
    cmp   w10, #96
    b.eq  intno_2
    b     intno_exit
ALABEL(intno_2)
    mov   w10, #96
    ldr   x1, =IRQ_PEND_2
    ldr   w2, [x1]
ALABEL(intno_2_loop)			/* 割込み2の判定 */
    tst   w2, #0x1
    b.ne  intno_2_check
    lsr   w2, w2, #1
    add   w10, w10, #1
    cmp   w10, #128
    b.ne  intno_2_loop
ALABEL(intno_2_check)
    cmp   w10, #128
    b.eq  intno_unknown
    b     intno_exit
ALABEL(intno_unknown)
    mov   w10, #IRQNO_UNKNOWN
ALABEL(intno_exit)

//#ifdef TOPPERS_SAFEG_SECURE
//    /*
//     *  割込みグループをサポートしている場合は(INT_ID=1022(0x03fe))も
//     *  spurious割込み
//     */
//    ldr   w0, =(0x03fe)
//    cmp   w0, w10
//    b.eq  ret_int_2
//#endif /* TOPPERS_SAFEG_SECURE */

#ifdef USE_IPI_DIS_HANDER_BYPASS
    my_core_index x0
	add   w1, x0, #IRQNO_MBOX0
    cmp   w10, w1
    b.ne  cur_spx_irq_handler_0
    ldr   x1, =CORE0_MBOX0_RC
	lsl   x2, x0, #2
	lsl   x3, x0, #4
	add   x3, x2, x3
	add   x1, x1, x3
    ldr   w4, =0xffffffff
    str   w4, [x1]
    b.ne  ret_int_ipi_dispatch
ALABEL(cur_spx_irq_handler_0)
#endif /* USE_IPI_DIS_HANDER_BYPASS */

    /*
     *  多重割込みか判定
     */
    ldr   w1, [x8, #PCB_excpt_nest_count]  /* 例外・割込みネスト回数を取得 */
    add   w2, w1, #1                       /* 例外・割込みネスト回数を更新 */
    str   w2, [x8, #PCB_excpt_nest_count]
    cmp   w1, wzr
    b.ne  cur_spx_irq_handler_1

    /*
     * 一段目の例外（割込み）ならスタックを変更する
     */
    ldr   x1, [x8, #PCB_istkpt]
	mov   sp, x1

ALABEL(cur_spx_irq_handler_1)
    /*
     *  割込みハンドラのアドレスを取得
     */
    ldr   x0, [x8, #PCB_p_inh_tbl]	/* 割込みハンドラテーブルの読み込み */
    ldr   x13, [x0, x10, lsl #3]	/* x13<-割込みハンドラ */

    /*
     *  割込み要因の割込み優先度を求め(モデル上の)割込み優先度マスクをセット
     *  する．またその際，ハンドラ実行前の(モデル上の)割込み優先度マスクを
     *  保存する．
     */
    /* TODO : 割込み優先度マスクテーブルに従って該当の割込みをdisableにする */
    ldr   x0, [x8, #PCB_p_inh_iipm_tbl]	/* 発生した割込みの優先度を取得 */
	ldr   w14, [x0, x10, lsl #2]
	ldr   x2, [x8, #PCB_iipm]	    /* 割込み発生前の割込み優先度を取得 */
	ldr   w12, [x2]              
	str   w1, [x2]			        /* 発生した割込みの優先度をiipmにセット */
//    add   x0, x14, #TNUM_INTPRI
//    lsl   x0, x0, #4		/* x16 */
//    ldr   x1, [x8, #PCB_p_iipm_mask_tbl]
//    add   x1, x1, x0

	/*
	 *  NMIであるタイマ，Mailboxの割込みをここでクリアしておく．
	 *  TODO : 割込み優先度の概念が未実装なので，個別に割込みを禁止にしておく
	 *         こうしないと，全割込み許可した瞬間に再度同じ割込みが
	 *         発生してしまう．
	 */
    cmp   w10, #IRQNO_CNTP
    b.eq  int_tim
    cmp   w10, #IRQNO_MBOX0
    b.eq  int_mbox0
    cmp   w10, #IRQNO_MBOX1
    b.eq  int_mbox1
    cmp   w10, #IRQNO_MBOX2
    b.eq  int_mbox2
    cmp   w10, #IRQNO_MBOX3
    b.eq  int_mbox3
    cmp   w10, #IRQNO_AUX
    b.eq  int_aux
    b     int_exit
int_tim:
    msr   cntp_ctl_el0, xzr
    b     int_exit
int_mbox0:
    ldr   x0, =CORE0_MBOX0_RC
    ldr   w1, =0xffffffff
    str   w1, [x0]
    b     int_exit
int_mbox1:
    ldr   x0, =CORE1_MBOX1_RC
    ldr   w1, =0xffffffff
    str   w1, [x0]
    b     int_exit
int_mbox2:
    ldr   x0, =CORE2_MBOX2_RC
    ldr   w1, =0xffffffff
    str   w1, [x0]
    b     int_exit
int_mbox3:
    ldr   x0, =CORE3_MBOX3_RC
    ldr   w1, =0xffffffff
    str   w1, [x0]
    b     int_exit
int_aux:
    ldr   x0, =DISABLE_IRQ_1
    ldr   w1, =0x20000000
    str   w1, [x0]
    b     int_exit
int_exit:

	stp   x11, x12, [sp, #-16]!
						/* "この時点"のスタックを保存 */
						/* 割込み発生前の割込み優先度マスクをスタックに保存 */
	stp   x14, x10, [sp, #-16]!
						/* TODO : 発生した割込みの割込み優先度 */
						/* inhno を保存  */

//#ifdef TOPPERS_SAFEG_LOAD
//    stp   x10, x13, [sp, #-16]!
//    bl    safeg_load_int
//    ldp   x10, x13, [sp], #16
//#endif /* TOPPERS_SAFEG_LOAD */

    /*
     *  割り込み許可
     */
#ifndef ENABLE_NMFI
	mov   x0, #(DAIF_ALWAYS_SET)
	msr   daif, x0
#endif  /* ENABLE_NMFI */

#ifdef LOG_INH_ENTER
	mov   x0, x13				/* inhno をパラメータに */
	str   x13, [sp, #-16]!
	bl    log_inh_enter			/* log_inh_enterを呼び出す */
	ldr   x13, [sp], #16
#endif /* LOG_INH_ENTER */

    /*
     *  割込みの空振りは出口へ
     */
    cmp   w10, #IRQNO_UNKNOWN
    b.eq  target_ret_int

    /*
     *  割込みハンドラの呼び出し
     */
	blr   x13

ALABEL(target_ret_int)
	/*
	 *  割込みハンドラからの戻り
	 */
	ldp   x14, x10, [sp], #16	/* GICC_IAR から取得した内容を保存  */
								/* inhno を復帰  */

//    ldr   x0, =(GICC_EOIR)  /* EOIレジスタへの書き込み */
//    str   w14, [x0]

#ifdef LOG_INH_LEAVE
	bl    log_inh_leave  /* log_inh_leaveを呼び出す */
#endif /* LOG_INH_LEAVE */

	/*
	 *  カーネル管理の割込みを禁止する
	 */
	mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET) /* CPUロック状態へ */
	msr   daif, x0

    /*
     *  PCBの取得
     */
    my_pcb x8, x0

	/*
	 *  割込み優先度マスクを元に戻す
	 */
	ldp   x11, x12, [sp], #16	/* 元のスタックポインタを取得 */
								/* 元の割込み優先度マスクを取得 */
    /* TODO : 割込み優先度マスクテーブルに従って該当の割込みをenableにする */
	ldr   x2, [x8, #PCB_iipm]	/* (モデル上の)割込み優先度マスクをセット */
	str   w12, [x2]

    /*
	 * TODO : 割込み優先度の概念が未実装なので，個別に割込みを許可にしておく
	 */
    cmp   w10, #IRQNO_AUX
    b.eq  int_aux_out
    b     int_exit_out
int_aux_out:
    ldr   x0, =ENABLE_IRQ_1
    ldr   w1, =0x20000000
    str   w1, [x0]
int_exit_out:

	/*
	 *  スタックポインタの復帰
	 */
	mov   sp, x11

	/*
	 *  後の処理はARM依存部で実行
	 */
	b     ret_int

/*
 *  ターゲット依存の例外入口処理
 */
// TODO : ターゲット依存の例外処理を実装する
    ATEXT
    AGLOBAL(target_exc_handler)
ALABEL(target_exc_handler)
    b     target_exc_handler
//    mov   x11, sp         		/* この時点のスタックを復帰のため取得 */
//
////    ldr   x0, =(GICC_PMR)		/* 例外発生前の割込み優先度マスクを取得 */
//    ldr   w16, [x0]
////    asr   w16, w16, #GICC_PMR_OFFSET /* 外部表現に変更 */
////    sub   w16, w16, #(GICC_PMR_MASK)
//
//    /*
//     *  コンテキスト判定のため，excpt_nest_count をスタックに保存．
//     *  スタックに保存せず，現在のexcpt_nest_countを-1すると取得できるが，
//     *  スタックに積んでおいた方がデバッグ等が行いやすいので，スタックに
//     *  保存する．
//     */
//    my_pcb x8, x0
//    ldr   w17, [x8, #PCB_excpt_nest_count]
//    stp   x16, x17, [sp, #-16]!
//    				/* 例外発生前の割込み優先度マスクをスタックに保存 */
//    				/* excpt_nest_count をスタックに保存．*/
//
////#ifdef USE_GIC_CPULOCK
////    ldrb  w0, [x8, #PCB_lock_flag]
////    ldrb  w1, [x8, #PCB_saved_iipm]
////    stp   x0, x1, [sp, #-16]!
////#endif /* USE_GIC_CPULOCK */
//
//    /*
//     *  デバッグのために例外発生前のSP，非破壊レジスタをスタックに保存
//     */
//    sub   sp, sp, #(16 * 6)
//    add   x0, sp, #(EXC_FRAME)
//    stp   x0, x19, [sp, #0x00]
//    stp   x20, x21, [sp, #0x10]
//    stp   x22, x23, [sp, #0x20]
//    stp   x24, x25, [sp, #0x30]
//    stp   x26, x27, [sp, #0x40]
//    stp   x28, x29, [sp, #0x50]
//
//    mov   x15,  sp         /* 例外フレーム番地を保存 */
//
//    /*
//     *  カーネル管理外のCPU例外か判定する
//     *
//     *  カーネル管理外のCPU例外は，カーネル実行中，全割込みロック状態，
//     *  CPUロック状態，カーネル管理外の割込みハンドラ実行中に発生した
//     *  CPU例外である．ARMの場合は，SPSRのI/Fフラグのいずれかが'1'なら，
//     *  該当する．
//     */
//    ldr   x0, [sp, #EXC_FRAME_cpsr] /* 例外フレームからspsrを取得 */
//    tst   w0, #(DAIF_F_BIT AOR DAIF_I_BIT)
//    b.ne   target_kernel_unc_exc_handler /* カーネル管理外のCPU例外の処理へ */
//
//    /*
//     *  例外発生時のコンテキストを判定
//     */
//    ldr   w1, [x8, #PCB_excpt_nest_count]
//    add   w1, w1, #1
//    str   w1, [x8, #PCB_excpt_nest_count]
//    cmp   w1, #1
//    b.ne  target_exc_handler_1
//
//    /*
//     *  タスクコンテキストでCPU例外が発生した場合
//     *  スタックを非タスクコンテキストに切り替える．
//     */
//    ldr   x1, [x8, #PCB_istkpt]
//    mov   sp, x1
//
//ALABEL(target_exc_handler_1)
//
//    /*
//     *  locspnidを保存
//     */
//    my_pcb x8, x0
//    ldr   w14, [x8, #PCB_locspnid]
//    str   x14, [sp, #-16]!		/* excno を保存 */
//
//    /*
//     * 割込み発生前の割込み優先度マスクをスタックに保存
//     * 割込みハンドラと出口ルーチンを共有するために保存
//     */
////    ldr   x0, =(GICC_PMR)		/* 例外発生前の割込み優先度マスクを取得 */
//    ldr   w12, [x0]
//
//    stp   x11, x12, [sp, #-16]!
//    					/* 復帰用のスタックポインタの保存 */
//    					/* 例外発生前の割込み優先度マスクをスタックに保存 */
//
//    /*
//     *  CPU例外ハンドラの起動番地を取得
//     */
//    ldr   x0, [x8, #PCB_p_exch_tbl]	/* 割込みハンドラテーブルの読み出し */
//    ldr   x13, [x0, x10, lsl #3]	/* x13<-例外ハンドラ                 */
//
//    str   x10, [sp, #-16]!		/* excno を保存 */
//
//    /*
//     *  割り込み許可
//     *  CPU管理外の例外ハンドラは別ルーチンで呼び出すため，単純にSVCモード
//     *  にするだけでよい．
//     */
//#ifndef ENABLE_NMFI
//    mov   x0, #(DAIF_ALWAYS_SET)
//    msr   daif, x0
//#endif /* ENABLE_NMFI */
//
//#ifdef LOG_EXC_ENTER
//    stp   x13, x15, [sp, #-16]!
//    mov   x0, x10      			/* excno をパラメータに */
//    bl	  log_exc_enter  		/* log_exc_enterを呼び出す */
//    ldp   x13, x15, [sp], #16
//#endif /* LOG_EXC_ENTER */
//
//    /*
//     * CPU例外ハンドラの呼び出し
//     *
//     * 例外フレームの先頭を引数として渡す
//     */
//    mov   x0, x15
//    blr   x13
//
//ALABEL(target_ret_exc)
//    ldr   x10, [sp], #16		/* excno を復帰  */
//
//#ifdef LOG_EXC_LEAVE
//    mov   x0, x10
//    bl    log_exc_leave  /* log_exc_leaveを呼び出す */
//#endif /* LOG_EXC_LEAVE */
//
//    /*
//     *  カーネル管理の割込みを禁止する
//     */
//    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
//    msr   daif, x0
//
//    /*
//     *  割込み優先度マスクを元に戻す
//     */
//    ldp   x11, x12, [sp], #16	/* 元のスタックポインタを取得 */
//    							/* 元の割込み優先度マスクを取得 */
////    ldr   x0, =(GICC_PMR)		/* GICにセット */
//    str   w12, [x0]
//
//    /*
//     * CPU例外発生時のlocspnidをr0へ
//     */
//    ldr   x14, [sp], #16		/* locspinidを取得 */
//
//    /*
//     *  スタックポインタの復帰
//     */
//    mov   sp, x11
//
//    /*
//     *  後の処理はARM依存部で実行
//     */
//    b     ret_exc
//
//
///*
// *  カーネル管理外のCPU例外の出入口処理
// */
//ALABEL(target_kernel_unc_exc_handler)
//    /*
//     *  割込み発生時のコンテキストを判定
//     */
//    ldr   w1, [x8, #PCB_excpt_nest_count]
//    add   w1, w1, #1
//    str   w1, [x8, #PCB_excpt_nest_count]
//    cmp   w1, #1
//    b.ne   target_kernel_unc_exc_handler_1
//
//    /*
//     * タスクコンテキストでCPU例外が発生した場合
//     * スタックを非タスクコンテキストに切り替える．
//     */
//    ldr   x4, [x8, #PCB_istkpt]
//    mov   sp, x4
//
//ALABEL(target_kernel_unc_exc_handler_1)
//    str   x11, [sp, #-16]!		/* 復帰用のスタックポインタの保存 */
//
//    /*
//     *  CPU例外ハンドラの起動番地を取得
//     */
//    ldr   x0, [x8, #PCB_p_exch_tbl]	/* 割込みハンドラテーブルの読み出し */
//    ldr   x13, [x0, x10, lsl #3]	/* x13<-例外ハンドラ                 */
//
//   /*
//    * システム状態（コンテキストは除く）を，CPU例外発生時の状態へ
//    */
//    ldr   x0, [x15, #EXC_FRAME_cpsr]     /* CPU例外発生前のCPSRの取得 */
//    and   x0, x0, #(DAIF_I_BIT AOR DAIF_F_BIT)
//    mov   x1, #DAIF_ALWAYS_SET
//    orr   x0, x0, x1
//    msr   daif, x0
//
//    /*
//     *  CPU例外ハンドラの呼び出し
//     *  例外フレームの先頭を引数として渡す
//     */
//    mov   x0, x15
//    blr   x13
//
//    /*
//     *  例外・割込みのネストカウント（excpt_nest_count)のデクリメント
//     */
//    my_pcb x8, x0
//    ldr   w1, [x8, #PCB_excpt_nest_count]
//    sub   w1, w1, #1
//    str   w1, [x8, #PCB_excpt_nest_count]
//
//    /*
//     *  スタックポインタの復帰
//     */
//    ldr   x11, [sp], #16			/* 元のスタックポインタを取得 */
//    mov   sp, x11
//
//    /*
//     *  CPU例外からの復帰
//     */
//    ldp   x0, x1, [sp], #16
//    msr   elr_el1, x0
//    msr   spsr_el1, x1
//
//    ldp   x18, x30, [sp, #0x90]
//    ldp   x16, x17, [sp, #0x80]
//    ldp   x14, x15, [sp, #0x70]
//    ldp   x12, x13, [sp, #0x60]
//    ldp   x10, x11, [sp, #0x50]
//    ldp   x8, x9, [sp, #0x40]
//    ldp   x6, x7, [sp, #0x30]
//    ldp   x4, x5, [sp, #0x20]
//    ldp   x2, x3, [sp, #0x10]
//    ldp   x0, x1, [sp, #0x00]
//    add   sp, sp, #(16 * 10)
//    eret
