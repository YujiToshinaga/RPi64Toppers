/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2014 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: dataqueue.c 1064 2014-11-28 04:24:30Z ertl-honda $
 */

/*
 *		データキュー機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "wait.h"
#include "dataqueue.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SND_DTQ_ENTER
#define LOG_SND_DTQ_ENTER(dtqid, data)
#endif /* LOG_SND_DTQ_ENTER */

#ifndef LOG_SND_DTQ_LEAVE
#define LOG_SND_DTQ_LEAVE(ercd)
#endif /* LOG_SND_DTQ_LEAVE */

#ifndef LOG_PSND_DTQ_ENTER
#define LOG_PSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_PSND_DTQ_ENTER */

#ifndef LOG_PSND_DTQ_LEAVE
#define LOG_PSND_DTQ_LEAVE(ercd)
#endif /* LOG_PSND_DTQ_LEAVE */

#ifndef LOG_IPSND_DTQ_ENTER
#define LOG_IPSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_IPSND_DTQ_ENTER */

#ifndef LOG_IPSND_DTQ_LEAVE
#define LOG_IPSND_DTQ_LEAVE(ercd)
#endif /* LOG_IPSND_DTQ_LEAVE */

#ifndef LOG_TSND_DTQ_ENTER
#define LOG_TSND_DTQ_ENTER(dtqid, data, tmout)
#endif /* LOG_TSND_DTQ_ENTER */

#ifndef LOG_TSND_DTQ_LEAVE
#define LOG_TSND_DTQ_LEAVE(ercd)
#endif /* LOG_TSND_DTQ_LEAVE */

#ifndef LOG_FSND_DTQ_ENTER
#define LOG_FSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_FSND_DTQ_ENTER */

#ifndef LOG_FSND_DTQ_LEAVE
#define LOG_FSND_DTQ_LEAVE(ercd)
#endif /* LOG_FSND_DTQ_LEAVE */

#ifndef LOG_IFSND_DTQ_ENTER
#define LOG_IFSND_DTQ_ENTER(dtqid, data)
#endif /* LOG_IFSND_DTQ_ENTER */

#ifndef LOG_IFSND_DTQ_LEAVE
#define LOG_IFSND_DTQ_LEAVE(ercd)
#endif /* LOG_IFSND_DTQ_LEAVE */

#ifndef LOG_RCV_DTQ_ENTER
#define LOG_RCV_DTQ_ENTER(dtqid, p_data)
#endif /* LOG_RCV_DTQ_ENTER */

#ifndef LOG_RCV_DTQ_LEAVE
#define LOG_RCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_RCV_DTQ_LEAVE */

#ifndef LOG_PRCV_DTQ_ENTER
#define LOG_PRCV_DTQ_ENTER(dtqid, p_data)
#endif /* LOG_PRCV_DTQ_ENTER */

#ifndef LOG_PRCV_DTQ_LEAVE
#define LOG_PRCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_PRCV_DTQ_LEAVE */

#ifndef LOG_TRCV_DTQ_ENTER
#define LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout)
#endif /* LOG_TRCV_DTQ_ENTER */

#ifndef LOG_TRCV_DTQ_LEAVE
#define LOG_TRCV_DTQ_LEAVE(ercd, data)
#endif /* LOG_TRCV_DTQ_LEAVE */

#ifndef LOG_INI_DTQ_ENTER
#define LOG_INI_DTQ_ENTER(dtqid)
#endif /* LOG_INI_DTQ_ENTER */

#ifndef LOG_INI_DTQ_LEAVE
#define LOG_INI_DTQ_LEAVE(ercd)
#endif /* LOG_INI_DTQ_LEAVE */

#ifndef LOG_REF_DTQ_ENTER
#define LOG_REF_DTQ_ENTER(dtqid, pk_rdtq)
#endif /* LOG_REF_DTQ_ENTER */

#ifndef LOG_REF_DTQ_LEAVE
#define LOG_REF_DTQ_LEAVE(ercd, pk_rdtq)
#endif /* LOG_REF_DTQ_LEAVE */

/*
 *  データキューの数
 */
#define tnum_dtq	((uint_t)(tmax_dtqid - TMIN_DTQID + 1))

/*
 *  データキューIDからデータキュー管理ブロックを取り出すためのマクロ
 */
#define INDEX_DTQ(dtqid)	((uint_t)((dtqid) - TMIN_DTQID))
#define get_dtqcb(dtqid)	(p_dtqcb_table[INDEX_DTQ(dtqid)])

/*
 *  データキュー機能の初期化
 */
#ifdef TOPPERS_dtqini

void
initialize_dataqueue(void)
{
	uint_t	i;
	DTQCB	*p_dtqcb;

	if (x_sense_mprc()) {
		for (i = 0; i < tnum_dtq; i++) {
			p_dtqcb = p_dtqcb_table[i];
			queue_initialize(&(p_dtqcb->swait_queue));
			p_dtqcb->p_dtqinib = &(dtqinib_table[i]);
			queue_initialize(&(p_dtqcb->rwait_queue));
			p_dtqcb->count = 0U;
			p_dtqcb->head = 0U;
			p_dtqcb->tail = 0U;
#if TTYPE_KLOCK == F_KLOCK
			x_initialize_obj_lock(&(p_dtqcb->obj_lock));
#endif /* TTYPE_KLOCK == F_KLOCK */
		}
	}
}

#endif /* TOPPERS_dtqini */

/*
 *  データキュー管理領域へのデータの格納
 */
#ifdef TOPPERS_dtqenq

void
enqueue_data(DTQCB *p_dtqcb, intptr_t data)
{
	(p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->tail)->data = data;
	p_dtqcb->count++;
	p_dtqcb->tail++;
	if (p_dtqcb->tail >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->tail = 0U;
	}
}

#endif /* TOPPERS_dtqenq */

/*
 *  データキュー管理領域へのデータの強制格納
 */
#ifdef TOPPERS_dtqfenq

void
force_enqueue_data(DTQCB *p_dtqcb, intptr_t data)
{
	(p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->tail)->data = data;
	p_dtqcb->tail++;
	if (p_dtqcb->tail >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->tail = 0U;
	}
	if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->count++;
	}
	else {
		p_dtqcb->head = p_dtqcb->tail;
	}
}

#endif /* TOPPERS_dtqfenq */

/*
 *  データキュー管理領域からのデータの取出し
 */
#ifdef TOPPERS_dtqdeq

void
dequeue_data(DTQCB *p_dtqcb, intptr_t *p_data)
{
	*p_data = (p_dtqcb->p_dtqinib->p_dtqmb + p_dtqcb->head)->data;
	p_dtqcb->count--;
	p_dtqcb->head++;
	if (p_dtqcb->head >= p_dtqcb->p_dtqinib->dtqcnt) {
		p_dtqcb->head = 0U;
	}
}

#endif /* TOPPERS_dtqdeq */

/*
 *  データキューへの送信
 */
#ifdef TOPPERS_snd_dtq

ER
snd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*my_p_pcb;
	PCB		*p_pcb;
	TCB		*p_runtsk;

	LOG_SND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない．*/
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_OK;
	}
	else {
		if ((my_p_pcb = t_acquire_nested_tsk_lock_self(&GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		p_runtsk = my_p_pcb->p_runtsk;
		p_runtsk->winfo_obj.dtq.data = data;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SDTQ);
		wobj_make_wait((WOBJCB *) p_dtqcb, p_runtsk);
		release_nested_tsk_lock(my_p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		dispatch();
		ercd = p_runtsk->wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_SND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_snd_dtq */

/*
 *  データキューへの送信（ポーリング）
 */
#ifdef TOPPERS_psnd_dtq

ER
psnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*p_pcb;

	LOG_PSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない． */
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		ercd = E_OK;
	}
	else if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_PSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_psnd_dtq */

/*
 *  データキューへの送信（ポーリング，非タスクコンテキスト用）
 */
#ifdef TOPPERS_ipsnd_dtq

ER
ipsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*p_pcb;

	LOG_IPSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	i_lock_cpu();

  retry:
	i_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない．*/
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb = i_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			if (dispatch_request(p_pcb)) {
				(get_my_p_pcb())->reqflg = true;
			}
		}
		release_nested_tsk_lock(p_pcb);
		ercd = E_OK;
	}
	else if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	i_unlock_cpu();

  error_exit:
	LOG_IPSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ipsnd_dtq */

/*
 *  データキューへの送信（タイムアウトあり）
 */
#ifdef TOPPERS_tsnd_dtq

ER
tsnd_dtq(ID dtqid, intptr_t data, TMO tmout)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*my_p_pcb;
	PCB		*p_pcb;
	TCB		*p_runtsk;

	LOG_TSND_DTQ_ENTER(dtqid, data, tmout);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない．*/
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (p_dtqcb->count < p_dtqcb->p_dtqinib->dtqcnt) {
		enqueue_data(p_dtqcb, data);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_OK;
	}
	else if (tmout == TMO_POL) {
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_TMOUT;
	}
	else {
		if ((my_p_pcb = t_acquire_nested_tsk_lock_self(&GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		p_runtsk = my_p_pcb->p_runtsk;
		p_runtsk->winfo_obj.dtq.data = data;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_SDTQ);
		wobj_make_wait_tmout((WOBJCB *) p_dtqcb, tmout, p_runtsk);
		release_nested_tsk_lock(my_p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		dispatch();
		ercd = p_runtsk->wercd;
	}
	t_unlock_cpu();

  error_exit:
	LOG_TSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_tsnd_dtq */

/*
 *  データキューへの強制送信
 */
#ifdef TOPPERS_fsnd_dtq

ER
fsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	ER		ercd;
	bool_t	dspreq = false;
	TCB		*p_tcb;
	PCB		*p_pcb;

	LOG_FSND_DTQ_ENTER(dtqid, data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(p_dtqcb->p_dtqinib->dtqcnt > 0U);

	t_lock_cpu();
  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない．*/
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb =t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
	}
	else {
		force_enqueue_data(p_dtqcb, data);
	}
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	if (dspreq) {
		dispatch();
	}
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_FSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_fsnd_dtq */

/*
 *  データキューへの強制送信（非タスクコンテキスト用）
 */
#ifdef TOPPERS_ifsnd_dtq

ER
ifsnd_dtq(ID dtqid, intptr_t data)
{
	DTQCB	*p_dtqcb;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*p_pcb;

	LOG_IFSND_DTQ_ENTER(dtqid, data);
	CHECK_INTCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);
	CHECK_ILUSE(p_dtqcb->p_dtqinib->dtqcnt > 0U);

	i_lock_cpu();

  retry:
	i_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/*
	 * この間は繰り返し実行されるため，非破壊コードでなければならない．
	 */
	if (!queue_empty(&(p_dtqcb->rwait_queue))) {
		p_tcb = (TCB *)(p_dtqcb->rwait_queue.p_next);
		if ((p_pcb = i_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		p_tcb->winfo_obj.dtq.data = data;
		if (wait_complete(p_tcb)) {
			if (dispatch_request(p_pcb)){
				(get_my_p_pcb())->reqflg = true;
			}
		}
		release_nested_tsk_lock(p_pcb);
	}
	else {
		force_enqueue_data(p_dtqcb, data);
	}
	ercd = E_OK;
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	i_unlock_cpu();

  error_exit:
	LOG_IFSND_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ifsnd_dtq */

/*
 *  データキューからの受信
 */
#ifdef TOPPERS_rcv_dtq

ER
rcv_dtq(ID dtqid, intptr_t *p_data)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*my_p_pcb;
	PCB		*p_pcb;
	TCB		*p_runtsk;

	LOG_RCV_DTQ_ENTER(dtqid, p_data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	if (!queue_empty(&(p_dtqcb->swait_queue))) {
		/* 送信待ちタスクあり */
		p_tcb = (TCB *)(p_dtqcb->swait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL){
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		if (p_dtqcb->count > 0U) {
			dequeue_data(p_dtqcb, p_data);
			enqueue_data(p_dtqcb, p_tcb->winfo_obj.dtq.data);
		}
		else {
			*p_data = p_tcb->winfo_obj.dtq.data;
		}
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (p_dtqcb->count > 0U) {
		/* バッファにデータがあり，送信待ちタスクなし */
		dequeue_data(p_dtqcb, p_data);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_OK;
	}
	else {
		/* バッファが空かつ送信待ちタスクなし */
		if ((my_p_pcb = t_acquire_nested_tsk_lock_self(&GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		p_runtsk = my_p_pcb->p_runtsk;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RDTQ);
		make_wait(p_runtsk);
		queue_insert_prev(&(p_dtqcb->rwait_queue), &(p_runtsk->task_queue));
		p_runtsk->p_wobjcb = (WOBJCB*)p_dtqcb;
		LOG_TSKSTAT(p_runtsk);
		release_nested_tsk_lock(my_p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		dispatch();
		ercd = p_runtsk->wercd;
		if (ercd == E_OK) {
			*p_data = p_runtsk->winfo_obj.dtq.data;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_RCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_rcv_dtq */

/*
 *  データキューからの受信（ポーリング）
 */
#ifdef TOPPERS_prcv_dtq

ER
prcv_dtq(ID dtqid, intptr_t *p_data)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*p_pcb;

	LOG_PRCV_DTQ_ENTER(dtqid, p_data);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない． */
	if (!queue_empty(&(p_dtqcb->swait_queue))) {
		/* 送信待ちタスクあり */
		p_tcb = (TCB *)(p_dtqcb->swait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		if (p_dtqcb->count > 0U) {
			dequeue_data(p_dtqcb, p_data);
			enqueue_data(p_dtqcb, p_tcb->winfo_obj.dtq.data);
		}
		else {
			*p_data = p_tcb->winfo_obj.dtq.data;
		}
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		ercd = E_OK;
	}
	else if (p_dtqcb->count > 0U) {
		/* バッファにデータがあり，送信待ちタスクなし */
		dequeue_data(p_dtqcb, p_data);
		ercd = E_OK;
	}
	else {
		ercd = E_TMOUT;
	}
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_PRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_prcv_dtq */

/*
 *  データキューからの受信（タイムアウトあり）
 */
#ifdef TOPPERS_trcv_dtq

ER
trcv_dtq(ID dtqid, intptr_t *p_data, TMO tmout)
{
	DTQCB	*p_dtqcb;
	bool_t	dspreq = false;
	ER		ercd;
	TCB		*p_tcb;
	PCB		*my_p_pcb;
	PCB		*p_pcb;
	TCB		*p_runtsk;

	LOG_TRCV_DTQ_ENTER(dtqid, p_data, tmout);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	CHECK_TMOUT(tmout);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	T_CHECK_DISPATCH(my_p_pcb);

  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	/* この間は繰り返し実行されるため，非破壊コードでなければならない．*/
	if (!queue_empty(&(p_dtqcb->swait_queue))) {
		/* 送信待ちタスクあり */
		p_tcb = (TCB *)(p_dtqcb->swait_queue.p_next);
		if ((p_pcb = t_acquire_nested_tsk_lock(p_tcb, &GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		queue_delete((QUEUE *) p_tcb);
		if (p_dtqcb->count > 0U) {
			dequeue_data(p_dtqcb, p_data);
			enqueue_data(p_dtqcb, p_tcb->winfo_obj.dtq.data);
		}
		else {
			*p_data = p_tcb->winfo_obj.dtq.data;
		}
		if (wait_complete(p_tcb)) {
			dspreq = dispatch_request(p_pcb);
		}
		release_nested_tsk_lock(p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		if (dspreq) {
			dispatch();
		}
		ercd = E_OK;
	}
	else if (p_dtqcb->count > 0U) {
		/* バッファにデータがあり，送信待ちタスクなし */
		dequeue_data(p_dtqcb, p_data);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_OK;
	}   /* バッファが空かつ送信待ちタスクなし */
	else if (tmout == TMO_POL) {
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		ercd = E_TMOUT;
	}
	else {
		if ((my_p_pcb = t_acquire_nested_tsk_lock_self(&GET_OBJLOCK(p_dtqcb))) == NULL) {
			goto retry;
		}
		p_runtsk = my_p_pcb->p_runtsk;
		p_runtsk->tstat = (TS_WAITING | TS_WAIT_RDTQ);
		make_wait_tmout(tmout, my_p_pcb);
		queue_insert_prev(&(p_dtqcb->rwait_queue), &(p_runtsk->task_queue));
		p_runtsk->p_wobjcb = (WOBJCB*)p_dtqcb;
		LOG_TSKSTAT(p_runtsk);
		release_nested_tsk_lock(my_p_pcb);
		release_obj_lock(&GET_OBJLOCK(p_dtqcb));
		dispatch();
		ercd = p_runtsk->wercd;
		if (ercd == E_OK) {
			*p_data = p_runtsk->winfo_obj.dtq.data;
		}
	}
	t_unlock_cpu();

  error_exit:
	LOG_TRCV_DTQ_LEAVE(ercd, *p_data);
	return(ercd);
}

#endif /* TOPPERS_trcv_dtq */

/*
 *  データキューの再初期化
 */
#ifdef TOPPERS_ini_dtq

ER
ini_dtq(ID dtqid)
{
	DTQCB	*p_dtqcb;
	ER		ercd;
	bool_t	dspreq;

	LOG_INI_DTQ_ENTER(dtqid);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	dspreq = init_wait_queue(&(p_dtqcb->swait_queue));
	if (init_wait_queue(&(p_dtqcb->rwait_queue))){
		dspreq = true;
	}
	p_dtqcb->count = 0U;
	p_dtqcb->head = 0U;
	p_dtqcb->tail = 0U;
	ercd = E_OK;
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_INI_DTQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ini_dtq */

/*
 *  データキューの状態参照
 */
#ifdef TOPPERS_ref_dtq

ER
ref_dtq(ID dtqid, T_RDTQ *pk_rdtq)
{
	DTQCB	*p_dtqcb;
	ER		ercd;

	LOG_REF_DTQ_ENTER(dtqid, pk_rdtq);
	CHECK_TSKCTX_UNL();
	CHECK_DTQID(dtqid);
	p_dtqcb = get_dtqcb(dtqid);

	t_lock_cpu();
	t_acquire_obj_lock(&GET_OBJLOCK(p_dtqcb));
	pk_rdtq->stskid = wait_tskid(&(p_dtqcb->swait_queue));
	pk_rdtq->rtskid = wait_tskid(&(p_dtqcb->rwait_queue));
	pk_rdtq->sdtqcnt = p_dtqcb->count;
	ercd = E_OK;
	release_obj_lock(&GET_OBJLOCK(p_dtqcb));
	t_unlock_cpu();

  error_exit:
	LOG_REF_DTQ_LEAVE(ercd, pk_rdtq);
	return(ercd);
}

#endif /* TOPPERS_ref_dtq */
