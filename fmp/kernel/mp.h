/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: mp.h 905 2012-02-27 09:01:23Z ertl-honda $
 */

/*
 *		マルチプロセッサ管理モジュール
 *		メモリ構造等で変わるマクロ等を定義
 */

#ifndef TOPPERS_MP_H
#define TOPPERS_MP_H

#include <queue.h>
#include "task.h"
#include "alarm.h"
#include "cyclic.h"

/*
 *  プロセッサIDからプロセッサINDEXへの変換
 */
#define INDEX_PRC(prcid)	((uint_t)((prcid) - TMIN_PRCID))

/*
 *  プロセッサINDEXからプロセッサIDへの変換
 */
#define ID_PRC(prcindex)	((uint_t)((prcindex) + TMIN_PRCID))

/*
 *  起動時の初期化
 */
extern void initialize_pcb(void);

/*
 *  プロセッサ間割込みハンドラ
 */
extern void ipi_handler(void);

#ifndef OMIT_KER_REQ_ON_IPI
/*
 *  終了処理要求フラグ
 */
extern bool_t ext_ker_reqflg;

/*
 *  終了処理要求
 */
extern void ext_ker_request(void);
#endif /* OMIT_KER_REQ_ON_IPI */

#ifndef OMIT_BARRIER_SYNC
/*
 *  バリア同期
 */
extern void barrier_sync(uint_t phase);
#endif  /* OMIT_BARRIER_SYNC */

/*
 *		タスク管理モジュール関連
 */

/*
 *  ディスパッチ要求
 */
extern bool_t dispatch_request(PCB* p_pcb);

/*
 *  非タスクコンテキスト用のスタック関連のデータ（kernel_cfg.c）
 */
extern const SIZE _kernel_istksz_table[];

extern STK_T *const _kernel_istk_table[];

#ifdef TOPPERS_ISTKPT
extern STK_T *const _kernel_istkpt_table[];
#endif /* TOPPERS_ISTKPT */

/*
 *		ロック関係機能
 */

#if TTYPE_KLOCK == G_KLOCK

/*
 *  ジャイアントロック
 */

/*
 *  ジャイアントロック用のロック変数
 */
extern LOCK giant_lock;

/*
 *  オブジェクトコントロールブロックからロック変数の取り出し
 */
#define GET_OBJLOCK(p_wobjcb)   (giant_lock)

/*
 *		タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（タスクコンテキスト）
 */
Inline PCB*
t_acquire_tsk_lock_prcid(ID prcid)
{
	t_acquire_lock(&giant_lock);
	return(get_mp_p_pcb(prcid));
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 *
 *  ロック取得後，自タスクのマイグレーションをチェックする方法もあるが，
 *  チェックのコストを考えると，チェックせずに一律 p_pcb を取得した方が効
 *  率がよいと判断した． 
 */
Inline PCB*
t_acquire_tsk_lock_self(void)
{
	PCB *p_pcb;

	while(true) {
		t_acquire_lock(&giant_lock);
		/* 自タスクがマイグレートした場合を考慮してp_pcbをロック取得後に取得 */
		p_pcb = get_my_p_pcb();
		if (!TSTAT_RUNNABLE(p_pcb->p_runtsk->tstat)) { 
			/* RUNNABLEでない場合 */
			x_release_lock(&giant_lock);
			t_unlock_cpu();
			t_lock_cpu();
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 *  RUNNABLEチェックなし
 * 
 *  過度状態に対応するため，自タスクの状態を変更しないシステムコールは
 *  こちらを呼び出す． 
 */
Inline PCB*
t_acquire_tsk_lock_self_without_runnable_check(void)
{
	PCB *p_pcb;

	t_acquire_lock(&giant_lock);
	/* 自タスクがマイグレートした場合を考慮してp_pcbをロック取得後に取得 */
	p_pcb = get_my_p_pcb();
	return(p_pcb);
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/2段目）
 */ 
Inline PCB*
t_acquire_nested_tsk_lock_self(LOCK *p_objlock)
{
	return(get_my_p_pcb());
}

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/1段目）
 * 
 *  対象タスクがマイグレーションする可能性があるので，ロック取得後に，p_pcbを取得する．  
 */ 
Inline  PCB*
t_acquire_tsk_lock(TCB *p_tcb)
{
	t_acquire_lock(&giant_lock);
	return(p_tcb->p_pcb);
}

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/2段目）
 */
Inline PCB*
t_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock)
{
	return(p_tcb->p_pcb);
}

/*
 *  任意のアラームハンドラのタスクロックの取得（タスクコンテキスト/1段目）
 * 
 *  対象アラームハンドラがマイグレーションする可能性があるので，ロック取得後に，
 *  p_pcbを取得する．
 */
Inline PCB*
t_acquire_tsk_lock_alm(ALMCB *p_almcb)
{
	t_acquire_lock(&giant_lock);
	return(p_almcb->p_pcb);
}

/*
 *  任意の周期ハンドラのタスクロックの取得（タスクコンテキスト/1段目）
 *  
 *  対象周期ハンドラがマイグレーションする可能性があるので，ロック取得後に，
 *  p_pcbを取得する．  
 */
Inline PCB*
t_acquire_tsk_lock_cyc(CYCCB *p_cyccb)
{
	t_acquire_lock(&giant_lock);
	return(p_cyccb->p_pcb);
}

/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/1段目）
 * 
 *  対象タスクがマイグレーションする可能性があるので，ロック取得後に，p_pcbを取得する．    
 */  
Inline void
t_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
						PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	t_acquire_lock(&giant_lock);
	*pp_srcpcb = p_tcb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
}

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 * 
 *  対象アラームハンドラがマイグレーションする可能性があるので，ロック取得後に，
 *  p_pcbを取得する．  
 */  
Inline void
t_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	t_acquire_lock(&giant_lock);
	*pp_srcpcb = p_almcb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
}

/*
 *  周期ハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 *  
 *  対象周期ハンドラがマイグレーションする可能性があるので，ロック取得後に，
 *  p_pcbを取得する．    
 */  
Inline void
t_acquire_dual_tsk_lock_cyc(CYCCB *p_cyccb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	t_acquire_lock(&giant_lock);
	*pp_srcpcb = p_cyccb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
}

/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/2段目）
 */
Inline bool_t
t_acquire_nested_dual_tsk_lock(TCB *p_tcb, ID dstprcid, LOCK *p_objlock,
							   PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	*pp_srcpcb = p_tcb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
	return(false);
}

/*
 *		非タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（非タスクコンテキスト）
 */
Inline PCB*
i_acquire_tsk_lock_prcid(ID prcid)
{
	i_acquire_lock(&giant_lock);
	return(get_mp_p_pcb(prcid));
}

/*
 *  自プロセッサのタスクロックの取得（非タスクコンテキスト/1段目）
 */ 
Inline PCB*
i_acquire_tsk_lock_self(void)
{
	i_acquire_lock(&giant_lock);
	return(get_my_p_pcb());
}

/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/1段目）
 *
 *  対象タスクがマイグレーションする可能性があるので，ロック取得後に，
 *  pp_pcbを取得する．
 */
Inline PCB*
i_acquire_tsk_lock(TCB *p_tcb)
{
	i_acquire_lock(&giant_lock);
	return(p_tcb->p_pcb);
}

/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/2段目）
 */
Inline PCB*
i_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock)
{
	return(p_tcb->p_pcb);
}

/*
 *  任意のアラームハンドラのタスクロックの取得（非タスクコンテキスト）
 *
 *  対象アラームハンドラがマイグレーションする可能性があるので，ロック取得後に，
 *  pp_pcbを取得する．
 */
Inline PCB*
i_acquire_tsk_lock_alm(ALMCB *p_almcb)
{
	i_acquire_lock(&giant_lock);
	return(p_almcb->p_pcb);
}

/*
 *  2個のタスクロックの取得（非タスクコンテキスト/1段目）
 *
 *  対象タスクがマイグレーションする可能性があるので，ロック取得後に，
 *  pp_srcpcbを取得する．  
 */  
Inline void
i_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
						PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	i_acquire_lock(&giant_lock);
	*pp_srcpcb = p_tcb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
}

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 *
 *  対象アラームハンドラがマイグレーションする可能性があるので，ロック取
 *  得後に，pp_srcpcbを取得する．  
 */  
Inline void
i_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	i_acquire_lock(&giant_lock);
	*pp_srcpcb = p_almcb->p_pcb;
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
}

/*
 *		タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 *  オブジェクトロックの取得（タスクコンテキスト）
 */
Inline void
t_acquire_obj_lock(LOCK *p_objlock)
{
	t_acquire_lock(&giant_lock);
}

/*
 *		非タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 * オブジェクトロックの取得（非タスクコンテキスト）
 */ 
Inline void
i_acquire_obj_lock(LOCK *p_objlock)
{
	t_acquire_lock(&giant_lock);
}

/*
 *		タスクロック解放関数
 */

/*
 *  タスクロックの解放（タスクコンテキスト）
 */ 
Inline void
release_tsk_lock(PCB *p_pcb)
{
	x_release_lock(&giant_lock);
}

/*
 *  タスクロックの解放（2段目）
 */ 
Inline void
release_nested_tsk_lock(PCB *p_pcb)
{
	
}

/*
 *  2個のタスクロックの解放
 */ 
Inline void
release_dual_tsk_lock(PCB *p_srcpcb,  PCB *p_dstpcb)
{
	x_release_lock(&giant_lock);
}

/*
 *		オブジェクトロック解放関数
 */

/*
 * オブジェクトロックの解放（非タスクコンテキスト）
 */ 
Inline void
release_obj_lock(LOCK *p_objlock)
{
	x_release_lock(&giant_lock);
}

/*
 *		その他のロック関数
 */

/*
 *  プリエンプションしない自タスクのタスクロックの取得
 */
Inline PCB*
acquire_tsk_lock_without_preemption_self(void)
{
	x_acquire_lock_without_preemption(&giant_lock);
	return(get_my_p_pcb());
} 
  
/*
 *  プリエンプションしないタスクロックの取得（2段目）
 */
Inline PCB*
acquire_nested_tsk_lock_without_preemption(TCB *p_tcb)
{
	return(p_tcb->p_pcb);
}

#else /* TTYPE_KLOCK != G_KLOCK */

#if TTYPE_KLOCK == P_KLOCK
#define GET_OBJLOCK(p_wobjcb)   (*((WOBJCB*)p_wobjcb)->p_wobjinib->p_obj_lock)
#else /* TTYPE_KLOCK == F_KLOCK */
#define GET_OBJLOCK(p_wobjcb)   (p_wobjcb->obj_lock)
#endif /* TTYPE_KLOCK == F_KLOCK */

/*
 *  プロセッサロックないし細粒度ロック
 */

/*
 *		タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（タスクコンテキスト）
 */
extern PCB* t_acquire_tsk_lock_prcid(ID prcid);

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 */ 
extern PCB* t_acquire_tsk_lock_self(void);

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 *  RUNNABLEチェックなし． 
 */ 
extern PCB* t_acquire_tsk_lock_self_without_runnable_check(void);

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/2段目）
 */ 
extern PCB* t_acquire_nested_tsk_lock_self(LOCK *p_objlock);

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/1段目）
 */ 
extern PCB* t_acquire_tsk_lock(TCB *p_tcb);

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/2段目）
 */ 
extern PCB* t_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock);

/*
 *  任意のアラームハンドラのタスクロックの取得（タスクコンテキスト/1段目） 
 */
extern PCB* t_acquire_tsk_lock_alm(ALMCB *p_almcb);

/*
 *  任意の周期ハンドラのタスクロックの取得（タスクコンテキスト/1段目） 
 */
extern PCB* t_acquire_tsk_lock_cyc(CYCCB *p_cyccb);

/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */  
extern void t_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
									PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/2段目）
 */
extern bool_t t_acquire_nested_dual_tsk_lock(TCB *p_tcb, ID dstprcid, LOCK *p_objlock,
										 PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */  
extern void t_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
										PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *  周期ハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */  
extern void t_acquire_dual_tsk_lock_cyc(CYCCB *p_cyccb, ID dstprcid, 
										PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *		非タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（非タスクコンテキスト）
 */
extern PCB* i_acquire_tsk_lock_prcid(ID prcid);

/*
 *  自プロセッサのタスクロックの取得（非タスクコンテキスト/1段目）
 */ 
extern PCB* i_acquire_tsk_lock_self(void);

/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/1段目）
 */
extern PCB* i_acquire_tsk_lock(TCB *p_tcb);

/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/2段目）
 */
extern PCB* i_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock);

/*
 *  任意のアラームハンドラのタスクロックの取得（非タスクコンテキスト） 
 */
extern PCB* i_acquire_tsk_lock_alm(ALMCB *p_almcb);

/*
 *  タスクの2個のタスクロックの取得（非タスクコンテキスト/1段目）
 */  
extern void i_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
									PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */  
extern void i_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
										PCB **pp_srcpcb, PCB **pp_dstpcb);

/*
 *		タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 *  オブジェクトロックの取得（タスクコンテキスト）
 */
extern void t_acquire_obj_lock(LOCK *p_objlock);

/*
 *  オブジェクトロックの解放（タスクコンテキスト）
 */
extern void t_release_obj_lock(LOCK *p_objlock);

/*
 *		非タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 * オブジェクトロックの取得（非タスクコンテキスト）
 */ 
extern void i_acquire_obj_lock(LOCK *p_objlock);

/*
 *		タスクロック解放関数
 */

/*
 *  タスクロックの解放
 */ 
extern void release_tsk_lock(PCB *p_pcb);

/*
 *  タスクロックの解放（2段目）
 */ 
extern void release_nested_tsk_lock(PCB *p_pcb);

/*
 *  2個のタスクロックの解放（タスクコンテキスト）
 */ 
extern void release_dual_tsk_lock(PCB *p_srcpcb,  PCB *p_dstpcb);

/*
 *		オブジェクトロック解放関数
 */

/*
 * オブジェクトロックの解放
 */
extern void release_obj_lock(LOCK *p_objlock);

/*
 *		その他のロック関数
 */

/*
 *  プリエンプションしない自タスクのタスクロックの取得
 */
extern PCB* acquire_tsk_lock_without_preemption_self(void);

/*
 *  プリエンプションしないタスクロックの取得（2段目）
 */
extern PCB* acquire_nested_tsk_lock_without_preemption(TCB *p_tcb);

/*
 *  プリエンプションしないタスクの2個のタスクロックの取得（タスクコンテキスト）
 */
extern void acquire_dual_tsk_lock_without_preemption(TCB *p_tcb, ID dstprcid, 
										 PCB **pp_srcpcb, PCB **pp_dstpcb);
#endif /* TTYPE_KLOCK != G_KLOCK */

#endif /* TOPPERS_MP_H */
