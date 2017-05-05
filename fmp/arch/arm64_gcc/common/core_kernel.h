/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_kernel.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *      kernel.hのコア依存部（ARM64用）
 *
 *  このインクルードファイルは，target_kernel.h（または，そこからインク
 *  ルードされるファイル）のみからインクルードされる．他のファイルから
 *  直接インクルードしてはならない．
 */

#ifndef TOPPERS_CORE_KERNEL_H
#define TOPPERS_CORE_KERNEL_H

#include "arm64.h"

/*
 *  タイムティックの定義
 */
#define TIC_NUME  1U   /* タイムティックの周期の分子 */
#define TIC_DENO  1U   /* タイムティックの周期の分母 */

/*
 *  スタックの型
 *  ARM64では，スタックを16byte境界に配置する必要がある
 */
#define TOPPERS_STK_T  __int128

/*
 *  サポートする機能の定義
 */
#define TOPPERS_TARGET_SUPPORT_ENA_INT			/* ena_intをサポートする */
#define TOPPERS_TARGET_SUPPORT_DIS_INT			/* dis_intをサポートする */
#ifdef TOPPERS_SYSTIM_LOCAL
#define TOPPERS_TARGET_SUPPORT_GET_UTM			/* get_utmをサポートする */
#endif /* TOPPERS_SYSTIM_LOCAL */

/*
 *  Customのアイドル処理を使用
 */
//#define TOPPERS_CUSTOM_IDLE

#ifndef TOPPERS_MACRO_ONLY

#include "arm64.h"

/*
 *  例外フレーム構造体
 */
typedef struct {
	uint64_t sp;
	uint64_t x19;
	uint64_t x20;
	uint64_t x21;
	uint64_t x22;
	uint64_t x23;
	uint64_t x24;
	uint64_t x25;
	uint64_t x26;
	uint64_t x27;
	uint64_t x28;
	uint64_t x29;
#ifdef USE_GIC_CPULOCK
	uint64_t lock_flag;
	uint64_t saved_iipm;
#endif /* USE_GIC_CPULOCK */
	uint64_t ipm;
	uint64_t nest_count;
	uint64_t pc;
	uint64_t cpsr;
	uint64_t x0;
	uint64_t x1;
	uint64_t x2;
	uint64_t x3;
	uint64_t x4;
	uint64_t x5;
	uint64_t x6;
	uint64_t x7;
	uint64_t x8;
	uint64_t x9;
	uint64_t x10;
	uint64_t x11;
	uint64_t x12;
	uint64_t x13;
	uint64_t x14;
	uint64_t x15;
	uint64_t x16;
	uint64_t x17;
	uint64_t x18;
	uint64_t x30;
} exc_frame_t;

/*
 *  CPU例外からの戻り先アドレスの取得
 */
Inline uint32_t
x_get_exc_raddr(void *p_excinf)
{
	return(((exc_frame_t *)(p_excinf))->pc);
}

/*
 * CPU例外からの戻り先アドレスの設定
 */
Inline void
x_set_exc_raddr(void *p_excinf, uint32_t pc)
{
	((exc_frame_t *)(p_excinf))->pc = pc;
}

/* 性能計測用のカウンタのデータ型 */
typedef uint64_t PERFCNT;

/*
 *  パフォーマンスカウンタの初期化
 */
Inline void
x_init_pcc(void)
{
	uint32_t tmp;

	/* 全カウンターの有効化 */
	PMCR_EL0_READ(tmp);

	PMCR_EL0_WRITE(tmp | PMCR_LC_BIT | PMCR_C_BIT);

	/* パフォーマンスカウンタの有効化 */
	PMCNTENSET_EL0_READ(tmp);
	PMCNTENSET_EL0_WRITE(tmp | PMCNTENSET_C_BIT);

	INST_SYNC_BARRIER();
}

/*
 *  パフォーマンスカウンタの読み込み
 */
Inline void 
x_get_pcc(PERFCNT *p_count)
{
	PMCCNTR_EL0_READ(*p_count);
}

/*
 *  パフォーマンスカウンタのリセット
 */
Inline void
x_rst_pcc(void)
{
	uint32_t tmp;

	PMCR_EL0_READ(tmp);
	PMCR_EL0_WRITE(tmp | PMCR_C_BIT);
	INST_SYNC_BARRIER();
}

#include "target_timer.h"

/*
 *  カウンタ値のnsecへの変換
 */
Inline ulong_t
x_cnv_nsec(PERFCNT count) {
	return ((ulong_t)(count * 1000 / timer_clock));
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_KERNEL_H */
