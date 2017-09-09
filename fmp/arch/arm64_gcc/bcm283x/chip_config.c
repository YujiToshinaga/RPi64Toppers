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
 *  @(#) $Id: chip_config.c 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  チップ依存モジュール（BCM283X用）
 */
#include "kernel_impl.h"
#include <sil.h>
#include "check.h"
#include "task.h"
#include "mp.h"
#include "target_timer.h"
#include "target_ipi.h"

/*
 *  SILのスピンロック用変数
 */
uint32_t TOPPERS_spn_var;

#ifdef USE_IPI_DIS_HANDER_BYPASS
/*
 *  終了処理開始フラグ
 */
static bool_t ext_ker_reqflg;
#endif /* USE_IPI_DIS_HANDER_BYPASS */

/*
 *  str_ker() の実行前にマスタプロセッサのみ実行される初期化処理
 */
void
chip_mprc_initialize(void)
{
	/*
	 *  SILのスピンロック用変数の初期化
	 */
	TOPPERS_spn_var = 0U;

#ifdef USE_IPI_DIS_HANDER_BYPASS
	/*
	 *  終了処理開始フラグの初期化
	 */
	ext_ker_reqflg = false;
#endif /* USE_IPI_DIS_HANDER_BYPASS */

	/*
	 *  プロセッサ間割込みを初期化
	 */
	sil_wrw_mem((void *)CORE0_MICTL, 0x1U);
	sil_wrw_mem((void *)CORE1_MICTL, 0x2U);
	sil_wrw_mem((void *)CORE2_MICTL, 0x4U);
	sil_wrw_mem((void *)CORE3_MICTL, 0x8U);
	sil_wrw_mem((void *)CORE0_MBOX0_RC, 0xffffffffU);
	sil_wrw_mem((void *)CORE1_MBOX1_RC, 0xffffffffU);
	sil_wrw_mem((void *)CORE2_MBOX2_RC, 0xffffffffU);
	sil_wrw_mem((void *)CORE3_MBOX3_RC, 0xffffffffU);

	/*
	 *  割込みを初期化
	 */
    sil_wrw_mem((void *)(DISABLE_IRQ_B), 0xffffffff);
    sil_wrw_mem((void *)(DISABLE_IRQ_1), 0xffffffff);
    sil_wrw_mem((void *)(DISABLE_IRQ_2), 0xffffffff);

	core_mprc_initialize();
}

/*
 *  チップ依存の初期化
 */
void
chip_initialize(void)
{
	TPCB *p_tpcb = get_my_p_tpcb();
	uint32_t index = x_prc_index();

#ifdef USE_THREAD_ID_PCB
	/*
	 *  Thread ID レジスタへのPCBへのポインタの設定
	 */
	TPIDR_EL1_WRITE((uint64_t)p_pcb_table[x_prc_index()]);
	p_tpcb = get_my_p_tpcb();
#endif /* USE_THREAD_ID_PCB */

	/*
	 *  ARM64依存の初期化
	 */
	core_initialize();

	/*
	 *  タイマを有効化
	 */
    sil_wrw_mem((void *)(CORE0_TICTL + x_prc_index() * 4),
            CORE_TICTL_CNTP_IRQ_BIT);

	/*
	 *  カーネル起動時は非タスクコンテキストとして動作させるため1に
	 */
	p_tpcb->excpt_nest_count = 1;

	/*
	 *  非タスクコンテキスト用のスタックの初期値
	 */
	p_tpcb->istkpt = istkpt_table[index];

	/*
	 *  CPU例外ハンドラテーブルへのポインタの初期化
	 */
	p_tpcb->p_exch_tbl = p_exch_table[index];

	/*
	 *  割込みハンドラテーブルへのポインタの初期化
	 */
	p_tpcb->p_inh_tbl = p_inh_table[index];

	/*
	 *  割込み優先度テーブルへのポインタの初期化
	 */
	p_tpcb->p_inh_iipm_tbl = p_inh_iipm_table[index];

	/*
	 *  割込みマスクテーブルへのポインタの初期化
	 */
	p_tpcb->p_iipm_mask_tbl = p_iipm_mask_table[index];
}

/*
 *  チップ依存の終了処理
 */
void
chip_exit(void)
{
	/*
	 *  ARM64依存の終了処理
	 */
	core_exit();
}

/*
 *  割込み要求ラインの属性の設定
 *
 *  ASPカーネルでの利用を想定して，パラメータエラーはアサーションでチェッ
 *  クしている．FI4カーネルに利用する場合には，エラーを返すようにすべき
 *  であろう．
 */
void
x_config_int(INTNO intno, ATR intatr, PRI intpri, uint_t affinity_mask)
{
	assert(VALID_INTNO_CFGINT(ID_PRC(x_prc_index()), intno));
	assert(TMIN_INTPRI <= intpri && intpri <= TMAX_INTPRI);

	/*
	 *  割込み要求のマスク
	 *
	 *  割込みを受け付けたまま，レベルトリガ／エッジトリガの設定や，割
	 *  込み優先度の設定を行うのは危険なため，割込み属性にかかわらず，
	 *  一旦マスクする．
	 */
	x_disable_int(intno);

	// TODO: 割込み優先度マスクの概念を実装する
//	/*
//	 *  属性を設定
//	 */
//	if ((intatr & TA_EDGE) != 0U) {
//		gicd_config(intno, true, true);
//		x_clear_int(intno);
//	} else {
//		gicd_config(intno, false, true);
//	}

//	/*
//	 *  割込み優先度マスクの設定
//	 */
//	gicd_set_priority(INTNO_MASK(intno), INT_IPM(intpri));    

//	/*
//	 *  ターゲットCPUの設定（グローバル割込みのみ）
//	 */
//	if (INTNO_MASK(intno) >= TMIN_GLOBAL_INTNO) {
//		gicd_set_target(INTNO_MASK(intno), x_gic_target(x_prc_index()));
//	}

	/*
	 *  割込みを許可
	 */
	if ((intatr & TA_ENAINT) != 0U) {
		(void)x_enable_int(intno);
	}
}

#ifndef OMIT_DEFAULT_INT_HANDLER
/*
 *  未定義の割込みが入った場合の処理
 */
void
default_int_handler(void){
	ID prc_id = ID_PRC(x_prc_index());
	
	syslog_1(LOG_EMERG, "Processor %d : Unregistered Interrupt occurs.", prc_id);
	target_exit();
}

#endif /* OMIT_DEFAULT_INT_HANDLER */

#ifdef USE_IPI_DIS_HANDER_BYPASS
/*
 *  カーネル終了処理要求
 */
void
ext_ker_request(void)
{
	ID prcid;

	/* すでに要求が出ていればリターン */
	if (ext_ker_reqflg) {
		return;
	}

	ext_ker_reqflg = true;

	for(prcid = 1; prcid <= TNUM_PRCID; prcid++){
		if (prcid != ID_PRC(x_prc_index())) {
			target_ipi_raise(prcid);
		}
	}
}

/*
 *  カーネル終了処理用のプロセッサ間割込みハンドラ
 */
void
ipi_ext_handler(void)
{
	ext_ker();
}
#endif /* USE_IPI_DIS_HANDER_BYPASS */

