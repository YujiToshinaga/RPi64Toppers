/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: alarm.c 721 2010-10-14 02:53:19Z ertl-honda $
 */

/*
 *		アラームハンドラ機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "alarm.h"
#include "mp.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_ALM_ENTER
#define LOG_ALM_ENTER(p_almcb)
#endif /* LOG_ALM_ENTER */

#ifndef LOG_ALM_LEAVE
#define LOG_ALM_LEAVE(p_almcb)
#endif /* LOG_ALM_LEAVE */

#ifndef LOG_STA_ALM_ENTER
#define LOG_STA_ALM_ENTER(almid, almtim)
#endif /* LOG_STA_ALM_ENTER */

#ifndef LOG_STA_ALM_LEAVE
#define LOG_STA_ALM_LEAVE(ercd)
#endif /* LOG_STA_ALM_LEAVE */

#ifndef LOG_MSTA_ALM_ENTER
#define LOG_MSTA_ALM_ENTER(almid, almtim, prcid)
#endif /* LOG_STA_ALM_ENTER */

#ifndef LOG_MSTA_ALM_LEAVE
#define LOG_MSTA_ALM_LEAVE(ercd)
#endif /* LOG_STA_ALM_LEAVE */

#ifndef LOG_ALMMIG
#define LOG_ALMMIG(p_almcb, src_id, dest_id)
#endif /* LOG_ALMMIG */

#ifndef LOG_ISTA_ALM_ENTER
#define LOG_ISTA_ALM_ENTER(almid, almtim)
#endif /* LOG_ISTA_ALM_ENTER */

#ifndef LOG_ISTA_ALM_LEAVE
#define LOG_ISTA_ALM_LEAVE(ercd)
#endif /* LOG_ISTA_ALM_LEAVE */

#ifndef LOG_IMSTA_ALM_ENTER
#define LOG_IMSTA_ALM_ENTER(almid, almtim, prcid)
#endif /* LOG_ISTA_ALM_ENTER */

#ifndef LOG_IMSTA_ALM_LEAVE
#define LOG_IMSTA_ALM_LEAVE(ercd)
#endif /* LOG_ISTA_ALM_LEAVE */

#ifndef LOG_STP_ALM_ENTER
#define LOG_STP_ALM_ENTER(almid)
#endif /* LOG_STP_ALM_ENTER */

#ifndef LOG_STP_ALM_LEAVE
#define LOG_STP_ALM_LEAVE(ercd)
#endif /* LOG_STP_ALM_LEAVE */

#ifndef LOG_ISTP_ALM_ENTER
#define LOG_ISTP_ALM_ENTER(almid)
#endif /* LOG_ISTP_ALM_ENTER */

#ifndef LOG_ISTP_ALM_LEAVE
#define LOG_ISTP_ALM_LEAVE(ercd)
#endif /* LOG_ISTP_ALM_LEAVE */

#ifndef LOG_REF_ALM_ENTER
#define LOG_REF_ALM_ENTER(almid, pk_ralm)
#endif /* LOG_REF_ALM_ENTER */

#ifndef LOG_REF_ALM_LEAVE
#define LOG_REF_ALM_LEAVE(ercd, pk_ralm)
#endif /* LOG_REF_ALM_LEAVE */

/*
 *  アラームハンドラの数
 */
#define tnum_alm	((uint_t)(tmax_almid - TMIN_ALMID + 1))

/*
 *  アラームハンドラIDからアラームハンドラ管理ブロックを取り出すためのマクロ
 */
#define INDEX_ALM(almid)	((uint_t)((almid) - TMIN_ALMID))
#define get_almcb(almid)	(p_almcb_table[INDEX_ALM(almid)])

/*
 *  アラームハンドラ機能の初期化
 */
#ifdef TOPPERS_almini

void
initialize_alarm(void)
{
	uint_t	i;
	ALMCB	*p_almcb;
	PCB 	*p_pcb;

#ifdef TOPPERS_SYSTIM_GLOBAL
	if (!x_sense_mprc()) {
		return;
	}
	p_pcb = get_mp_p_pcb(TOPPERS_SYSTIM_PRCID);
#else  /* TOPPERS_SYSTIM_LOCAL */
	p_pcb = get_my_p_pcb();
#endif /* TOPPERS_SYSTIM_GLOBAL */

	for (i = 0; i < tnum_alm; i++) {
#ifdef TOPPERS_SYSTIM_LOCAL
		/* 自コアに割り付けれたアラームハンドラの初期化 */
		if(alminib_table[i].iaffinity == ID_PRC(x_prc_index())){
#endif /* TOPPERS_SYSTIM_LOCAL */
			p_almcb = p_almcb_table[i];
			p_almcb->p_alminib = &alminib_table[i];
			p_almcb->almsta = false;
			p_almcb->p_pcb = p_pcb;
#ifdef TOPPERS_SYSTIM_LOCAL
		}
#endif /* TOPPERS_SYSTIM_LOCAL */
	}
}

#endif /* TOPPERS_almini */

/*
 *  アラームハンドラの動作開始
 */
#ifdef TOPPERS_sta_alm

ER
sta_alm(ID almid, RELTIM almtim)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_STA_ALM_ENTER(almid, almtim);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_alm(p_almcb);
	if (p_almcb->almsta) {
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}
	tmevtb_enqueue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_STA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_alm */

/*
 *  アラームハンドラの動作開始
 */
#ifdef TOPPERS_msta_alm

#ifdef TOPPERS_SYSTIM_LOCAL

ER
msta_alm(ID almid, RELTIM almtim, ID prcid)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;

	LOG_MSTA_ALM_ENTER(almid, almtim, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	CHECK_PRCID_INI(prcid);
	p_almcb = get_almcb(almid);
	prcid = (prcid == TPRC_INI)? p_almcb->p_alminib->iaffinity : prcid;
	CHECK_MIG(p_almcb->p_alminib->affinity_mask, prcid);

	t_lock_cpu();
	t_acquire_dual_tsk_lock_alm(p_almcb, prcid, &f_p_pcb, &t_p_pcb);
	if (p_almcb->almsta) {
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}
	LOG_ALMMIG(p_almcb, f_p_pcb->prcid, prcid);
	/* 移動先の pcb を登録 */
	p_almcb->p_pcb = t_p_pcb;
	tmevtb_enqueue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_MSTA_ALM_LEAVE(ercd);
	return(ercd);
}

#else /* TOPPERS_SYSTIM_GLOBAL */

ER
msta_alm(ID almid, RELTIM almtim, ID prcid)
{
	ER		ercd = E_NOSPT;

	LOG_MSTA_ALM_ENTER(almid, almtim, prcid);
	LOG_MSTA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SYSTIM_GLOBAL */

#endif /* TOPPERS_msta_alm */

/*
 *  アラームハンドラの動作開始（非タスクコンテキスト用）
 */
#ifdef TOPPERS_ista_alm

ER
ista_alm(ID almid, RELTIM almtim)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_ISTA_ALM_ENTER(almid, almtim);
	CHECK_INTCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	p_almcb = get_almcb(almid);

	i_lock_cpu();
	p_pcb = i_acquire_tsk_lock_alm(p_almcb);
	if (p_almcb->almsta) {
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}
	tmevtb_enqueue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_ISTA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ista_alm */

/*
 *  アラームハンドラの動作開始（非タスクコンテキスト用）
 */
#ifdef TOPPERS_imsta_alm

#ifdef TOPPERS_SYSTIM_LOCAL

ER
imsta_alm(ID almid, RELTIM almtim, ID prcid)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;

	LOG_IMSTA_ALM_ENTER(almid, almtim, prcid);
	CHECK_INTCTX_UNL();
	CHECK_ALMID(almid);
	CHECK_PAR(almtim <= TMAX_RELTIM);
	CHECK_PRCID_INI(prcid);
	p_almcb = get_almcb(almid);
	prcid = (prcid == TPRC_INI)? p_almcb->p_alminib->iaffinity : prcid;
	CHECK_MIG(p_almcb->p_alminib->affinity_mask, prcid);

	i_lock_cpu();
	i_acquire_dual_tsk_lock_alm(p_almcb, prcid, &f_p_pcb, &t_p_pcb);
	if (p_almcb->almsta) {
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	else {
		p_almcb->almsta = true;
	}

	LOG_ALMMIG(p_almcb, f_p_pcb->prcid, prcid);
	/* 移動先の pcb を登録 */
	p_almcb->p_pcb = t_p_pcb;
	tmevtb_enqueue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb), almtim,
								(CBACK) call_almhdr, (void *) p_almcb);
	ercd = E_OK;
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IMSTA_ALM_LEAVE(ercd);
	return(ercd);
}

#else /* TOPPERS_SYSTIM_GLOBAL */

ER
imsta_alm(ID almid, RELTIM almtim, ID prcid)
{
	ER		ercd = E_NOSPT;

	LOG_IMSTA_ALM_ENTER(almid, almtim, prcid);
	LOG_IMSTA_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SYSTIM_GLOBAL */

#endif /* TOPPERS_imsta_alm */

/*
 *  アラームハンドラの動作停止
 */
#ifdef TOPPERS_stp_alm

ER
stp_alm(ID almid)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_STP_ALM_ENTER(almid);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_alm(p_almcb);
	if (p_almcb->almsta) {
		p_almcb->almsta = false;
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_STP_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_alm */

/*
 *  アラームハンドラの動作停止（非タスクコンテキスト用）
 */
#ifdef TOPPERS_istp_alm

ER
istp_alm(ID almid)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_ISTP_ALM_ENTER(almid);
	CHECK_INTCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	i_lock_cpu();
	p_pcb = i_acquire_tsk_lock_alm(p_almcb);
	if (p_almcb->almsta) {
		p_almcb->almsta = false;
		tmevtb_dequeue(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}	
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_ISTP_ALM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_istp_alm */

/*
 *  アラームハンドラの状態参照
 */
#ifdef TOPPERS_ref_alm

ER
ref_alm(ID almid, T_RALM *pk_ralm)
{
	ALMCB	*p_almcb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_REF_ALM_ENTER(almid, pk_ralm);
	CHECK_TSKCTX_UNL();
	CHECK_ALMID(almid);
	p_almcb = get_almcb(almid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_alm(p_almcb);
	if (p_almcb->almsta) {
		pk_ralm->almstat = TALM_STA;
		pk_ralm->lefttim = tmevt_lefttim(p_almcb->p_pcb->p_tevtcb, &(p_almcb->tmevtb));
	}
	else {
		pk_ralm->almstat = TALM_STP;
	}
	pk_ralm->prcid = p_almcb->p_pcb->prcid;
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_REF_ALM_LEAVE(ercd, pk_ralm);
	return(ercd);
}

#endif /* TOPPERS_ref_alm */

/*
 *  アラームハンドラ起動ルーチン
 */
#ifdef TOPPERS_almcal

void
call_almhdr(ALMCB *p_almcb)
{
	PRI		saved_ipm;
	PCB		*my_p_pcb;

	/*
	 *  アラームハンドラを停止状態にする．
	 */
	p_almcb->almsta = false;

	/*
	 *  アラームハンドラを，CPUロック解除状態で呼び出す．
	 */
	saved_ipm = i_get_ipm();
	release_tsk_lock(p_almcb->p_pcb);
	i_unlock_cpu();

	LOG_ALM_ENTER(p_almcb);
	(*((ALMHDR)(p_almcb->p_alminib->almhdr)))(p_almcb->p_alminib->exinf);
	LOG_ALM_LEAVE(p_almcb);

	if (!i_sense_lock()) {
		i_lock_cpu();
	}
	my_p_pcb = i_acquire_tsk_lock_self();

	i_set_ipm(saved_ipm);
	force_unlock_spin(my_p_pcb);
}

#endif /* TOPPERS_almcal */
