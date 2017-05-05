/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_test.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */


/*
 *		テストプログラムのコア依存定義（ARM64用）
 */

#ifndef TOPPERS_CORE_TEST_H
#define TOPPERS_CORE_TEST_H

/*
 *  コアで共通な定義
 */
#define CPUEXC1         	(0x10000|EXCH_NO_CUR_SPX_SYNC)
#define CPUEXC2         	(0x20000|EXCH_NO_CUR_SPX_SYNC)
#define CPUEXC3         	(0x30000|EXCH_NO_CUR_SPX_SYNC)
#define CPUEXC4         	(0x40000|EXCH_NO_CUR_SPX_SYNC)
#define RAISE_CPU_EXCEPTION	(*((volatile int *) 0xFFFFFEC1U));\
								Asm("isb":::"memory") /* ロードエラー例外 */
//#define RAISE_CPU_EXCEPTION	Asm("svc #0")

// nsオーダの測定を提供する場合は下記を有効にする
//#define HISTTIM					uint64_t
//#define HIST_GET_TIM(p_time)	CNTPCT_EL0_READ(*p_time)
//#define HIST_CONV_TIM(time)		((uint64_t)(time) * 1000 * 1000 / (uint64_t)timer_clock)
//#define HIST_BM_HOOK()			cache_flush()

//#define HISTTIM  PERFCNT
//#define HIST_GET_TIM(p_time)  ((void) x_get_pcc(p_time));
//#define HIST_CONV_TIM(time)   (x_cnv_nsec(time))
//#define HIST_BM_HOOK()        (x_rst_pcc())

#endif /* TOPPERS_CORE_TEST_H */
