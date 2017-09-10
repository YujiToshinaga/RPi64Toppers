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
 */
#ifndef TOPPERS_TARGET_CONFIG_H
#define TOPPERS_TARGET_CONFIG_H

/*
 *  ターゲット依存部のハードウェア資源の定義
 */
#include "rpi_arm64.h"

/*
 *  トレースログに関する設定
 */
#ifdef TOPPERS_ENABLE_TRACE
#include "logtrace/trace_config.h"
#endif /* TOPPERS_ENABLE_TRACE */

/*
 *  ipi_hanlderのバイパス処理を使用するか
 */
// TODO : 下記定義を有効にしてもビルドエラーになる
//#define USE_IPI_DIS_HANDER_BYPASS

/*
 *  スピンロック機能の方式
 */
#define TTYPE_SPN NATIVE_SPN
//#define TTYPE_SPN EMULATE_SPN

/*
 *  ネイティブスピン方式の場合のスピンロックの最大数
 */
#define TMAX_NATIVE_SPN   256

/*
 *  デフォルトの非タスクコンテキスト用のスタック領域の定義
 */
#define DEFAULT_PRC1_ISTKSZ      (1024 * 8)   /* 8KB */
#define DEFAULT_PRC2_ISTKSZ      (1024 * 8)   /* 8KB */
#define DEFAULT_PRC3_ISTKSZ      (1024 * 8)   /* 8KB */
#define DEFAULT_PRC4_ISTKSZ      (1024 * 8)   /* 8KB */

/*
 *  微少時間待ちのための定義（本来はSILのターゲット依存部）
 */
#define SIL_DLY_TIM1    6
#define SIL_DLY_TIM2    2

#ifndef TOPPERS_MACRO_ONLY

#ifdef INIT_MONITOR
/*
 *  EL3で行う初期化処理
 */
extern void target_mon_initialize(void);
#endif /* INIT_MONITOR */

#ifdef INIT_HYPERVISOR
/*
 *  EL2で行う初期化処理
 */
extern void target_hyp_initialize(void);
#endif /* INIT_HYPERVISOR */

/*
 *  メモリマップの初期化
 */
extern void target_mmu_init(void);

/*
 *  ターゲットシステム依存の初期化
 */
extern void target_initialize(void);

/*
 *  ターゲットシステムの終了
 *
 *  システムを終了する時に使う．
 */
extern void target_exit(void) NoReturn;

//#ifdef TOPPERS_SAFEG_SECURE
//extern uint64_t target_get_nt_start_address(void);
//#endif

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  チップ依存モジュール（BCM283X用）
 */
#include "arm64_gcc/bcm283x/chip_config.h"

#endif /* TOPPERS_TARGET_CONFIG_H */
