/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2010 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: wait.h 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		待ち状態管理モジュール
 */

#ifndef TOPPERS_WAIT_H
#define TOPPERS_WAIT_H

#include "task.h"
#include "time_event.h"

/*
 *  タスクの優先度順の待ちキューへの挿入
 *
 *  p_tcbで指定されるタスクを，タスク優先度順のキューp_queueに挿入する．
 *  キューの中に同じ優先度のタスクがある場合には，その最後に挿入する．
 */
Inline void
queue_insert_tpri(QUEUE *p_queue, TCB *p_tcb)
{
	QUEUE	*p_entry;
	uint_t	pri = p_tcb->priority;

	for (p_entry = p_queue->p_next; p_entry != p_queue;
										p_entry = p_entry->p_next) {
		if (pri < ((TCB *) p_entry)->priority) {
			break;
		}
	}
	queue_insert_prev(p_entry, &(p_tcb->task_queue));
}

/*
 *  待ち状態への遷移
 *
 *  実行中のタスクを待ち状態に遷移させる．具体的には，実行中のタスクを
 *  レディキューから削除し，TCBのtmevtbのcallbackフィールドをNULLに
 *  設定する（永久待ち）．
 */
Inline void
make_wait(TCB *p_runtsk)
{
	(void) make_non_runnable(p_runtsk);
	(p_runtsk->tmevtb).callback = NULL;
}

/*
 *  待ち状態への遷移（タイムアウト指定）
 *
 *  実行中のタスクを，タイムアウト指定付きで待ち状態に遷移させる．具体
 *  的には，実行中のタスクをレディキューから削除し，タイムイベントブ
 *  ロックを登録する．
 */
extern void	make_wait_tmout(TMO tmout, PCB *p_pcb);

/*
 *  待ち解除のためのタスク状態の更新
 *
 *  p_tcbで指定されるタスクを，待ち解除するようタスク状態を更新する．
 *  待ち解除するタスクが実行できる状態になる場合は，レディキューにつな
 *  ぐ．また，ディスパッチが必要な場合にはtrueを返す．
 *    
 *  待ち状態を解除する場合は，必ずこの関数を呼び出すため，デッドロック
 *  回避のためのペンディングフラグのチェックをまとめて行う．
 */
Inline bool_t
make_non_wait(TCB *p_tcb)
{
	assert(TSTAT_WAITING(p_tcb->tstat));

#if TTYPE_KLOCK != G_KLOCK
	/* 優先度変更フラグチェック */
	if (p_tcb->pend_chgpri) {
		p_tcb->priority = p_tcb->pend_newpri;
		/* 優先度変更フラグのクリア */
		p_tcb->pend_chgpri = false;
	}

	/* タスク強制待ち解除保留クリア */
	p_tcb->pend_relwai = false;
#endif /* TTYPE_KLOCK != G_KLOCK */

	if (!TSTAT_SUSPENDED(p_tcb->tstat)) {
		/*
		 *  待ち状態から実行できる状態への遷移
		 */
		p_tcb->tstat = TS_RUNNABLE;
		LOG_TSKSTAT(p_tcb);
		return(make_runnable(p_tcb));
	}
	else {
		/*
		 *  二重待ち状態から強制待ち状態への遷移
		 */
		p_tcb->tstat = TS_SUSPENDED;
		LOG_TSKSTAT(p_tcb);
		return(false);
	}
}

/*
 *  オブジェクト待ちキューからの削除
 *
 *  p_tcbで指定されるタスクが，同期・通信オブジェクトの待ちキューにつ
 *  ながれていれば，待ちキューから削除する．
 */
Inline void
wait_dequeue_wobj(TCB *p_tcb)
{
	if (TSTAT_WAIT_WOBJ(p_tcb->tstat)) {
		queue_delete(&(p_tcb->task_queue));
	}
}

/*
 *  時間待ちのためのタイムイベントブロックの登録解除
 *
 *  p_tcbで指定されるタスクに対して，時間待ちのためのタイムイベントブ
 *  ロックが登録されていれば，それを登録解除する．
 */
Inline void
wait_dequeue_tmevtb(TCB *p_tcb)
{
	if ((p_tcb->tmevtb).callback != NULL) {
		tmevtb_dequeue(p_tcb->p_pcb->p_tevtcb, &(p_tcb->tmevtb));
	}
}

/*
 *  待ち解除
 *
 *  p_tcbで指定されるタスクの待ち状態を解除する．具体的には，タイムイ
 *  ベントブロックが登録されていれば，それを登録解除する．また，タスク
 *  状態を更新し，待ち解除したタスクからの返値をE_OKとする．待ちキュー
 *  からの削除は行わない．待ち解除したタスクへのディスパッチが必要な場
 *  合にはtrueを返す．
 */
extern bool_t	wait_complete(TCB *p_tcb);

/*
 *  タイムアウトに伴う待ち解除
 *
 *  p_tcbで指定されるタスクが，待ちキューにつながれていれば待ちキュー
 *  から削除し，タスク状態を更新する．また，待ち解除したタスクからの返
 *  値を，wait_tmoutではE_TMOUT，wait_tmout_okではE_OKとする．待ち解除
 *  したタスクへのディスパッチが必要な時は，reqflgをtrueにする．
 *
 *  wait_tmout_okは，dly_tskで使うためのもので，待ちキューから削除する
 *  処理を行わない．
 *
 *  いずれの関数も，タイムイベントのコールバック関数として用いるための
 *  もので，割込みハンドラから呼び出されることを想定している．
 */
extern void	wait_tmout(TCB *p_tcb);
extern void	wait_tmout_ok(TCB *p_tcb);

/*
 *  待ち状態の強制解除
 *
 *  p_tcbで指定されるタスクの待ち状態を強制的に解除する．具体的には，
 *  タスクが待ちキューにつながれていれば待ちキューから削除し，タイムイ
 *  ベントブロックが登録されていればそれを登録解除する．また，タスクの
 *  状態を更新し，待ち解除したタスクからの返値をE_RLWAIとする．また，
 *  待ち解除したタスクへのディスパッチが必要な場合にはtrueを返す．
 */
extern bool_t	wait_release(TCB *p_tcb);

/*
 *  待ちキューの先頭のタスクID
 *
 *  p_wait_queueで指定した待ちキューの先頭のタスクIDを返す．待ちキュー
 *  が空の場合には，TSK_NONEを返す．
 */
Inline ID
wait_tskid(QUEUE *p_wait_queue)
{
	if (!queue_empty(p_wait_queue)) {
		return(TSKID(((TCB *)p_wait_queue->p_next)));
	}
	else {
		return(TSK_NONE);
	}
}

/*
 *  同期・通信オブジェクトに対する待ち状態への遷移
 *  
 *  実行中のタスクを待ち状態に遷移させ，同期・通信オブジェクトの待ちキュー
 *  につなぐ．また，待ち情報ブロック（WINFO）のp_wobjcbを設定する．
 *  wobj_make_wait_tmoutは，タイムイベントブロックの登録も行う．
 */
extern void	wobj_make_wait(WOBJCB *p_wobjcb, TCB *p_runtsk);
extern void	wobj_make_wait_tmout(WOBJCB *p_wobjcb, TMO tmout, TCB *p_runtsk);

/*
 *  タスク優先度変更時の処理
 *
 *  同期・通信オブジェクトに対する待ち状態にあるタスクの優先度が変更さ
 *  れた場合に，待ちキューの中でのタスクの位置を修正する．
 */
Inline void
wobj_change_priority(WOBJCB *p_wobjcb, TCB *p_tcb)
{
	if ((p_wobjcb->p_wobjinib->wobjatr & TA_TPRI) != 0U) {
		queue_delete(&(p_tcb->task_queue));
		queue_insert_tpri(&(p_wobjcb->wait_queue), p_tcb);
	}
}

/*
 *  待ちキューの初期化
 *
 *  待ちキューにつながれているタスクをすべて待ち解除する．待ち解除した
 *  タスクからの返値は，E_DLTとする．待ち解除したタスクへのディスパッチ
 *  が必要な場合はtrue，そうでない場合はfalseを返す．
 */
extern bool_t	init_wait_queue(QUEUE *p_wait_queue);

#endif /* TOPPERS_WAIT_H */
