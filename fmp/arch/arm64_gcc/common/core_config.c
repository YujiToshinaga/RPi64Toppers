/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_config.c 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *        コア依存モジュール（ARM64用）
 */
#include "kernel_impl.h"
#include <sil.h>
#include "check.h"
#include "task.h"
#include "mp.h"
#include "target_timer.h"

/*
 *  start.S での同期用変数
 */
volatile uint_t start_sync[TNUM_PRCID];

/*
 *  str_ker() の実行前にマスタプロセッサのみ実行される初期化処理
 */
void
core_mprc_initialize(void)
{
}

/*
 *  プロセッサ依存の初期化
 */
void
core_initialize(void)
{
    uint32_t timer_clock_hz;
    uint64_t tmp;

    /*
     *  例外ベクタテーブルをVECTOR BASE ADDRESS レジスタに設定する
     */
    extern void *vector_table;
    VBAR_EL1_WRITE((uint64_t)&vector_table);

    /*
     *  アラインメントチェックの設定
     */
    SCTLR_EL1_WRITE(SCTLR_SA_BIT | SCTLR_A_BIT);
    inst_sync_barrier();

    /*
     *  タイマのクロックの取得
     *  timer_clockに代入する値の単位はkHz
     */
    CNTFRQ_EL0_READ(timer_clock_hz);
    timer_clock = timer_clock_hz / 1000;

    /*
     *  キャッシュを無効に
     */
    cache_disable();

    /*
     *  SMPモードを有効化
     */
    CPUECTLR_EL1_READ(tmp);
    tmp |= CPUECTLR_SMPEN_BIT;
    CPUECTLR_EL1_WRITE(tmp);

    /*
     *  MMUを有効に
     */
    mmu_init();

    /*
     *  キャッシュを有効に
     */
    cache_enable();
}

/*
 *  プロセッサ依存の終了処理
 */
void
core_exit(void)
{
    extern void    software_term_hook(void);
    void (*volatile fp)(void) = software_term_hook;

    /*
     *  software_term_hookへのポインタを，一旦volatile指定のあるfpに代
     *  入してから使うのは，0との比較が最適化で削除されないようにするた
     *  めである．
     */
    if (fp != 0) {
        (*fp)();
    }
}

/*
 * CPU例外ハンドラの初期化
 */
void
initialize_exception(void)
{
}

/*
 *  CPU例外の発生状況のログ出力
 *
 *  CPU例外ハンドラの中から，CPU例外情報ポインタ（p_excinf）を引数とし
 *  て呼び出すことで，CPU例外の発生状況をシステムログに出力する．
 */
#ifdef SUPPORT_XLOG_SYS

void
xlog_sys(void *p_excinf)
{
}

#endif /* SUPPORT_XLOG_SYS */

#ifndef OMIT_DEFAULT_EXC_HANDLER
/*
 * 未定義の例外が入った場合の処理
 */
void
dump_excframe(exc_frame_t *p_exc_frame)
{
    ID prc_id = ID_PRC(x_prc_index());

    syslog_3(LOG_EMERG, "Processor %d : nest_count = %d, ipm = %d.",
             prc_id, (int)p_exc_frame->nest_count, (int)p_exc_frame->ipm);
    syslog_3(LOG_EMERG, "Processor %d : pc = 0x%016lx, cpsr = 0x%08x",
             prc_id, p_exc_frame->pc, (uint32_t)p_exc_frame->cpsr);
    syslog_3(LOG_EMERG, "Processor %d : x0 = 0x%016lx, x1 = 0x%016lx",
             prc_id, p_exc_frame->x0, p_exc_frame->x1);
    syslog_3(LOG_EMERG, "Processor %d : x2 = 0x%016lx, x3 = 0x%016lx",
             prc_id, p_exc_frame->x2, p_exc_frame->x3);
    syslog_3(LOG_EMERG, "Processor %d : x4 = 0x%016lx, x5 = 0x%016lx",
             prc_id, p_exc_frame->x4, p_exc_frame->x5);
    syslog_3(LOG_EMERG, "Processor %d : x6 = 0x%016lx, x7 = 0x%016lx",
             prc_id, p_exc_frame->x6, p_exc_frame->x7);
    syslog_3(LOG_EMERG, "Processor %d : x8 = 0x%016lx, x9 = 0x%016lx",
             prc_id, p_exc_frame->x8, p_exc_frame->x9);
    syslog_3(LOG_EMERG, "Processor %d : x10 = 0x%016lx, x11 = 0x%016lx",
             prc_id, p_exc_frame->x10, p_exc_frame->x11);
    syslog_3(LOG_EMERG, "Processor %d : x12 = 0x%016lx, x13 = 0x%016lx",
             prc_id, p_exc_frame->x12, p_exc_frame->x13);
    syslog_3(LOG_EMERG, "Processor %d : x14 = 0x%016lx, x15 = 0x%016lx",
             prc_id, p_exc_frame->x14, p_exc_frame->x15);
    syslog_3(LOG_EMERG, "Processor %d : x16 = 0x%016lx, x17 = 0x%016lx",
             prc_id, p_exc_frame->x16, p_exc_frame->x17);
    syslog_3(LOG_EMERG, "Processor %d : x18 = 0x%016lx, x19 = 0x%016lx",
             prc_id, p_exc_frame->x18, p_exc_frame->x19);
    syslog_3(LOG_EMERG, "Processor %d : x20 = 0x%016lx, x21 = 0x%016lx",
             prc_id, p_exc_frame->x20, p_exc_frame->x21);
    syslog_3(LOG_EMERG, "Processor %d : x22 = 0x%016lx, x23 = 0x%016lx",
             prc_id, p_exc_frame->x22, p_exc_frame->x23);
    syslog_3(LOG_EMERG, "Processor %d : x24 = 0x%016lx, x25 = 0x%016lx",
             prc_id, p_exc_frame->x24, p_exc_frame->x25);
    syslog_3(LOG_EMERG, "Processor %d : x26 = 0x%016lx, x27 = 0x%016lx",
             prc_id, p_exc_frame->x26, p_exc_frame->x27);
    syslog_3(LOG_EMERG, "Processor %d : x28 = 0x%016lx, x29 = 0x%016lx",
             prc_id, p_exc_frame->x28, p_exc_frame->x29);
    syslog_3(LOG_EMERG, "Processor %d : x30(lr) = 0x%016lx, sp = 0x%016lx",
             prc_id, p_exc_frame->x30, p_exc_frame->sp);
}

void
default_exc_handler(void *p_excinf)
{
    ID prc_id = ID_PRC(x_prc_index());

    syslog_1(LOG_EMERG, "\nProcessor %d : Unregistered Exception occurs.", prc_id);
    dump_excframe((exc_frame_t*)p_excinf);
    target_exit();
}
#endif /* OMIT_DEFAULT_EXC_HANDLER */
