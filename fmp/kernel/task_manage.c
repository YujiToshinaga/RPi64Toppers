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
 *  @(#) $Id: task_manage.c 905 2012-02-27 09:01:23Z ertl-honda $
 */

/*
 *		タスク管理機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "spin_lock.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_MIG_TSK_ENTER
#define LOG_MIG_TSK_ENTER(tskid, prcid)
#endif  /* LOG_MIG_TSK_ENTER */

#ifndef LOG_MIG_TSK_LEAVE
#define LOG_MIG_TSK_LEAVE(ercd)
#endif  /* LOG_MIG_TSK_LEAVE */

#ifndef LOG_ACT_TSK_ENTER
#define LOG_ACT_TSK_ENTER(tskid)
#endif /* LOG_ACT_TSK_ENTER */

#ifndef LOG_ACT_TSK_LEAVE
#define LOG_ACT_TSK_LEAVE(ercd)
#endif /* LOG_ACT_TSK_LEAVE */

#ifndef LOG_IACT_TSK_ENTER
#define LOG_IACT_TSK_ENTER(tskid)
#endif /* LOG_IACT_TSK_ENTER */

#ifndef LOG_IACT_TSK_LEAVE
#define LOG_IACT_TSK_LEAVE(ercd)
#endif /* LOG_IACT_TSK_LEAVE */

#ifndef LOG_MACT_TSK_ENTER
#define LOG_MACT_TSK_ENTER(tskid, prcid)
#endif /* LOG_MACT_TSK_ENTER */

#ifndef LOG_MACT_TSK_LEAVE
#define LOG_MACT_TSK_LEAVE(ercd)
#endif /* LOG_MACT_TSK_LEAVE */

#ifndef LOG_IMACT_TSK_ENTER
#define LOG_IMACT_TSK_ENTER(tskid, prcid)
#endif /* LOG_IMACT_TSK_ENTER */

#ifndef LOG_IMACT_TSK_LEAVE
#define LOG_IMACT_TSK_LEAVE(ercd)
#endif /* LOG_IMACT_TSK_LEAVE */

#ifndef LOG_CAN_ACT_ENTER
#define LOG_CAN_ACT_ENTER(tskid)
#endif /* LOG_CAN_ACT_ENTER */

#ifndef LOG_CAN_ACT_LEAVE
#define LOG_CAN_ACT_LEAVE(ercd)
#endif /* LOG_CAN_ACT_LEAVE */

#ifndef LOG_EXT_TSK_ENTER
#define LOG_EXT_TSK_ENTER()
#endif /* LOG_EXT_TSK_ENTER */

#ifndef LOG_EXT_TSK_LEAVE
#define LOG_EXT_TSK_LEAVE(ercd)
#endif /* LOG_EXT_TSK_LEAVE */

#ifndef LOG_TER_TSK_ENTER
#define LOG_TER_TSK_ENTER(tskid)
#endif /* LOG_TER_TSK_ENTER */

#ifndef LOG_TER_TSK_LEAVE
#define LOG_TER_TSK_LEAVE(ercd)
#endif /* LOG_TER_TSK_LEAVE */

#ifndef LOG_CHG_PRI_ENTER
#define LOG_CHG_PRI_ENTER(tskid, tskpri)
#endif /* LOG_CHG_PRI_ENTER */

#ifndef LOG_CHG_PRI_LEAVE
#define LOG_CHG_PRI_LEAVE(ercd)
#endif /* LOG_CHG_PRI_LEAVE */

#ifndef LOG_GET_PRI_ENTER
#define LOG_GET_PRI_ENTER(tskid, p_tskpri)
#endif /* LOG_GET_PRI_ENTER */

#ifndef LOG_GET_PRI_LEAVE
#define LOG_GET_PRI_LEAVE(ercd, tskpri)
#endif /* LOG_GET_PRI_LEAVE */

#ifndef LOG_GET_INF_ENTER
#define LOG_GET_INF_ENTER(p_exinf)
#endif /* LOG_GET_INF_ENTER */

#ifndef LOG_GET_INF_LEAVE
#define LOG_GET_INF_LEAVE(ercd, exinf)
#endif /* LOG_GET_INF_LEAVE */

/*
 *  自プロセッサの他のタスク移動させる
 */
#ifdef TOPPERS_mig_tsk

ER
mig_tsk(ID tskid, ID prcid)
{
	TCB		*p_tcb;
	ER		ercd = E_OK;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;
	bool_t	dspreq = false;
	PCB		*my_p_pcb;
#ifdef TOPPERS_SYSTIM_LOCAL
	EVTTIM	left_time;
#endif /* TOPPERS_SYSTIM_LOCAL */

	LOG_MIG_TSK_ENTER(tskid, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_PRCID_INI(prcid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	prcid = (prcid == TPRC_INI)? p_tcb->p_tinib->iaffinity : prcid;
	T_CHECK_MIG(p_tcb->p_tinib->affinity_mask, prcid);

	/* 現在割り付けられているプロセッサと移動先のプロセッサのタスクロックを取得 */
	t_acquire_dual_tsk_lock(p_tcb, prcid, &f_p_pcb, &t_p_pcb);
	my_p_pcb = get_my_p_pcb();
	if (f_p_pcb != my_p_pcb) {
		/*
		 * 自タスクと同じプロセッサに割り付けられているタスクでなけれ
		 * ばエラー． mig_tsk を呼び出したタスクがシステムコール呼出し後，
		 * マイグレートされた場合にも，ここでエラーとなる
		 */
		ercd = E_OBJ;
	}
	else if (TSTAT_RUNNABLE(p_tcb->tstat)){
		/* 実行可能状態 */
		if(p_tcb == my_p_pcb->p_runtsk) {
			/* 自タスクに対して発行 */
			if (!(my_p_pcb->dspflg)) {
				/* ディスパッチ禁止中ならエラー */
				ercd = E_CTX;
			}
			else if (t_p_pcb == my_p_pcb) {
				/* 同一プロセッサを指定 */
				/* 優先順位を同一優先度のタスクの中で最低とする */
				dspreq = set_lowest_precedence(p_tcb, my_p_pcb);
				ercd = E_OK; 
			}
			else {
				/* マイグレーション要求を処理 */
				LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
				dispatch_and_migrate(prcid);
				/* ここに戻ってくる時にはロックは解放されている */
				ercd = E_OK;
				t_unlock_cpu();
				goto error_exit;
			}
		} 
		else {
			/* 他タスクの場合 */
			if (t_p_pcb == my_p_pcb) {
				/* 同一プロセッサを指定 */
				/* 
				 *  優先順位を同一優先度のタスクの中で最低とする．
				 *  対象のタスクは最高優先順位のタスクでないため，タ
				 *  スク切り替えは発生しない
				 */
				(void)set_lowest_precedence(p_tcb, my_p_pcb);
				ercd = E_OK; 
			}
			else {
				/* 異なるプロセッサを指定 */
				/* レディーキューから外す */
				make_non_runnable(p_tcb);
				/* pcb の書き換え */
				p_tcb->p_pcb = t_p_pcb;
				LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
				/* 移行先のプロセッサでmake_runnable する*/
				if (make_runnable(p_tcb)) {
					dispatch_request(t_p_pcb);
				}
				ercd = E_OK;
			}
		}
	}
	else if (TSTAT_DORMANT(p_tcb->tstat)) {
		/* 休止状態 */
		LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
		p_tcb->p_pcb = t_p_pcb;
		ercd = E_OK;
	}
	else {
		/* 待ち状態 */
		if ((p_tcb->tmevtb).callback == NULL) {
			/* 時間待ちでない場合 */
			LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
			p_tcb->p_pcb = t_p_pcb;
			ercd = E_OK;
		}
		else {
			/*
			 * 時間待ちの場合 グローバルタイマ方式 なら必要なし
			 */
#ifdef TOPPERS_SYSTIM_LOCAL
			/* キューから削除 */
			left_time = tmevtb_dequeue(f_p_pcb->p_tevtcb, &(p_tcb->tmevtb));
			LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
			/* 移動先のプロセッサのキューに挿入 */
			tmevtb_insert(t_p_pcb->p_tevtcb, &(p_tcb->tmevtb), base_time(t_p_pcb->p_tevtcb) + left_time);
#else  /* TOPPERS_SYSTIM_GLOBAL */
			LOG_TSKMIG(p_tcb, my_p_pcb->prcid, prcid);
#endif /* TOPPERS_SYSTIM_GLOBAL */
			p_tcb->p_pcb = t_p_pcb;
			ercd = E_OK;
		}
	}
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_MIG_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_mig_tsk */

/*
 *  タスクの起動
 */
#ifdef TOPPERS_act_tsk

ER
act_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_ACT_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		if (make_active(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
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
	LOG_ACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_act_tsk */

/*
 *  タスクの起動（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iact_tsk

ER
iact_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_IACT_TSK_ENTER(tskid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	i_lock_cpu();
	p_pcb = i_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		if (make_active(p_tcb)) {
			if (dispatch_request(p_pcb)) {
				p_pcb->reqflg = true;
			}
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iact_tsk */

/*
 *  プロセッサ指定のタスクの起動
 */
#ifdef TOPPERS_mact_tsk

ER
mact_tsk(ID tskid, ID prcid)
{
	TCB		*p_tcb;
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;
	bool_t	dspreq = false;

	LOG_MACT_TSK_ENTER(tskid, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_PRCID_INI(prcid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	prcid = (prcid == TPRC_INI)? p_tcb->p_tinib->iaffinity : prcid;
	T_CHECK_MIG(p_tcb->p_tinib->affinity_mask, prcid);

	/* 現在割り付けられているプロセッサと移動先のプロセッサのタスクロックを取得 */
	t_acquire_dual_tsk_lock(p_tcb, prcid, &f_p_pcb, &t_p_pcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		LOG_TSKMIG(p_tcb, f_p_pcb->prcid, prcid);
		p_tcb->p_pcb = t_p_pcb;
		if (make_active(p_tcb)) {
			dspreq = dispatch_request(t_p_pcb);
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
		p_tcb->actprc = prcid;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_MACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_mact_tsk */

/*
 *  プロセッサ指定のタスクの起動（非タスクコンテキスト用）
 */
#ifdef TOPPERS_imact_tsk

ER
imact_tsk(ID tskid, ID prcid)
{
	TCB		*p_tcb;
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;

	LOG_IMACT_TSK_ENTER(tskid, prcid);
	CHECK_INTCTX_UNL();
	CHECK_TSKID(tskid);
	CHECK_PRCID_INI(prcid);
	p_tcb = get_tcb(tskid);
	prcid = (prcid == TPRC_INI)? p_tcb->p_tinib->iaffinity : prcid;
	CHECK_MIG(p_tcb->p_tinib->affinity_mask, prcid);

	i_lock_cpu();
	/* 現在割り付けられているプロセッサと移動先のプロセッサのタスクロックを取得 */
	i_acquire_dual_tsk_lock(p_tcb, prcid, &f_p_pcb, &t_p_pcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		LOG_TSKMIG(p_tcb, f_p_pcb->prcid, prcid);
		p_tcb->p_pcb = t_p_pcb;
		if (make_active(p_tcb)) {
			if (dispatch_request(t_p_pcb)) {
				t_p_pcb->reqflg = true;
			}
		}
		ercd = E_OK;
	}
	else if (!(p_tcb->actque)) {
		p_tcb->actque = true;
		p_tcb->actprc = prcid;
		ercd = E_OK;
	}
	else {
		ercd = E_QOVR;
	}
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IMACT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_imact_tsk */

/*
 *  タスク起動要求のキャンセル
 */
#ifdef TOPPERS_can_act

ER_UINT
can_act(ID tskid)
{
	TCB		*p_tcb;
	ER_UINT	ercd;
	PCB		*p_pcb;

	LOG_CAN_ACT_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	ercd = p_tcb->actque ? 1 : 0;
	p_tcb->actque = false;
	p_tcb->actprc = TPRC_NONE;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_CAN_ACT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_can_act */

/*
 *  自タスクの終了
 */
#ifdef TOPPERS_ext_tsk

#if TTYPE_KLOCK == G_KLOCK

ER
ext_tsk(void)
{
	ER		ercd;
	PCB		*my_p_pcb;
	TCB		*p_runtsk;

	LOG_EXT_TSK_ENTER();
	CHECK_TSKCTX();

	/*
	 *  CPUロック状態でext_tskが呼ばれた場合は，ロック取得時に割込みを許可
	 *  しない.
	 *  ロック取得時に割込みを許可しないのは， 過度状態の場合にロックを取得
	 *  できなかった場合に割込みを許可すると，割込みが入りサスペンド状態と
	 *  なってしまうためである．
	 *  ディスパッチ禁止や割込み優先度マスク全解除状態でない場合の過度状態の
	 *  場合は，割込みが入っても割込みの出口でディスパッチャに行かずに戻って
	 *  くるため問題ない．
	 */
	if (t_sense_lock()) {
		my_p_pcb = acquire_tsk_lock_without_preemption_self();
	}
	else {
		t_lock_cpu();
		my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	}

	p_runtsk = my_p_pcb->p_runtsk;
	if (my_p_pcb->disdsp) {
		/*
		 *  ディスパッチ禁止状態でext_tskが呼ばれた場合に対応して，ディスパッ
		 *  チ許可状態にしてからタスクを終了する．
		 */
		my_p_pcb->disdsp = false;
	}
	if (!my_p_pcb->ipmflg) {
	/*
	 *  割込み優先度マスク（IPM）がTIPM_ENAALL以外の状態でext_tskが呼ばれ
	 *  た場合に対応して，IPMをTIPM_ENAALLにしてからタスクを終了する．
	 */
		t_set_ipm(TIPM_ENAALL);
		my_p_pcb->ipmflg = true;
	}
	my_p_pcb->dspflg = true;
	/*
	 *  スピンロックを取得している場合は，スピンロックを解除する
	 */
	force_unlock_spin(my_p_pcb);

	/* 再起動時のマイグレーションなし */
	if ((p_runtsk->actprc == TPRC_NONE) || (p_runtsk->actprc == my_p_pcb->prcid)) {
		(void) make_non_runnable(p_runtsk);
		make_dormant(p_runtsk);
		if (p_runtsk->actque) {
			p_runtsk->actque = false;
			p_runtsk->actprc = TPRC_NONE;
			(void) make_active(p_runtsk);
		}
		release_tsk_lock(my_p_pcb);
		exit_and_dispatch();
		ercd = E_SYS;
	}
	else {
		/* 現在コンテキストを捨ててマイグレーション */
		exit_and_migrate(p_runtsk->actprc);
		ercd = E_SYS;
	}

  error_exit:
	LOG_EXT_TSK_LEAVE(ercd);
	return(ercd);
}

#else /* TTYPE_KLOCK != G_KLOCK */


ER
ext_tsk(void)
{
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*my_p_pcb;
	TCB		*p_runtsk;
	bool_t	locked;

	LOG_EXT_TSK_ENTER();
	CHECK_TSKCTX();

	/*
	 *  CPUロック状態でext_tskが呼ばれた場合は，ロック取得時に割込みを許可
	 *  しない.
	 *  ロック取得時に割込みを許可しないのは， 過度状態の場合にロックを取得
	 *  できなかった場合に割込みを許可すると，割込みが入りサスペンド状態と
	 *  なってしまうためである． 
	 */
	locked = t_sense_lock();
  retry:
	if (locked) {
		my_p_pcb = acquire_tsk_lock_without_preemption_self();
	}
	else {
		t_lock_cpu();
		my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	}

	p_runtsk = my_p_pcb->p_runtsk;

	/* 再起動時のマイグレーションなし */
	if ((p_runtsk->actprc == TPRC_NONE) || (p_runtsk->actprc == my_p_pcb->prcid)) {
		/*
		 *  ディスパッチ禁止状態でext_tskが呼ばれた場合に対応して，ディスパッ
		 *  チ許可状態にしてからタスクを終了する．
		 */
		my_p_pcb->disdsp = false;
		/*
		 *  割込み優先度マスク（IPM）がTIPM_ENAALL以外の状態でext_tskが呼ばれ
		 *  た場合に対応して，IPMをTIPM_ENAALLにしてからタスクを終了する．
		 */
		t_set_ipm(TIPM_ENAALL);
		my_p_pcb->ipmflg = true;
		my_p_pcb->dspflg = true;
		/*
		 *  スピンロックを取得している場合は，スピンロックを解除する
		 */
		force_unlock_spin(my_p_pcb);

		(void) make_non_runnable(p_runtsk);
		make_dormant(p_runtsk);
		if (p_runtsk->actque) {
			p_runtsk->actque = false;
			p_runtsk->actprc = TPRC_NONE;
			(void) make_active(p_runtsk);
		}
		release_tsk_lock(my_p_pcb);
		exit_and_dispatch();
		ercd = E_SYS;
	}
	else {
		/* 移動先のプロセッサのPCBを取得 */
		t_p_pcb = get_mp_p_pcb(p_runtsk->actprc);
		/* 一旦タスクロックを離す */
		release_tsk_lock(my_p_pcb);

		/*
		 * 現在割り付けられているプロセッサと移動先のプロセッサのタスク
		 * ロックを取得
		 */
		if (locked) {
			acquire_dual_tsk_lock_without_preemption(p_runtsk, p_runtsk->actprc, &my_p_pcb, &t_p_pcb);
		}
		else {
			t_acquire_dual_tsk_lock(p_runtsk, p_runtsk->actprc, &my_p_pcb, &t_p_pcb);
		}
		/*
		 * マイグレーション先のプロセッサが変更された場合はリトライ
		 */
		if (p_runtsk->actprc != t_p_pcb->prcid) {
			release_dual_tsk_lock(p_runtsk->p_pcb, t_p_pcb);
			goto retry;
		}

		/*
		 *  ここで各カーネル状態を変更するのは，ディスパッチ禁止状態と割込
		 *  み優先度マスク全解除状態でない状態による過度状態の場合に，各カ
		 *  ーネル状態を変更してから，ロック取得関数で割込みを許可して割込
		 *  みが入ると，割込みからのリターン時にディスパッチャが呼ばれてし
		 *  まい，休止状態となるためである． 
		 */
		my_p_pcb->disdsp = false;
		t_set_ipm(TIPM_ENAALL);
		my_p_pcb->ipmflg = true;
		my_p_pcb->dspflg = true;
		force_unlock_spin(my_p_pcb);

		/* 現在コンテキストを捨ててマイグレーション */
		exit_and_migrate(p_runtsk->actprc);
		ercd = E_SYS;
	}

  error_exit:
	LOG_EXT_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TTYPE_KLOCK != G_KLOCK */

#endif /* TOPPERS_ext_tsk */

/*
 *  タスクの強制終了
 */
#ifdef TOPPERS_ter_tsk

#if TTYPE_KLOCK == G_KLOCK
ER
ter_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;
	PCB		*my_p_pcb;

	LOG_TER_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock(p_tcb);
	my_p_pcb = get_my_p_pcb();
	/* 自タスクを指定するとエラー */
	if ((p_tcb) == my_p_pcb->p_runtsk) {
		ercd = E_ILUSE;
	}
	/* 異なるプロセッサに割り付けられているタスクならエラーとする */
	else if (p_pcb != my_p_pcb) {
		ercd = E_OBJ;
	}
	else if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		if (TSTAT_RUNNABLE(p_tcb->tstat)) {
			/*
			 * 対象タスクが他プロセッサ上のタスクかつRUNNABLEの場合，
			 * RUN状態の可能性があるため，ディスパッチ要求を出す必要があるが，
			 * 本システムコールは同じプロセッサに割り付けられているタスクのみ
			 * に発行可能であるので，ディスパッチ要求を出す必要はない． 
			 */
			(void)make_non_runnable(p_tcb);
		}
		else if (TSTAT_WAITING(p_tcb->tstat)) {
			wait_dequeue_wobj(p_tcb);
			wait_dequeue_tmevtb(p_tcb);
		}
		make_dormant(p_tcb);
		if (p_tcb->actque) {
			p_tcb->actque = false;
			/*
			 * マイグレーション要求がキューイングしている場合は，
			 * マイグレーション処理を行う． 
			 */
			if ((p_tcb->actprc != TPRC_NONE) && (p_tcb->actprc != my_p_pcb->prcid)) {
				p_tcb->p_pcb = get_mp_p_pcb(p_tcb->actprc);
			}
			p_tcb->actprc = TPRC_NONE;
			if (make_active(p_tcb)) {
				dspreq = dispatch_request(p_tcb->p_pcb);
			}
		}
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_TER_TSK_LEAVE(ercd);
	return(ercd);
}

#else /* TTYPE_KLOCK != G_KLOCK */

ER
ter_tsk(ID tskid)
{
	TCB		*p_tcb;
	ER		ercd = E_OK;
	bool_t	dspreq = false;
	PCB		*f_p_pcb;
	PCB		*t_p_pcb;
	PCB		*my_p_pcb;
	PCB		*p_pcb;

	LOG_TER_TSK_ENTER(tskid);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID(tskid);
	p_tcb = get_tcb(tskid);

	t_lock_cpu();
  retry:
	p_pcb = t_acquire_tsk_lock(p_tcb);
	my_p_pcb = get_my_p_pcb();
	/* 自タスクを指定するとエラー */
	if ((p_tcb) == my_p_pcb->p_runtsk) {
		ercd = E_ILUSE;
		release_tsk_lock(p_pcb);
	}
	/* 異なるプロセッサに割り付けられているタスクならエラーとする */
	else if (p_pcb != my_p_pcb) {
		ercd = E_OBJ;
		release_tsk_lock(p_pcb);
	}
	else if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
		release_tsk_lock(p_pcb);
	}
	else {
		if (!(TSTAT_WAITING(p_tcb->tstat) && TSTAT_WAIT_WOBJ(p_tcb->tstat))) {
			/* オブジェクト待ち以外の状態の場合 */
			if ((p_tcb->actprc == TPRC_NONE) || (p_tcb->actprc == p_pcb->prcid)) {
				/* 再起動時のマイグレーションなし */
				if (TSTAT_RUNNABLE(p_tcb->tstat)) {
					/*
					 * 対象タスクが他プロセッサ上のタスクかつRUNNABLEの場合，
					 * RUN状態の可能性があるため，ディスパッチ要求を出す必要があるが，
					 * 本システムコールは同じプロセッサに割り付けられているタスクのみ
					 * に発行可能であるので，ディスパッチ要求を出す必要はない． 
					 */
					(void)make_non_runnable(p_tcb);
				}
				else if (TSTAT_WAITING(p_tcb->tstat)) {
					/* オブジェクト待ち以外の待ち状態の場合 */
					wait_dequeue_tmevtb(p_tcb);
				}
				make_dormant(p_tcb);
				if (p_tcb->actque) {
					p_tcb->actque = false;
					p_tcb->actprc = TPRC_NONE;
					if (make_active(p_tcb)) {
						dspreq = dispatch_request(p_pcb);
					}
				}
				release_tsk_lock(p_pcb);
				if (dspreq) {
					dispatch();
				}
			}
			else {
				/* 起動要求キューイングあり & 他プロセッサにマイグレーションあり */
				/* 移動先のプロセッサのPCBを取得 */
				t_p_pcb = get_mp_p_pcb(p_tcb->actprc);
				/* 一旦タスクロックを離す */
				release_tsk_lock(p_pcb);

				/*
				 * 現在割り付けられているプロセッサと移動先のプロセッサのタスク
				 * ロックを取得
				 */
				t_acquire_dual_tsk_lock(p_tcb, p_tcb->actprc, &p_pcb, &t_p_pcb);

				/*
				 *  マイグレーション先のプロセッサが変更された場合はリトライ
				 *  対象タスクと自タスクが所属するプロセッサが異なる場合も
				 *  リトライする．
				 */
				if ((p_tcb->actprc != t_p_pcb->prcid) || (p_pcb != my_p_pcb)) {
					release_dual_tsk_lock(p_pcb, t_p_pcb);
					goto retry;
				}
				if (TSTAT_RUNNABLE(p_tcb->tstat)) {
					/*
					 * 対象タスクが他プロセッサ上のタスクかつRUNNABLEの場合，
					 * RUN状態の可能性があるため，ディスパッチ要求を出す必要があるが，
					 * 本システムコールは同じプロセッサに割り付けられているタスクのみ
					 * に発行可能であるので，ディスパッチ要求を出す必要はない． 
					 */
					(void)make_non_runnable(p_tcb);
				}
				else if (TSTAT_WAITING(p_tcb->tstat)) {
					/* オブジェクト待ち以外の待ち状態の場合 */
					wait_dequeue_tmevtb(p_tcb);
				}
				make_dormant(p_tcb);
				p_tcb->actque = false;
				p_tcb->actprc = TPRC_NONE;
				f_p_pcb = p_pcb;
				p_tcb->p_pcb = t_p_pcb;
				if (make_active(p_tcb)) {
					/*
					 * 現在所属するプロセッサと同じプロセッサにマイグレー
					 * ションする場合はここに来ないため，dispatch_request()
					 * 戻り値はチェックしない． 
					 */ 
					(void)dispatch_request(t_p_pcb);
				}
				release_dual_tsk_lock(f_p_pcb, t_p_pcb);
			}
		}
		else {
			/* オブジェクト待ち状態の場合 */
			if ((p_tcb->actprc == TPRC_NONE) || (p_tcb->actprc == p_pcb->prcid)) {
				/* 再起動時のマイグレーションなし */
				/*
				 * デッドロック回避のため，ロックを取得し直す 
				 */
				WOBJCB *p_wobjcb = p_tcb->p_wobjcb;
				release_tsk_lock(p_pcb);
				/*
				 * あらためて
				 *   オブジェクトロック -> タスクロック
				 * の順でロックを取得 
				 */
				TEST_G_LABEL("_test_ter_tsk_01");
				t_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
				if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_wobjcb))) == NULL){
					goto retry;
				}
				/*
				 * オブジェクト待ち状態以外ないし，異なるオブジェクトに対する待ち
				 * になっていないかチェック．
				 */
				if ((TSTAT_WAITING(p_tcb->tstat) && !TSTAT_WAIT_WOBJ(p_tcb->tstat))
					|| p_wobjcb != p_tcb->p_wobjcb) {
					release_nested_tsk_lock(p_pcb);
					release_obj_lock(&GET_OBJLOCK(p_wobjcb));
					goto retry;
				}
				wait_dequeue_wobj(p_tcb);
				wait_dequeue_tmevtb(p_tcb);
				release_obj_lock(&GET_OBJLOCK(p_wobjcb));
				make_dormant(p_tcb);
				if (p_tcb->actque) {
					p_tcb->actque = false;
					p_tcb->actprc = TPRC_NONE;
					if (make_active(p_tcb)) {
						dspreq = dispatch_request(p_pcb);
					}
				}
				release_tsk_lock(p_pcb);
				if (dspreq) {
					dispatch();
				}
			}
			else {
				/* 起動要求キューイングあり & 他プロセッサへのマイグレーション */
				/* 移動先のプロセッサのPCBを取得 */
				t_p_pcb = get_mp_p_pcb(p_tcb->actprc);
				/*
				 * デッドロック回避のため，ロックを取得し直す 
				 */
				WOBJCB *p_wobjcb = p_tcb->p_wobjcb;
				release_tsk_lock(p_pcb);

				/*
				 * あらためて
				 *   オブジェクトロック -> タスクロック
				 * の順でロックを取得 
				 */
				TEST_G_LABEL("_test_ter_tsk_02"); 
				t_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
				if (t_acquire_nested_dual_tsk_lock(p_tcb, p_tcb->actprc, &GET_OBJLOCK(p_wobjcb),
												   &p_pcb, &t_p_pcb)) {
					goto retry;
				}
				/*
				 * 異なるオブジェクトに対する待ちないし，オブジェクト待ち状態以外に
				 * になっていない，異なるプロセッサへのマイグレーション，対象タスク
				 * がマイグレートしているならリトライ．
				 */
				if ((p_wobjcb != p_tcb->p_wobjcb) ||
					(TSTAT_WAITING(p_tcb->tstat) && !TSTAT_WAIT_WOBJ(p_tcb->tstat)) ||
					(get_mp_p_pcb(p_tcb->actprc) != t_p_pcb) ||
					(p_pcb != my_p_pcb)) {
					release_dual_tsk_lock(p_pcb, t_p_pcb);
					release_obj_lock(&GET_OBJLOCK(p_wobjcb));
					goto retry;
				}
				wait_dequeue_wobj(p_tcb);
				wait_dequeue_tmevtb(p_tcb);
				release_obj_lock(&GET_OBJLOCK(p_wobjcb));
				make_dormant(p_tcb);
				p_tcb->actque = false;
				p_tcb->actprc = TPRC_NONE;
				f_p_pcb = p_pcb;
				p_tcb->p_pcb = t_p_pcb;
				if (make_active(p_tcb)) {
					/*
					 * 現在所属するプロセッサと同じプロセッサにマイグレー
					 * ションする場合はここに来ないため，dispatch_request()
					 * 戻り値はチェックしない． 
					 */
					(void)dispatch_request(t_p_pcb);
				}
				release_dual_tsk_lock(f_p_pcb, t_p_pcb);
			}
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TER_TSK_LEAVE(ercd);
	return(ercd);
}

#endif /* TTYPE_KLOCK != G_KLOCK */

#endif /* TOPPERS_ter_tsk */

/*
 *  タスクのベース優先度の変更
 */
#ifdef TOPPERS_chg_pri

ER
chg_pri(ID tskid, PRI tskpri)
{
	TCB		*p_tcb;
	uint_t	newpri;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;

	LOG_CHG_PRI_ENTER(tskid, tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);
	CHECK_TPRI_INI(tskpri);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	newpri = (tskpri == TPRI_INI) ? p_tcb->p_tinib->ipriority
										: INT_PRIORITY(tskpri);
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
#if TTYPE_KLOCK == G_KLOCK
		if (change_priority(p_tcb, newpri)) {
			dspreq = dispatch_request(p_pcb);
		}
#else /* TTYPE_KLOCK != G_KLOCK */
		if (!(TSTAT_WAITING(p_tcb->tstat) && TSTAT_WAIT_WOBJ(p_tcb->tstat))) {
			/* オブジェクト待ち以外の状態の場合 */
			if (change_priority(p_tcb, newpri)) {
				dspreq = dispatch_request(p_pcb);
			}
		}
		else {
			/*
			 * オブジェクト待ちの場合
			 * デッドロック回避のため，ロックを取得し直す 
			 */
			WOBJCB *p_wobjcb = p_tcb->p_wobjcb;
			p_tcb->pend_chgpri = true;
			p_tcb->pend_newpri = newpri;
			release_tsk_lock(p_pcb);

			/*
			 * あらためて
			 *   オブジェクトロック -> タスクロック
			 * の順でロックを取得 
			 */
			TEST_G_LABEL("_test_chg_pri");
		  retry:
			t_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
			if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_wobjcb))) == NULL){
				goto retry;
			}

			/* タスクの状態が変化していないかチェック */
			if (!(p_tcb->pend_chgpri)) {
				/* 既に他の箇所で優先度変更処理がなされた */
				release_nested_tsk_lock(p_pcb);
				release_obj_lock(&GET_OBJLOCK(p_wobjcb));
				t_unlock_cpu();
				ercd = E_OK;
				goto error_exit;
			}

			p_tcb->priority = p_tcb->pend_newpri;
			p_tcb->pend_chgpri = false;
			change_priority(p_tcb, newpri);
			release_obj_lock(&GET_OBJLOCK(p_wobjcb));
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
	LOG_CHG_PRI_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_chg_pri */

/*
 *  タスク優先度の参照
 */
#ifdef TOPPERS_get_pri

ER
get_pri(ID tskid, PRI *p_tskpri)
{
	TCB		*p_tcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_GET_PRI_ENTER(tskid, p_tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TSKID_SELF(tskid);

	t_lock_cpu();
	p_tcb = get_tcb_self(tskid, get_my_p_pcb());
	p_pcb = t_acquire_tsk_lock(p_tcb);
	if (TSTAT_DORMANT(p_tcb->tstat)) {
		ercd = E_OBJ;
	}
	else {
		*p_tskpri = EXT_TSKPRI(p_tcb->priority);
		ercd = E_OK;
	}
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_GET_PRI_LEAVE(ercd, *p_tskpri);
	return(ercd);
}

#endif /* TOPPERS_get_pri */

/*
 *  自タスクの拡張情報の参照
 */
#ifdef TOPPERS_get_inf

ER
get_inf(intptr_t *p_exinf)
{
	ER		ercd;
	PCB		*my_p_pcb;

	LOG_GET_INF_ENTER(p_exinf);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	*p_exinf = my_p_pcb->p_runtsk->p_tinib->exinf;
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_GET_INF_LEAVE(ercd, *p_exinf);
	return(ercd);
}

#endif /* TOPPERS_get_inf */
