/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: startup.c 843 2011-07-15 03:17:19Z ertl-honda $
 */

/*
 *		カーネルの初期化と終了処理
 */

#include "kernel_impl.h"
#include "time_event.h"
#include <sil.h>
#include "mp.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_KER_ENTER
#define LOG_KER_ENTER()
#endif /* LOG_KER_ENTER */

#ifndef LOG_KER_LEAVE
#define LOG_KER_LEAVE()
#endif /* LOG_KER_LEAVE */

#ifndef LOG_EXT_KER_ENTER
#define LOG_EXT_KER_ENTER()
#endif /* LOG_EXT_KER_ENTER */

#ifndef LOG_EXT_KER_LEAVE
#define LOG_EXT_KER_LEAVE(ercd)
#endif /* LOG_EXT_KER_LEAVE */

#ifdef TOPPERS_sta_ker

/*
 *  カーネルの起動
 */
void
sta_ker(void)
{
	PCB *my_p_pcb;

	/*
	 *  ターゲット依存の初期化
	 */
	target_initialize();

	/*
	 *  各モジュールの初期化
	 *
	 *  タイムイベント管理モジュールは他のモジュールより先に初期化
	 *  する必要がある．
	 */
	initialize_pcb();
	initialize_tmevt();
	initialize_object();

	/*
	 *  初期化ルーチンの実行
	 *
	 *  マスタプロセッサによるの実行が終了してから，スレーブプロセッサは実
	 *  行する．
	 */
	barrier_sync(1);

	if (x_sense_mprc()) {
		call_global_inirtn();
	}

	barrier_sync(2);

	call_local_inirtn();

	/*
	 *  オブジェクトの初期化待ちのバリア同期
	 */
	barrier_sync(3);

	/*
	 *  カーネル動作の開始
	 */
	my_p_pcb = get_my_p_pcb();
	my_p_pcb->kerflg = true;
	LOG_KER_ENTER();
	start_dispatch();
	assert(0);
}

#endif /* TOPPERS_sta_ker */

/*
 *  カーネルの終了
 */
#ifdef TOPPERS_ext_ker

ER
ext_ker(void)
{
	PCB *my_p_pcb;

	SIL_PRE_LOC;

	LOG_EXT_KER_ENTER();

	/*
	 *  割込みロック状態に移行
	 */
	SIL_LOC_INT();

	my_p_pcb = get_my_p_pcb();

	/*
	 *  スピンロックを取得している場合は，スピンロックを解除する
	 */
	force_unlock_spin(my_p_pcb);

	/*
	 *  カーネル動作の終了
	 */
	LOG_KER_LEAVE();
	my_p_pcb->kerflg = false;

	/*
	 *  他のプロセッサへ終了処理を要求する
	 */
	ext_ker_request();

	/*
	 *  カーネルの終了処理の呼出し
	 *
	 *  非タスクコンテキストに切り換えて，exit_kernelを呼び出す．
	 */
	call_exit_kernel();

	/* 
	 *  コンパイラの警告対策（ここへ来ることはないはず） 
	 */ 
	SIL_UNL_INT();
	LOG_EXT_KER_LEAVE(E_SYS);
	return(E_SYS);
}

/*
 *  カーネルの終了処理
 */
void
exit_kernel(void)
{
	/*
	 *  SILスピンロックの強制解放
	 */
	TOPPERS_sil_force_unl_spn();

	barrier_sync(4);

	/*
	 *  終了処理ルーチンの実行
	 */
	call_local_terrtn();

	barrier_sync(5);

	if (x_sense_mprc()) {
		call_global_terrtn();
	}

	barrier_sync(6);

	/*
	 *  ターゲット依存の終了処理
	 */
	target_exit();
	assert(0);
}

#endif /* TOPPERS_ext_ker */
