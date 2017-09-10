/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2004 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_support.S 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *        コア依存モジュール アセンブリ言語部（ARM64用）
 */

#define TOPPERS_MACRO_ONLY
#define TOPPERS_ASM_MACRO
#define UINT_C(val)     (val)       /* uint_t型の定数を作るマクロ */
#define ULONG_C(val)    (val)       /* ulong_t型の定数を作るマクロ */
#include "kernel_impl.h"
#include "offset.h"
#include "target_asm.inc"

/*
 *  例外ベクタ
 */
    ASECTION(vector)
    AALIGN(11)
    AGLOBAL(vector_table)
ALABEL(vector_table)
    b cur_sp0_sync_handler
    ABALIGN(0x80)
    b cur_sp0_irq_handler
    ABALIGN(0x80)
    b cur_sp0_fiq_handler
    ABALIGN(0x80)
    b cur_sp0_serr_handler
    ABALIGN(0x80)
    b cur_spx_sync_handler
    ABALIGN(0x80)
    b cur_spx_irq_handler
    ABALIGN(0x80)
    b cur_spx_fiq_handler
    ABALIGN(0x80)
    b cur_spx_serr_handler
    ABALIGN(0x80)
    b l64_sync_handler
    ABALIGN(0x80)
    b l64_irq_handler
    ABALIGN(0x80)
    b l64_fiq_handler
    ABALIGN(0x80)
    b l64_serr_handler
    ABALIGN(0x80)
    b l32_sync_handler
    ABALIGN(0x80)
    b l32_irq_handler
    ABALIGN(0x80)
    b l32_fiq_handler
    ABALIGN(0x80)
    b l32_serr_handler

//#ifdef TOPPERS_SAFEG_SECURE
//    ATEXT
//    AALIGN(2)
//    AGLOBAL(atf_vector_table)
//ALABEL(atf_vector_table)
//    b atf_std_smc_entry
//    b atf_fast_smc_entry
//    b atf_cpu_on_entry
//    b atf_cpu_off_entry
//    b atf_cpu_resume_entry
//    b atf_cpu_suspend_entry
//    b atf_fiq_entry
//    b atf_system_off_entry
//    b atf_system_reset_entry
//
//    ATEXT
//    AALIGN(2)
//    AGLOBAL(atf_std_smc_entry)
//ALABEL(atf_std_smc_entry)
//    smc   0
//
//    AGLOBAL(atf_fast_smc_entry)
//ALABEL(atf_fast_smc_entry)
//    smc   0
//
//    AGLOBAL(atf_cpu_on_entry)
//ALABEL(atf_cpu_on_entry)
//    b     start
//
//    AGLOBAL(atf_cpu_off_entry)
//ALABEL(atf_cpu_off_entry)
//    mov   x0, #0x0002
//    movk  x0, #0xf200, lsl #16
//    mov   x1, xzr
//    mov   x2, xzr
//    mov   x3, xzr
//    mov   x4, xzr
//    mov   x5, xzr
//    mov   x6, xzr
//    mov   x7, xzr
//    smc   0
//
//    AGLOBAL(atf_cpu_resume_entry)
//ALABEL(atf_cpu_resume_entry)
//    mov   x0, #0x0004
//    movk  x0, #0xf200, lsl #16
//    mov   x1, xzr
//    mov   x2, xzr
//    mov   x3, xzr
//    mov   x4, xzr
//    mov   x5, xzr
//    mov   x6, xzr
//    mov   x7, xzr
//    smc   0
//
//    AGLOBAL(atf_cpu_suspend_entry)
//ALABEL(atf_cpu_suspend_entry)
//    mov   x0, #0x0003
//    movk  x0, #0xf200, lsl #16
//    mov   x1, xzr
//    mov   x2, xzr
//    mov   x3, xzr
//    mov   x4, xzr
//    mov   x5, xzr
//    mov   x6, xzr
//    mov   x7, xzr
//    smc   0
//
//    AGLOBAL(atf_fiq_entry)
//ALABEL(atf_fiq_entry)
//    b cur_spx_fiq_handler
//
//    AGLOBAL(atf_system_off_entry)
//ALABEL(atf_system_off_entry)
//    mov   x0, #0x0008
//    movk  x0, #0xf200, lsl #16
//    mov   x1, xzr
//    mov   x2, xzr
//    mov   x3, xzr
//    mov   x4, xzr
//    mov   x5, xzr
//    mov   x6, xzr
//    mov   x7, xzr
//    smc   0
//
//    AGLOBAL(atf_system_reset_entry)
//ALABEL(atf_system_reset_entry)
//    mov   x0, #0x0009
//    movk  x0, #0xf200, lsl #16
//    mov   x1, xzr
//    mov   x2, xzr
//    mov   x3, xzr
//    mov   x4, xzr
//    mov   x5, xzr
//    mov   x6, xzr
//    mov   x7, xzr
//    smc   0
//#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  タスクディスパッチャ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(dispatch)
ALABEL(dispatch)
    /*
     *  このルーチンは，タスクコンテキスト・CPUロック状態・ディスパッチ
     *  許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さ
     *  れる．
     */
    sub   sp, sp, #(16 * 6)     /* レジスタの保存 */
    stp   x19, x20, [sp, #0x00]
    stp   x21, x22, [sp, #0x10]
    stp   x23, x24, [sp, #0x20]
    stp   x25, x26, [sp, #0x30]
    stp   x27, x28, [sp, #0x40]
    stp   x29, x30, [sp, #0x50]
    my_pcb x8, x0               /* p_runtskを読み込む */
    ldr   x9, [x8, #PCB_p_runtsk]
    mov   x0, sp                /* タスクスタックを保存 */
    str   x0, [x9, #TCB_sp]
    adr   x0, dispatch_r        /* 実行再開番地を保存 */
    str   x0, [x9, #TCB_pc]
    b     dispatcher

ALABEL(dispatch_r)
    ldp   x29, x30, [sp, #0x50] /* レジスタの復帰 */
    ldp   x27, x28, [sp, #0x40]
    ldp   x25, x26, [sp, #0x30]
    ldp   x23, x24, [sp, #0x20]
    ldp   x21, x22, [sp, #0x10]
    ldp   x19, x20, [sp, #0x00]
    add   sp, sp, #(16 * 6)
    /*
     *  タスク例外処理ルーチンの起動
     *  dispatcherから呼び出されるため，TCBのアドレスはr1に入っている
     */
    ldrb  w0, [x9, #TCB_enatex]
    tst   w0, #TCB_enatex_mask
    b.eq  dispatch_r_1          /* enatex が false ならリターン */
    ldr   w0, [x9, #TCB_texptn]
    tst   w0, w0
    b.eq  dispatch_r_1          /* texptn が0ならリターン */
    my_pcb x8, x0
    ldr   w1, [x8, #PCB_ipmflg] /* ipmflgがfalseであればリターン */
    tst   w1, w1
    b.ne  dispatch_call_texrtn  /* タスク例外ルーチンの呼び出し */
ALABEL(dispatch_r_1)
    ret

    /*
     *  自タスクのマイグレーション
     *  引数(x0)にマイグレーション先のプロセッサが入っているため破壊しないこと
     */
    ATEXT
    AALIGN(2)
    AGLOBAL(dispatch_and_migrate)
ALABEL(dispatch_and_migrate)
    sub   sp, sp, #(16 * 6)     /* レジスタの保存 */
    stp   x19, x20, [sp, #0x00]
    stp   x21, x22, [sp, #0x10]
    stp   x23, x24, [sp, #0x20]
    stp   x25, x26, [sp, #0x30]
    stp   x27, x28, [sp, #0x40]
    stp   x29, x30, [sp, #0x50]
    my_pcb x8, x1               /* p_runtskを読み込む */
    ldr   x9, [x8, #PCB_p_runtsk]
    mov   x1, sp                /* タスクスタックを保存 */
    str   x1, [x9, #TCB_sp]
    adr   x1, dispatch_r        /* 実行再開番地を保存 */
    str   x1, [x9, #TCB_pc]
    ldr   x1, [x8, #PCB_istkpt]
    mov   sp, x1
    b     migrate_self          /* 自タスクのマイグレーションルーチンへ */

    /*
     *  現在のコンテキストを捨てマイグレーション
     *  引数(x0)にマイグレーション先のプロセッサが入っているため破壊しないこと
     */
    ATEXT
    AALIGN(2)
    AGLOBAL(exit_and_migrate)
ALABEL(exit_and_migrate)
    my_pcb x8, x1
    ldr   x1, [x8, #PCB_istkpt] /* 非タスクコンテキスト用スタックへ切り替え */
    mov   sp, x1
    b     exit_and_migrate_self /* 自タスクのexitとマイグレーションルーチンへ */

/*
 *  ディスパッチャの動作開始（cpu_support.S）
 */
    AGLOBAL(start_dispatch)
ALABEL(start_dispatch)
    /*
     *  このルーチンは，カーネル起動時に，すべての割込みを禁止した状態
     *  （割込みロック状態と同等）で呼び出される．また，非タスクコンテキ
     *  ストから呼び出されることを想定している．
     *
     *  dispatcherは，CPUロック状態，（モデル上の）割込み優先度マスク全
     *  解除状態，例外（割込み/CPU例外）のネスト回数0で呼び出す．
     *  target_initializeでは，（モデル上の）割込み優先度マスク全解除
     *  とし，カーネル管理外の割込みであるFIQを許可することで，
     *  CPUロック状態・（モデル上の）割込み優先度マスク全解除状態になる．
     *  また，initialize_taskでdisdspをFALSEに初期化しているため，ディ
     *  スパッチ許可状態になっている．
     *
     */
//#ifndef USE_GIC_CPULOCK
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET) /* CPUロック状態へ */
    msr   daif, x0
//#else /* USE_GIC_CPULOCK */
//    /*
//     *  prc_initializeで，lock_flagをTRUEに，saved_iipmをIIPM_ENAALLに
//     *  初期化しているため，カーネル管理外の割込みを許可することで，
//     *  CPUロック状態・（モデル上の）割込み優先度マスク全解除状態になる．
//     */
//    gic_lock x0, w1
//    mov   x0, #(DAIF_ALWAYS_SET) 
//    msr   daif, x0
//#endif /* USE_GIC_CPULOCK */
    my_pcb x8, x0               /* 例外（割込み/CPU例外）のネスト回数を0に */
    str   wzr, [x8, #PCB_excpt_nest_count]
    b     dispatcher_0

/*
 *  現在のコンテキストを捨ててディスパッチ
 */
    AGLOBAL(exit_and_dispatch)
ALABEL(exit_and_dispatch)
    /* ディスパッチャ本体（dispatcher）へ */
    my_pcb x8, x0               /* my_pcbを読み込む */

/*
 *  ディスパッチャ本体
 */
ALABEL(dispatcher)
    /*
     *  このルーチンは，タスクコンテキスト・CPUロック状態・ディスパッチ
     *  許可状態・（モデル上の）割込み優先度マスク全解除状態で呼び出さ
     *  れる．実行再開番地へもこの状態のまま分岐する．
     *
     *  すなわち，スーパーバイザーモード，IRQ禁止となっている．
     *  x8には，my_pcb へのポインタを入れる
     */
#ifdef LOG_DSP_ENTER
    ldr   x0, [x8, #PCB_p_runtsk]
    bl    log_dsp_enter
#endif /* LOG_DSP_ENTER */
ALABEL(dispatcher_0)
    my_pcb x8, x0               /* my_pcbを読み込む */
    ldr   x9, [x8, #PCB_p_schedtsk] /* p_schedtskをp_runtskに */
    str   x9, [x8, #PCB_p_runtsk]
    cmp   x9, xzr               /* p_runtskがNULLならdispatcher_1へ */
    b.eq  dispatcher_1
    ldr   x0, [x9, #TCB_sp]     /* タスクスタックを復帰 */
    mov   sp, x0
#ifdef LOG_DSP_LEAVE
    mov   x0, x9                /* p_runtskをパラメータに */
    mov   x19, x9               /* r1はスクラッチレジスタなので保存 */
    bl    log_dsp_leave
    mov   x9, x19
#endif /* LOG_DSP_LEAVE */
    ldr   x19, [x9, #TCB_pc]    /* 実行再開番地を復帰 */
    br    x19

ALABEL(dispatcher_1)
    /*
     *  CPUロック状態の解除と，非タスクコンテキスト実行状態への
     *  準備をする
     */
    ldr   x1, [x8, #PCB_istkpt] /* 非タスクコンテキストのスタックへ */
    mov   sp, x1
    mov   w19, #1
    add   x20, x8, #PCB_reqflg  /* x20 <- reqflg */
    add   x21, x8, #PCB_excpt_nest_count
ALABEL(dispatcher_2)
    /*
     *  割込みを許可し，非タスクコンテキスト実行状態とし割込みを待つ．
     *
     *  ここで非タスクコンテキスト実行状態に切り換えるのは，ここで発生
     *  する割込み処理にどのスタックを使うかという問題の解決と，割込み
     *  ハンドラ内でのタスクディスパッチの防止という2つの意味がある．
     *
     *  プロセッサを割込み待ちに移行させる処理と，割込み許可とは，不可
     *  分に行なう必要がある．
     *  これを不可分に行なわない場合，割込みを許可した直後に割込
     *  みが入り，その中でタスクが実行可能状態になると，実行すべきタス
     *  クがあるにもかかわらずプロセッサが割込み待ちになってしまう．
     *
     *  割込み待ちの間は，p_runtskをNULL（＝0）に設定しなければならな
     *  い．このように設定しないと，割込みハンドラからiget_tidを呼び出
     *  した際の動作が仕様に合致しなくなる．
     *
     *  ターゲットによっては，省電力モード等に移行するため，標準の方法と
     *  異なる手順が必要な場合がある．
     *  そのようなターゲットでは，ターゲット依存において，TOPPERS_CUSTOM_IDLE
     *  を定義し，アセンブラマクロとして，toppers_asm_custom_idle を用意
     *  すればよい．
     *
     *  なお，toppers_asm_custom_idle の記述にあたっては，次のレジスタは
     *  toppers_asm_custom_idleの前後で使用するため，
     *  toppers_asm_custom_idle 内で使用する場合は，前後で保存復帰すること．
     *  これらのレジスタは Calee saved レジスタであるため，
     *  toppers_asm_custom_idle として関数呼び出しをした場合は，呼び出した
     *  関数で自動的に保存復帰されるため，アセンブラレベルでの保存復帰は必
     *  要ない．
     *
     *  レジスタ : 内容
     *   w19     : '1'
     *   x20     : reqflgのアドレス
     *   x21     : excpt_nest_countのアドレス
     *   sp      : 非タスクコンテキスト用のスタックの先頭アドレス．
     */
    str   w19, [x21]            /* excpt_nest_count = 1 */

//#ifdef TOPPERS_CUSTOM_IDLE
//    toppers_asm_custom_idle
//#else
    mov   x0, #(DAIF_ALWAYS_SET) /* 割り込み許可 */
    msr   daif, x0
    isb                         /* シミュレータ環境ではISBを入れないと */
                                /* 割込み許可区間が生まれない */
    nop
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET) /* CPUロック */
    msr   daif, x0
//#endif /* TOPPERS_CUSTOM_IDLE */
    ldr   w22, [x20]            /* reqflgがfalseならdispatcher_2へ */
    cmp   w22, wzr
    b.eq  dispatcher_2
    str   wzr, [x20]            /* reqflgをFALSEに */
    str   wzr, [x21]            /* excpt_nest_count = 0 */
    b     dispatcher_0

/*
 *  カーネルの終了処理の呼出し
 *
 *  モードとスタックを非タスクコンテキスト用に切り替え．
 */
    AGLOBAL(call_exit_kernel)
ALABEL(call_exit_kernel)
    mov   x0, #(DAIF_ALWAYS_SET AOR DAIF_I_BIT AOR DAIF_F_BIT)
    msr   daif, x0
    my_pcb x8, x0
    ldr   x0, [x8, #PCB_istkpt] /* 非タスクコンテキスト用スタックへ切り替え */
    mov   sp, x0
    b     exit_kernel

/*
 *  タスク開始時処理
 *
 *  dispatcherから呼び出されるため，TCBのアドレスはx9に入っている
 */
    ATEXT
    AGLOBAL(start_r)
ALABEL(start_r)
    mov   x0, #(DAIF_ALWAYS_SET)/* CPUロック解除状態に */
    msr   daif, x0
//#ifdef USE_GIC_CPULOCK
//    gic_unlock_cpu x0, x1, w2
//#endif /* USE_GIC_CPULOCK */
    ldr   x30, =ext_tsk         /* 戻り番地設定 */
    ldr   x1, [x9, #TCB_p_tinib]/* p_runtsk->p_tinibをx2に  */
    ldr   x0, [x1, #TINIB_exinf]/* exinfを引数レジスタx0に  */
    ldr   x2, [x1, #TINIB_task] /* タスク起動番地にジャンプ */
    br    x2

/*
 *  ディスパッチ要求用のプロセッサ間割込みのバイパス処理
 *
 *  共通部で用意されている ipi_handler を呼び出さずにディスパッチャを
 *  呼び出す．最も外側のルーチンでない場合は，reqflgをセットする．
 *  自PCBのアドレスがx8に入っていることを想定している．
 */
#ifdef USE_IPI_DIS_HANDER_BYPASS
    AGLOBAL(ret_int_ipi_dispatch)
ALABEL(ret_int_ipi_dispatch)
//#ifdef USE_GIC_CPULOCK
//    gic_lock_cpu x0, x1, w2
//    mov   x0, #(DAIF_ALWAYS_SET)/* CPUロック解除状態に */
//    msr   daif, x0
//#endif /* USE_GIC_CPULOCK */
    ldr   w0, [x8, #PCB_excpt_nest_count] /* 戻り先がタスクコンテキストなら */
    cmp   w0, wzr                         /* ret_int_4へ */
    b.eq  ret_int_4
    mov   w0, #1                /* 戻り先が非タスクコンテキストなら */
    str   w0, [x8, #PCB_reqflg] /* reqflgをtrueにしてリターン       */
    b     ret_int_2
#endif /* USE_IPI_DIS_HANDER_BYPASS */

/*
 *  CPU例外ハンドラ出口処理
 *
 *  ret_excは，CPU例外ハンドラから戻った直後に実行するルーチンで，
 *  CPU例外ハンドラ終了後，ターゲット依存の処理を実行した後，
 *  カーネル管理の割込みを禁止，スタックをCPU例外前のスタックに，
 *  w14にCPU例外発生時に取得しているスピンロックの値を入れた状態
 *  で呼び出される．
 */
    ATEXT
    AGLOBAL(ret_exc)
ALABEL(ret_exc)
    /*
     * CPU例外発生時にスピンロックを取得していれば，スピンロック
     * の解放は行わない．
     */
    cmp   w14, wzr
    b.ne  ret_int_1

/*
 *  割込みハンドラ/CPU例外ハンドラ出口処理
 *
 *  ret_intは，割込みハンドラから戻った直後に実行するルーチンで，
 *  割込みハンドラ終了後，ターゲット依存の処理を実行した後，
 *  カーネル管理の割込みを禁止，スタックを割込み前のスタックにした
 *  状態で呼び出される．
 */
    AGLOBAL(ret_int)
ALABEL(ret_int)
    /*
     *  スピンロックを取得していれば解除する
     */
    my_pcb x8, x0               /* 引数 */
    mov   x0, x8
    bl    force_unlock_spin

ALABEL(ret_int_1)
    /*
     *  例外・割込みのネストカウント（excpt_nest_count)のデクリメント
     */
    my_pcb x8, x0
    ldr   w1, [x8, #PCB_excpt_nest_count] /* r1 <-excpt_nest_count */
    sub   w1, w1, #1
    str   w1, [x8, #PCB_excpt_nest_count]
    cmp   w1, wzr               /* 戻り先が非タスクコンテキストなら */
    b.ne  ret_int_2             /* すぐにリターン                   */

    /*
     *  reqflgをチェックする前に割込みを禁止するのは，reqflgをチェック
     *  した直後に割込みハンドラが起動され，その中でディスパッチが要求
     *  された場合に，すぐにディスパッチされないという問題が生じるため
     *  である．
     */
    ldr   w1, [x8, #PCB_reqflg] /* reqflgがtrueであればret_int_3へ */
    cmp   w1, wzr
    b.ne  ret_int_3

    AGLOBAL(ret_int_2)
ALABEL(ret_int_2)
    /*
     *  割込み処理からのリターンにより，CPUロック解除状態に移行しなければ
     *  ならない．
     *  ARMでは，CPSRのIRQビットでCPUロック解除状態とするため，単にリターン
     *  すればよい．
     */
//#ifdef USE_GIC_CPULOCK
//    /* 
//     *  割込みがネストされるとスタックサイズ問題だが，
//     *  基本的にはENABLE_NMFI と一緒に使うためネストは
//     *  1段しか発生しないため問題ない 
//     */
//    gic_unlock_cpu x0, x1, w2
//#endif /* USE_GIC_CPULOCK */
    ldp   x0, x1, [sp], #16
    msr   elr_el1, x0
    msr   spsr_el1, x1

    ldp   x18, x30, [sp, #0x90]
    ldp   x16, x17, [sp, #0x80]
    ldp   x14, x15, [sp, #0x70]
    ldp   x12, x13, [sp, #0x60]
    ldp   x10, x11, [sp, #0x50]
    ldp   x8, x9, [sp, #0x40]
    ldp   x6, x7, [sp, #0x30]
    ldp   x4, x5, [sp, #0x20]
    ldp   x2, x3, [sp, #0x10]
    ldp   x0, x1, [sp]
    add   sp, sp, #(16 * 10)
    eret

ALABEL(ret_int_3)
    /*
     *  ここでは，戻り先がタスクであり，スタックは，タスクスタックの上
     *  にスクラッチレジスタのみが保存された状態になっている．また，
     *  プロセッサは，スーパーバイザーモード・カーネル管理の割込みを禁止
     *  した状態となっている．
     */
    str   wzr, [x8, #PCB_reqflg]/* reqflgをFALSEに */

    /*
     *  CPUロック状態に移行し，割込み優先度マスクを割込み処理前の値に
     *  設定する．
     *
     *  この時点でCPUロック状態とするのは，dispatcherへ分岐する時と，
     *  call_texrtnを呼び出す時に，CPUロック状態になっている必要がある
     *  ためである．
     *
     *  CPUロック状態に関しては，ARMでは，CPSRのIRQビットでCPUロック
     *  状態を表現するため，ここでは，特に何も行わない．
     *
     *  割込み優先度マスクに関しては，ターゲット毎に操作が異なるため，
     *  割込みハンドラ終了直後にターゲット依存部で割込み処理前の値に
     *  設定する．
     */

//#ifdef USE_GIC_CPULOCK
//    gic_lock_cpu x0, x1, w2
//    mov   x0, #(DAIF_ALWAYS_SET)
//    msr   daif, x0
//#endif /* USE_GIC_CPULOCK */

ALABEL(ret_int_4)
    /*
     *  dspflgがfalseである場合と，p_runtskとp_schedtskが同じ場合には，
     *  ディスパッチを行わない．このチェックが必要なのは，タスク例外処
     *  理ルーチンの呼出しが必要な場合に，ディスパッチが必要なくても，
     *  reqflgをTRUEにするためである．
     */
    ldr   x9, [x8, #PCB_p_runtsk] /* p_runtsk をx9に読み込む */
    ldr   w1, [x8, #PCB_dspflg] /* dspflgがfalseならret_int_r_1へ */
    cmp   w1, wzr
    b.eq  ret_int_r_1

    /*
     *  ディスパッチの必要性をチェック
     */
    ldr   x1, [x8, #PCB_p_schedtsk] /* p_schedtsk をx2に読み込む */
    cmp   x9, x1                /* p_runtskとp_schedtskが同じなら */
    b.eq  ret_int_r_1           /*                    ret_int_r_1へ */
    sub   sp, sp, #(16 * 6)     /* 残りのレジスタを保存 */
    stp   x19, x20, [sp, #0x00]
    stp   x21, x22, [sp, #0x10]
    stp   x23, x24, [sp, #0x20]
    stp   x25, x26, [sp, #0x30]
    stp   x27, x28, [sp, #0x40]
    str   x29,      [sp, #0x50]
    mov   x0, sp                /* タスクスタックを保存 */
    str   x0, [x9, #TCB_sp]
    adr   x0, ret_int_r         /* 実行再開番地を保存   */
    str   x0, [x9, #TCB_pc]
    b     dispatcher

ALABEL(ret_int_r)
    ldr   x29,      [sp, #0x50] /* レジスタの復帰 */
    ldp   x27, x28, [sp, #0x40]
    ldp   x25, x26, [sp, #0x30]
    ldp   x23, x24, [sp, #0x20]
    ldp   x21, x22, [sp, #0x10]
    ldp   x19, x20, [sp, #0x00]
    add   sp, sp, #(16 * 6)
ALABEL(ret_int_r_1)
    /*
     *  enatexがtrueで，texptnが0でなく，ipmflgがtrueであれば，タスク
     *  例外処理ルーチンを呼び出す．
     *  dispatcherから呼び出されるため，TCBのアドレスはr1に入っている
     */
    ldrb  w0, [x9, #TCB_enatex]
    tst   w0, #TCB_enatex_mask
    b.eq  ret_int_r_2           /* enatex が false なら ret_int_r2 へ */
    ldr   w0, [x9,#TCB_texptn]  /* texptnをロード               */
    tst   w0, w0                /* texptn が0ならリターン       */
    b.eq  ret_int_r_2
    my_pcb x8, x0
    ldr   w0, [x8, #PCB_ipmflg] /* ipmflgがfalseであればリターン */
    tst   w0, w0
    b.eq  ret_int_r_2
    bl    dispatch_call_texrtn   /* タスク例外ルーチンの呼び出し */
ALABEL(ret_int_r_2)
    /*
     *  割込み処理からのリターンにより，CPUロック解除状態に移行するよ
     *  うにする．ARMはCPSRのIRQビットによってCPUロック状態を表してい
     *  るため，そのままリターンすればよい．
     */

//#ifdef USE_GIC_CPULOCK
//    /* 
//     * 割込みがネストされるとスタックサイズ問題だが，基本的にはENABLE_NMFI と一緒に使うためネストは1段しか発生しないため問題ない 
//     */
//    gic_unlock_cpu x0, x1, w2
//#endif /* USE_GIC_CPULOCK */

    ldp   x0, x1, [sp], #16
    msr   elr_el1, x0
    msr   spsr_el1, x1

    ldp   x18, x30, [sp, #0x90]
    ldp   x16, x17, [sp, #0x80]
    ldp   x14, x15, [sp, #0x70]
    ldp   x12, x13, [sp, #0x60]
    ldp   x10, x11, [sp, #0x50]
    ldp   x8, x9, [sp, #0x40]
    ldp   x6, x7, [sp, #0x30]
    ldp   x4, x5, [sp, #0x20]
    ldp   x2, x3, [sp, #0x10]
    ldp   x0, x1, [sp]
    add   sp, sp, #(16 * 10)
    eret

/*
 *  CPU例外ハンドラ
 *
 *  CPU例外ハンドラは，非タスクコンテキストで実行する．
 */

/*
 *  同期 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_sp0_sync_handler)
ALABEL(cur_sp0_sync_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SP0_SYNC
    b     target_exc_handler

/*
 *  IRQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_sp0_irq_handler)
ALABEL(cur_sp0_irq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SP0_IRQ
    b     target_exc_handler

/*
 *  FIQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_sp0_fiq_handler)
ALABEL(cur_sp0_fiq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SP0_FIQ
    b     target_exc_handler

/*
 *  システムエラー 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_sp0_serr_handler)
ALABEL(cur_sp0_serr_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SP0_SERR
    b     target_exc_handler

/*
 *  同期 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_spx_sync_handler)
ALABEL(cur_spx_sync_handler)
    mrs   x0, elr_el1				// TODO : デバッグ用にELRを保存中
    b cur_spx_sync_handler
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SPX_SYNC
    b     target_exc_handler

/*
 *  IRQ 例外ハンドラ
 *  IRQ例外ハンドラはCHIP依存部で定義
 */
//    ATEXT
//    AALIGN(2)
//    AGLOBAL(cur_spx_irq_handler)
//ALABEL(cur_spx_irq_handler)

#ifndef TARGET_FIQ_HANDLER
/*
 *  FIQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_spx_fiq_handler)
ALABEL(cur_spx_fiq_handler)
//#ifdef TOPPERS_SAFEG_SECURE
//    b cur_spx_irq_handler
//#else /* TOPPERS_SAFEG_SECURE */
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SPX_FIQ
    b     target_exc_handler
//#endif /* TOPPERS_SAFEG_SECURE */
#endif /* TARGET_FIQ_HANDLER */

/*
 *  システムエラー 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(cur_spx_serr_handler)
ALABEL(cur_spx_serr_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_CUR_SPX_SERR
    b     target_exc_handler

/*
 *  同期 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l64_sync_handler)
ALABEL(l64_sync_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L64_SYNC
    b     target_exc_handler

/*
 *  IRQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l64_irq_handler)
ALABEL(l64_irq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L64_IRQ
    b     target_exc_handler

/*
 *  FIQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l64_fiq_handler)
ALABEL(l64_fiq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L64_FIQ
    b     target_exc_handler

/*
 *  システムエラー 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l64_serr_handler)
ALABEL(l64_serr_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L64_SERR
    b     target_exc_handler

/*
 *  同期 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l32_sync_handler)
ALABEL(l32_sync_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L32_SYNC
    b     target_exc_handler

/*
 *  IRQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l32_irq_handler)
ALABEL(l32_irq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L32_IRQ
    b     target_exc_handler

/*
 *  FIQ 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l32_fiq_handler)
ALABEL(l32_fiq_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L32_FIQ
    b     target_exc_handler

/*
 *  システムエラー 例外ハンドラ
 */
    ATEXT
    AALIGN(2)
    AGLOBAL(l32_serr_handler)
ALABEL(l32_serr_handler)
    /*
     *  レジスタをスタックへ保存
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
    mrs   x0, elr_el1
    mrs   x1, spsr_el1
    stp   x0, x1, [sp, #-16]!

    /*
     *  割込みロック状態，CPUロック状態はCPU例外発生時の
     *  状態を継承する．
     */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET)
    msr   daif, x0

    /*
     *  例外入口処理の呼び出し
     */
    mov   x10, #EXCH_NO_L32_SERR
    b     target_exc_handler

/*
 *  微少時間待ち
 */
    AGLOBAL(_sil_dly_nse)
ALABEL(_sil_dly_nse)
    sub   x0, x0, #SIL_DLY_TIM1
    cmp   x0, xzr
    b.gt  _sil_dly_nse1
    ret
ALABEL(_sil_dly_nse1)
    sub   x0, x0, #SIL_DLY_TIM2
    cmp   x0, xzr
    b.gt  _sil_dly_nse1
    ret
