/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2016 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: wait.c 1160 2016-05-27 08:39:32Z ertl-honda $
 */

/*
 *		待ち状態管理モジュール
 */

#include "kernel_impl.h"
#include "wait.h"
#include "mp.h"

/*
 *  待ち状態への遷移（タイムアウト指定）
 */
#ifdef TOPPERS_waimake

void
make_wait_tmout(TMO tmout, PCB *p_pcb)
{
	TCB *p_runtsk = p_pcb->p_runtsk;

	(void) make_non_runnable(p_runtsk);
	if (tmout > 0) {
		tmevtb_enqueue(p_pcb->p_tevtcb, &(p_runtsk->tmevtb), (RELTIM) tmout,
						(CBACK) wait_tmout, (void *) p_runtsk);
	}
	else {
		assert(tmout == TMO_FEVR);
		(p_runtsk->tmevtb).callback = NULL;
	}
}

#endif /* TOPPERS_waimake */

/*
 *  待ち解除
 */
#ifdef TOPPERS_waicmp

bool_t
wait_complete(TCB *p_tcb)
{
	wait_dequeue_tmevtb(p_tcb);
	p_tcb->wercd = E_OK;
	return(make_non_wait(p_tcb));
}

#endif /* TOPPERS_waicmp */

/*
 *  タイムアウトに伴う待ち解除
 *
 *  本関数は make_wait_tmout() によりタイムヒープに登録されタイムアウト
 *  時にコールバック関数として呼び出される．そのため，CPUロック状態かつ，
 *  タスクロック取得状態で呼び出される． 
 */
#ifdef TOPPERS_waitmo

void
wait_tmout(TCB *p_tcb)
{
	PCB		*my_p_pcb;

#if TTYPE_KLOCK == G_KLOCK
	/*
	 *  ジャイアントロックの場合は単にwaitキューから削除するだけ
	 */
	wait_dequeue_wobj(p_tcb);
#else /* TTYPE_KLOCK != G_KLOCK */
	/*
	 *  その他のロック方式
	 *
	 *  オブジェクロックを取得する場合は，デッドロック回避を行う
	 */
	if (TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
		/* オブジェクト待ちの場合 */
		WOBJCB* volatile p_wobjcb = p_tcb->p_wobjcb;
		
		/*
		 * デッドロック回避対応
		 *
		 * デットロック回避のためにタスクロックを解放した間に他
		 * のコアから待ち状態を解除される可能性がある．
		 * そのため，pend_relwai をtrue としてロックを解除する．
		 * 他の待ち状態を解除するAPIが発行された場合には，APIの内部で，
		 * pend_relwai を false とする．そのため，ロック取得後に pend_realwai
		 * をチェックして， false なら待ち解除は行わない．
		 *  
		 * TCBの tmevtb.callback を NULL とする．本コールバック関数が
		 * 呼び出される直前に signal_time() 内で tmevtb_delete_top()
		 * によりタイムイベントキューから削除されている．そのため，ロ
		 * ックを解除している間に他の待ち状態を解除するAPIが発行された
		 * 場合には，APIの内部で wait_dequeue_tmevtb() が呼び出され，
		 * 2重にタイムイベントキューから削除しようとする．
		 * wait_dequeue_tmevtb()内では，TCBの tmevtb.callback が NULL
		 * の場合には，タイムイベントキューからは削除しない．そのため，
		 * 2重の削除を防ぐため，TCBの tmevtb.callback を NULL とする．
		 */
		p_tcb->pend_relwai = true;
		(p_tcb->tmevtb).callback = NULL;

		release_tsk_lock(p_tcb->p_pcb);

		/*
		 * あらためて
		 * オブジェクトロック -> タスクロックの順でロックを取得
		 */
		TEST_G_LABEL("_test_wait_tmout");
	  retry:
		i_acquire_obj_lock(&GET_OBJLOCK(p_wobjcb));
		if (i_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_wobjcb)) == NULL){
			goto retry;
		}

		/* タスクの状態が変化していないかチェック */
		if (!(p_tcb->pend_relwai)) {
			/* 既に他の箇所で待ち解除処理がなされた */
			release_obj_lock(&GET_OBJLOCK(p_wobjcb));
			return;
		}
		p_tcb->pend_relwai = false;
		wait_dequeue_wobj(p_tcb);
		release_obj_lock(&GET_OBJLOCK(p_wobjcb));
	}
#endif /* TTYPE_KLOCK != G_KLOCK */

	p_tcb->wercd = E_TMOUT;
	if (make_non_wait(p_tcb)) {
		if (dispatch_request(p_tcb->p_pcb)) {
			p_tcb->p_pcb->reqflg = true;
		}
	}

	/*
	 *  ここで優先度の高い割込みを受け付ける．
	 */
	my_p_pcb = get_my_p_pcb();
	release_tsk_lock(my_p_pcb);
	i_unlock_cpu();
	i_lock_cpu();
	i_acquire_tsk_lock_self();
}

#endif /* TOPPERS_waitmo */

#ifdef TOPPERS_waitmook

void
wait_tmout_ok(TCB *p_tcb)
{
	PCB *p_pcb = p_tcb->p_pcb;
	PCB *my_p_pcb;

	p_tcb->wercd = E_OK;
	if (make_non_wait(p_tcb)) {
		if (dispatch_request(p_tcb->p_pcb)) {
			p_pcb->reqflg = true;
		}
	}

	/*
	 *  ここで優先度の高い割込みを受け付ける．
	 */
	my_p_pcb = get_my_p_pcb();
	release_tsk_lock(my_p_pcb);
	i_unlock_cpu();
	i_lock_cpu();
	i_acquire_tsk_lock_self();
}

#endif /* TOPPERS_waitmook */

/*
 *  待ち状態の強制解除
 */
#ifdef TOPPERS_wairel

bool_t
wait_release(TCB *p_tcb)
{
	wait_dequeue_wobj(p_tcb);
	wait_dequeue_tmevtb(p_tcb);
	p_tcb->wercd = E_RLWAI;
	return(make_non_wait(p_tcb));
}

#endif /* TOPPERS_wairel */

/*
 *  実行中のタスクの同期・通信オブジェクトの待ちキューへの挿入
 *
 *  実行中のタスクを，同期・通信オブジェクトの待ちキューへ挿入する．オ
 *  ブジェクトの属性に応じて，FIFO順またはタスク優先度順で挿入する．
 */
Inline void
wobj_queue_insert(WOBJCB *p_wobjcb, TCB *p_runtsk)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_runtsk);
	}
	else {
		queue_insert_prev(&(p_wobjcb->wait_queue), &(p_runtsk->task_queue));
	}
}

/*
 *  同期・通信オブジェクトに対する待ち状態への遷移
 */
#ifdef TOPPERS_wobjwai

void
wobj_make_wait(WOBJCB *p_wobjcb, TCB *p_runtsk)
{
	make_wait(p_runtsk);
	wobj_queue_insert(p_wobjcb, p_runtsk);
	p_runtsk->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_runtsk);
}

#endif /* TOPPERS_wobjwai */
#ifdef TOPPERS_wobjwaitmo

void
wobj_make_wait_tmout(WOBJCB *p_wobjcb, TMO tmout, TCB *p_runtsk)
{
	make_wait_tmout(tmout, p_runtsk->p_pcb);
	wobj_queue_insert(p_wobjcb, p_runtsk);
	p_runtsk->p_wobjcb = p_wobjcb;
	LOG_TSKSTAT(p_runtsk);
}

#endif /* TOPPERS_wobjwaitmo */

/*
 *  待ちキューの初期化
 */
#ifdef TOPPERS_iniwque

bool_t
init_wait_queue(QUEUE *p_wait_queue)
{
	TCB		*p_tcb;
	bool_t	dspreq = false;
	PCB		*p_pcb;     

	while (!queue_empty(p_wait_queue)) {
		p_tcb = (TCB *) queue_delete_next(p_wait_queue);
		/* タスクロックの取得 */
		p_pcb = acquire_nested_tsk_lock_without_preemption(p_tcb);
		wait_dequeue_tmevtb(p_tcb);
		p_tcb->wercd = E_DLT;
		if (make_non_wait(p_tcb)) {
			if (dispatch_request(p_pcb)) {
				dspreq = true;
			}
		}
		release_nested_tsk_lock(p_pcb);
	}

	return(dspreq);
}

#endif /* TOPPERS_iniwque */
