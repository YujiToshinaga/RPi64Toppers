/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2006-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_timer.c 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  タイマドライバ（ARMv8 Generic Timer用）
 */

#include "kernel_impl.h"
#include "target_syssvc.h"
#include "time_event.h"
#include "target_timer.h"

/*
 *  タイマの周波数を保持する変数
 *  単位はkHz
 *  target_initialize() で初期化される
 */
uint32_t timer_clock;

/*
 *  タイマの設定値
 */
uint64_t timer_cval;

/*
 *  タイマの起動処理
 */
void
target_timer_initialize(intptr_t exinf)
{
	CLOCK cyc;

    /*
     *  タイマの停止
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_CTL_EL1_WRITE((uint32_t)0);
//#else
    CNTP_CTL_EL0_WRITE((uint32_t)0);
//#endif

    /*
     *  1ティックのクロック数
     */
	cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

    /*
     *  タイマの開始
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_TVAL_EL1_WRITE((uint32_t)cyc);
//    CNTPS_CTL_EL1_WRITE((uint32_t)CNTPS_CTL_ENABLE_BIT);
//#else
    CNTP_TVAL_EL0_WRITE((uint32_t)cyc);
    CNTP_CTL_EL0_WRITE((uint32_t)CNTP_CTL_ENABLE_BIT);
//#endif

    /*
     *  タイマの設定値
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//	CNTPS_CVAL_EL1_READ(timer_cval);
//#else
	CNTP_CVAL_EL0_READ(timer_cval);
//#endif
}

/*
 *  タイマの停止処理
 */
void
target_timer_terminate(intptr_t exinf)
{
    /*
     *  タイマの停止
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_CTL_EL1_WRITE((uint32_t)0);
//#else
    CNTP_CTL_EL0_WRITE((uint32_t)0);
//#endif
}

/*
 *  タイマ割込みハンドラ
 */
void
target_timer_handler(void)
{
	ID prcid;
	CLOCK cyc;
    uint64_t pct;

    /*
     *  タイマの停止
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_CTL_EL1_WRITE((uint32_t)0);
//#else
    CNTP_CTL_EL0_WRITE((uint32_t)0);
//#endif

    /*
     *  1ティックのクロック数
     */
	cyc = TO_CLOCK(TIC_NUME, TIC_DENO);

    /*
     *  タイマの設定値
     */
	timer_cval = timer_cval + (uint64_t)cyc;

//#if 0
    /*
     *  ティック割込みを強制的に発生させた場合の補正．
     *  及び，ティック割込みが抜けた場合の補正．
	 *  この補正が行われるときは，システムが破綻しているときである．
     */
	CNTPCT_EL0_READ(pct);
	while (timer_cval > pct + cyc) {
		timer_cval = timer_cval - (uint64_t)cyc;
	}
	while (timer_cval < pct) {
		timer_cval = timer_cval + (uint64_t)cyc;
	}
//#endif

    /*
     *  タイマの設定
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//	CNTPS_CVAL_EL1_WRITE((uint64_t)timer_cval);
//#else
	CNTP_CVAL_EL0_WRITE((uint64_t)timer_cval);
//#endif

    /*
     *  タイマの開始
     */
//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_CTL_EL1_WRITE((uint32_t)CNTPS_CTL_ENABLE_BIT);
//#else
    CNTP_CTL_EL0_WRITE((uint32_t)CNTP_CTL_ENABLE_BIT);
//#endif

	iget_pid(&prcid);
	i_begin_int((0x10000 * prcid)|INTNO_TIMER);
	signal_time();				/* タイムティックの供給 */
	i_end_int((0x10000 * prcid)|INTNO_TIMER);
}
