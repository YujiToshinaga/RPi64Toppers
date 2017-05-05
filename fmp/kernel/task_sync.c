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
 *  @(#) $Id: task_sync.c 905 2012-02-27 09:01:23Z ertl-honda $
 */

/*
 *		タスク付属同期機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SLP_TSK_ENTER
#define LOG_SLP_TSK_ENTER()
#endif /* LOG_SLP_TSK_ENTER */

#ifndef LOG_SLP_TSK_LEAVE
#define LOG_SLP_TSK_LEAVE(ercd)
#endif /* LOG_SLP_TSK_LEAVE */

#ifndef LOG_TSLP_TSK_ENTER
#define LOG_TSLP_TSK_ENTER(tmout)
#endif /* LOG_TSLP_TSK_ENTER */

#ifndef LOG_TSLP_TSK_LEAVE
#define LOG_TSLP_TSK_LEAVE(ercd)
#endif /* LOG_TSLP_TSK_LEAVE */

#ifndef LOG_WUP_TSK_ENTER
#define LOG_WUP_TSK_ENTER(tskid)
#endif /* LOG_WUP_TSK_ENTER */

#ifndef LOG_WUP_TSK_LEAVE
#define LOG_WUP_TSK_LEAVE(ercd)
#endif /* LOG_WUP_TSK_LEAVE */

#ifndef LOG_IWUP_TSK_ENTER
#define LOG_IWUP_TSK_ENTER(tskid)
#endif /* LOG_IWUP_TSK_ENTER */

#ifndef LOG_IWUP_TSK_LEAVE
#define LOG_IWUP_TSK_LEAVE(ercd)
#endif /* LOG_IWUP_TSK_LEAVE */

#ifndef LOG_CAN_WUP_ENTER
#define LOG_CAN_WUP_ENTER(tskid)
#endif /* LOG_CAN_WUP_ENTER */

#ifndef LOG_CAN_WUP_LEAVE
#define LOG_CAN_WUP_LEAVE(ercd)
#endif /* LOG_CAN_WUP_LEAVE */

#ifndef LOG_REL_WAI_ENTER
#define LOG_REL_WAI_ENTER(tskid)
#endif /* LOG_REL_WAI_ENTER */

#ifndef LOG_REL_WAI_LEAVE
#define LOG_REL_WAI_LEAVE(ercd)
#endif /* LOG_REL_WAI_LEAVE */

#ifndef LOG_IREL_WAI_ENTER
#define LOG_IREL_WAI_ENTER(tskid)
#endif /* LOG_IREL_WAI_ENTER */

#ifndef LOG_IREL_WAI_LEAVE
#define LOG_IREL_WAI_LEAVE(ercd)
#endif /* LOG_IREL_WAI_LEAVE */

#ifndef LOG_SUS_TSK_ENTER
#define LOG_SUS_TSK_ENTER(tskid)
#endif /* LOG_SUS_TSK_ENTER */

#ifndef LOG_SUS_TSK_LEAVE
#define LOG_SUS_TSK_LEAVE(ercd)
#endif /* LOG_SUS_TSK_LEAVE */

#ifndef LOG_RSM_TSK_ENTER
#define LOG_RSM_TSK_ENTER(tskid)
#endif /* LOG_RSM_TSK_ENTER */

#ifndef LOG_RSM_TSK_LEAVE
#define LOG_RSM_TSK_LEAVE(ercd)
#endif /* LOG_RSM_TSK_LEAVE */

#ifndef LOG_DLY_TSK_ENTER
#define LOG_DLY_TSK_ENTER(dlytim)
#endif /* LOG_DLY_TSK_ENTER */

#ifndef LOG_DLY_TSK_LEAVE
#define LOG_DLY_TSK_LEAVE(ercd)
#endif /* LOG_DLY_TSK_LEAVE */

/*
 *  起床待ち
 */
#ifdef TOPPERS_slp_tsk

ER
slp_tsk(void)
{
	ER		ercd;
	PCB		*my_p_pcb;
	TCB		*p_runtsk;
	
	LOG_SLP_TSK_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

	my_p_pcb = t_acquire_tsk_lock_self();
	p_runtsk = my_p_pcb->p_runtsk;
	if (p_runtsk->wupque) {
		p_runtsk->wupque = false;
		release_tsk_lock(my_p_pcb);
		ercd = E_OK;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SLP);
		make_wait(p_runtsk);
		LOG_TSKSTAT(p_runtsk);
		release_tsk_lock(my_p_pcb);
		dispatch();
		ercd = p_runtsk->wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_slp_tsk */

/*
 *  起床待ち（タイムアウトあり）
 */
#ifdef TOPPERS_tslp_tsk

ER
tslp_tsk(TMO tmout)
{
	ER		ercd;
	PCB		*my_p_pcb;
	TCB		*p_runtsk;

	LOG_TSLP_TSK_ENTER(tmout);
	CHECK_TSKCTX_UNL();
	CHECK_TMOUT(tmout);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

	my_p_pcb = t_acquire_tsk_lock_self();
	p_runtsk = my_p_pcb->p_runtsk;
	if (p_runtsk->wupque) {
		p_runtsk->wupque = false;
		release_tsk_lock(my_p_pcb);
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		release_tsk_lock(my_p_pcb);
		ercd = E_TMOUT;
	}
	else {
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SLP);
		make_wait_tmout(tmout, my_p_pcb);
		LOG_TSKSTAT(p_runtsk);
		release_tsk_lock(my_p_pcb);
		dispatch();
		ercd = p_runtsk->wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TSLP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tslp_tsk */

/*
 *  タスクの起床
 */
#ifdef TOPPERS_wup_tsk

ER
wup_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_WUP_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (TSTAT_WAIT_SLP(p_tcb->tstat)) {
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->wupque)) {
		p_tcb->wupque = true;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_WUP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_wup_tsk */

/*
 *  タスクの起床（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iwup_tsk

ER
iwup_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_IWUP_TSK_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	p_pcb = i_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (TSTAT_WAIT_SLP(p_tcb->tstat)) {
		if (wait_complete(p_tcb)) {
			if (dispatch_request(p_pcb)) {
				(get_my_p_pcb())->reqflg = true;
			}
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->wupque)) {
		p_tcb->wupque = true;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IWUP_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iwup_tsk */

/*
 *  タスク起床要求のキャンセル
 */
#ifdef TOPPERS_can_wup

ER_UINT
can_wup(ID tskid)
{
	TCB		*p_tcb;
	ER_UINT	ercd;
	PCB		*p_pcb;

	LOG_CAN_WUP_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		ercd = p_tcb->wupque ? 1 : 0;
		p_tcb->wupque = false;
	}
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_CAN_WUP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_can_wup */

/*
 *  待ち状態の強制解除
 */
#ifdef TOPPERS_rel_wai

ER
rel_wai(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_REL_WAI_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (!TSTAT_WAITING(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
#if TTYPE_KLOCK == G_KLOCK
		if (wait_release(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
#else /* TTYPE_KLOCK != G_KLOCK */
		if (!TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
			/* オブジェクト待ちでない場合 */
			dspreq = wait_release(p_tcb);
		}
		else {
			/*
			 * オブジェクト待ちの場合
			 * デッドロック回避のため，ロックを取得し直す 
			 */
			WOBJCB *p_wobjcb = p_tcb->p_wobjcb;
			p_tcb->pend_relwai = true;
			release_tsk_lock(p_pcb);

			/*
			 * あらためて
			 *   オブジェクトロック -> タスクロック
			 * の順でロックを取得 
			 */
			TEST_G_LABEL("_test_rel_wai");
		  retry:
			t_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
			if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_wobjcb))) == NULL){
				goto retry;
			}

			/* タスクの状態が変化していないかチェック */
			if (!(p_tcb->pend_relwai)) {
				/* 既に他の箇所で待ち解除処理がなされた */
				release_nested_tsk_lock(p_pcb);
				release_obj_lock(&GET_OBJLOCK(p_wobjcb));
				t_unlock_cpu();
				ercd = E_OBJ;
				goto error_exit;
			}
			p_tcb->pend_relwai = false;
			dspreq = wait_release(p_tcb);
			release_obj_lock(&GET_OBJLOCK(p_wobjcb));
		}
		if (dspreq) {
			dspreq = dispatch_request(p_pcb);
		}
#endif /* TTYPE_KLOCK != G_KLOCK */
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_REL_WAI_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rel_wai */

/*
 *  待ち状態の強制解除（非タスクコンテキスト用）
 */
#ifdef TOPPERS_irel_wai

ER
irel_wai(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_IREL_WAI_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
#if TTYPE_KLOCK != G_KLOCK
  retry:
#endif /* TTYPE_KLOCK != G_KLOCK */
	p_pcb = i_acquire_tsk_lock(p_tcb);
	if (!TSTAT_WAITING(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
#if TTYPE_KLOCK == G_KLOCK
		dspreq = wait_release(p_tcb);
#else /* TTYPE_KLOCK != G_KLOCK */
		if (!TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
			/* オブジェクト待ちでない場合 */
			dspreq = wait_release(p_tcb);
		}
		else {
			/*
			 * オブジェクト待ちの場合
			 * デッドロック回避のため，ロックを取得し直す 
			 */
			WOBJCB *p_wobjcb = p_tcb->p_wobjcb;
			p_tcb->pend_relwai = true;
			release_tsk_lock(p_pcb);
			/*
			 * あらためて
			 *   オブジェクトロック -> タスクロック
			 * の順でロックを取得 
			 */
			TEST_G_LABEL("_test_irel_wai");
			t_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
			if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_wobjcb))) == NULL){
				goto retry;
			}

			/* タスクの状態が変化していないかチェック */
			if (!(p_tcb->pend_relwai)) {
				/* 既に他の箇所で待ち解除処理がなされた */
				release_nested_tsk_lock(p_pcb);
				release_obj_lock(&GET_OBJLOCK(p_wobjcb));
				t_unlock_cpu();
				ercd = E_OBJ;
				goto error_exit;
			}
			p_tcb->pend_relwai = false;
			dspreq = wait_release(p_tcb);
			release_obj_lock(&GET_OBJLOCK(p_wobjcb));
		}
#endif /* TTYPE_KLOCK != G_KLOCK */
		if (dspreq){
			if (dispatch_request(p_pcb)) {
				(get_my_p_pcb())->reqflg = true;
			}
		}
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IREL_WAI_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_irel_wai */

/*
 *  強制待ち状態への移行
 * 
 *  自タスクが対象でない場合は相手のプロセッサがディスパッチ禁止中でも
 *  発行する．
 *  自タスクが対象の場合は，他のsus_tskとレーシングすると，E_QOVR が返るこ
 *  とがある．
 */
#ifdef TOPPERS_sus_tsk

ER
sus_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;
	PCB		*my_p_pcb;

	LOG_SUS_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	my_p_pcb = get_my_p_pcb();
	if (p_tcb == my_p_pcb->p_runtsk && !(my_p_pcb->dspflg)) {
		/* 自分がディスパッチ禁止状態でsus_tskを発行するとエラー */
		ercd = E_CTX;
	}
	else if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (TSTAT_RUNNABLE(p_tcb->tstat)) {
		/*
		 *  実行できる状態から強制待ち状態への遷移
		 */
		p_tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		if (make_non_runnable(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		ercd = E_OK;
	}
	else if (TSTAT_SUSPENDED(p_tcb->tstat)) {
		ercd = E_QOVR;
	}
	else {
		/*
		 *  待ち状態から二重待ち状態への遷移
		 */
		p_tcb->tstat |= TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_SUS_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sus_tsk */

/*
 *  強制待ち状態からの再開
 */
#ifdef TOPPERS_rsm_tsk

ER
rsm_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_RSM_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (!TSTAT_SUSPENDED(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else if (!TSTAT_WAITING(p_tcb->tstat)) {
		/*
		 *  強制待ち状態から実行できる状態への遷移
		 */
		p_tcb->tstat = TS_RUNNABLE;
		LOG_TSKSTAT(p_tcb);
		if (make_runnable(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		ercd = E_OK;
	}
	else {
		/*
		 *  二重待ち状態から待ち状態への遷移
		 */
		p_tcb->tstat &= ~TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_RSM_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rsm_tsk */

/*
 *  自タスクの遅延
 */
#ifdef TOPPERS_dly_tsk

ER
dly_tsk(RELTIM dlytim)
{
	ER		ercd;
	PCB		*my_p_pcb;
	TCB		*p_runtsk;
	
	LOG_DLY_TSK_ENTER(dlytim);
	CHECK_TSKCTX_UNL();
	CHECK_PAR(dlytim <= TMAX_RELTIM);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

	my_p_pcb = t_acquire_tsk_lock_self();
	p_runtsk = my_p_pcb->p_runtsk;
	p_runtsk->tstat = (TS_WAITING | TS_WAIT_DLY);
	(void) make_non_runnable(p_runtsk);
	tmevtb_enqueue(my_p_pcb->p_tevtcb, &(p_runtsk->tmevtb), dlytim, (CBACK) wait_tmout_ok, (void *) p_runtsk);
	LOG_TSKSTAT(p_runtsk);
	release_tsk_lock(my_p_pcb);
	dispatch();
	ercd = p_runtsk->wercd;
	t_unlock_cpu();

  error_exit:
	LOG_DLY_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_dly_tsk */
