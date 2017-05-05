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
 *  @(#) $Id: cyclic.c 721 2010-10-14 02:53:19Z ertl-honda $
 */

/*
 *		周期ハンドラ機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "cyclic.h"
#include "mp.h"
#include "spin_lock.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_CYC_ENTER
#define LOG_CYC_ENTER(p_cyccb)
#endif /* LOG_CYC_ENTER */

#ifndef LOG_CYC_LEAVE
#define LOG_CYC_LEAVE(p_cyccb)
#endif /* LOG_CYC_LEAVE */

#ifndef LOG_STA_CYC_ENTER
#define LOG_STA_CYC_ENTER(cycid)
#endif /* LOG_STA_CYC_ENTER */

#ifndef LOG_STA_CYC_LEAVE
#define LOG_STA_CYC_LEAVE(ercd)
#endif /* LOG_STA_CYC_LEAVE */

#ifndef LOG_MSTA_CYC_ENTER
#define LOG_MSTA_CYC_ENTER(cycid, prcid)
#endif /* LOG_MSTA_CYC_ENTER */

#ifndef LOG_MSTA_CYC_LEAVE
#define LOG_MSTA_CYC_LEAVE(ercd)
#endif /* LOG_MSTA_CYC_LEAVE */

#ifndef LOG_CYCMIG
#define LOG_CYCMIG(p_cyccb, src_id, dest_id)
#endif /* LOG_CYCMIG */

#ifndef LOG_STP_CYC_ENTER
#define LOG_STP_CYC_ENTER(cycid)
#endif /* LOG_STP_CYC_ENTER */

#ifndef LOG_STP_CYC_LEAVE
#define LOG_STP_CYC_LEAVE(ercd)
#endif /* LOG_STP_CYC_LEAVE */

#ifndef LOG_REF_CYC_ENTER
#define LOG_REF_CYC_ENTER(cycid, pk_rcyc)
#endif /* LOG_REF_CYC_ENTER */

#ifndef LOG_REF_CYC_LEAVE
#define LOG_REF_CYC_LEAVE(ercd, pk_rcyc)
#endif /* LOG_REF_CYC_LEAVE */

/*
 *  周期ハンドラの数
 */
#define tnum_cyc	((uint_t)(tmax_cycid - TMIN_CYCID + 1))

/*
 *  周期ハンドラIDから周期ハンドラ管理ブロックを取り出すためのマクロ
 */
#define INDEX_CYC(cycid)	((uint_t)((cycid) - TMIN_CYCID))
#define get_cyccb(cycid)	(p_cyccb_table[INDEX_CYC(cycid)])

/*
 *  周期ハンドラ起動のためのタイムイベントブロックの登録
 */
Inline void
tmevtb_enqueue_cyc(CYCCB *p_cyccb, EVTTIM evttim)
{
	tmevtb_enqueue_evttim(p_cyccb->p_pcb->p_tevtcb, &(p_cyccb->tmevtb), evttim,
								(CBACK) call_cychdr, (void *) p_cyccb);
	p_cyccb->evttim = evttim;
}

/*
 *  周期ハンドラ機能の初期化
 */
#ifdef TOPPERS_cycini

void
initialize_cyclic(void)
{
	uint_t	i;
	CYCCB	*p_cyccb;
	PCB		*p_pcb;

#ifdef TOPPERS_SYSTIM_GLOBAL
	if (!x_sense_mprc()) {
		return;
	}
	p_pcb = get_mp_p_pcb(TOPPERS_SYSTIM_PRCID);
#else  /* TOPPERS_SYSTIM_LOCAL */
	p_pcb = get_my_p_pcb();
#endif /* TOPPERS_SYSTIM_GLOBAL */

	for (i = 0; i < tnum_cyc; i++) {
#ifdef TOPPERS_SYSTIM_LOCAL
		/* 自コアに割り付けれた周期ハンドラの初期化 */
		if(cycinib_table[i].iaffinity == ID_PRC(x_prc_index())){
#endif /* TOPPERS_SYSTIM_LOCAL */
			p_cyccb = p_cyccb_table[i];
			p_cyccb->p_cycinib = &(cycinib_table[i]);
			p_cyccb->p_pcb = p_pcb;
			if ((p_cyccb->p_cycinib->cycatr & TA_STA) != 0U) {
				p_cyccb->cycsta = true;
				tmevtb_enqueue_cyc(p_cyccb, (EVTTIM)(p_cyccb->p_cycinib->cycphs));
			}
			else {
				p_cyccb->cycsta = false;
			}
#ifdef TOPPERS_SYSTIM_LOCAL
		}
#endif /* TOPPERS_SYSTIM_LOCAL */
	}
}

#endif /* TOPPERS_cycini */

/*
 *  周期ハンドラの動作開始
 */
#ifdef TOPPERS_sta_cyc

ER
sta_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_STA_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_cyc(p_cyccb);
	if (p_cyccb->cycsta) {
		tmevtb_dequeue(p_cyccb->p_pcb->p_tevtcb, &(p_cyccb->tmevtb));
	}
	else {
		p_cyccb->cycsta = true;
	}
	tmevtb_enqueue_cyc(p_cyccb, base_time(p_cyccb->p_pcb->p_tevtcb) + p_cyccb->p_cycinib->cycphs);
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_STA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_sta_cyc */


/*
 *  周期ハンドラの動作開始
 */
#ifdef TOPPERS_msta_cyc

#ifdef TOPPERS_SYSTIM_LOCAL

ER
msta_cyc(ID cycid, ID prcid)
{
	CYCCB	*p_cyccb;
	ER		ercd;
	PCB		*t_p_pcb;
	PCB		*f_p_pcb;

	LOG_MSTA_CYC_ENTER(cycid, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	CHECK_PRCID_INI(prcid);
	p_cyccb = get_cyccb(cycid);
	prcid = (prcid == TPRC_INI)? p_cyccb->p_cycinib->iaffinity : prcid;
	CHECK_MIG(p_cyccb->p_cycinib->affinity_mask, prcid);

	t_lock_cpu();
	t_acquire_dual_tsk_lock_cyc(p_cyccb, prcid, &f_p_pcb, &t_p_pcb);
	if (p_cyccb->cycsta) {
		tmevtb_dequeue(p_cyccb->p_pcb->p_tevtcb, &(p_cyccb->tmevtb));
	}
	else {
		p_cyccb->cycsta = true;
	}

	LOG_CYCMIG(p_cyccb, f_p_pcb->prcid, prcid);
	/* 移動先の pcb を登録 */
	p_cyccb->p_pcb = t_p_pcb;

	tmevtb_enqueue_cyc(p_cyccb, base_time(t_p_pcb->p_tevtcb) + p_cyccb->p_cycinib->cycphs);
	ercd = E_OK;
	release_dual_tsk_lock(f_p_pcb, t_p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_MSTA_CYC_LEAVE(ercd);
	return(ercd);
}

#else  /* TOPPERS_SYSTIM_GLOBAL */

ER
msta_cyc(ID cycid, ID prcid)
{
	ER		ercd = E_NOSPT;

	LOG_MSTA_CYC_ENTER(cycid, prcid);
	LOG_MSTA_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SYSTIM_GLOBAL */

#endif /* TOPPERS_msta_cyc */


/*
 *  周期ハンドラの動作停止
 */
#ifdef TOPPERS_stp_cyc

ER
stp_cyc(ID cycid)
{
	CYCCB	*p_cyccb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_STP_CYC_ENTER(cycid);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_cyc(p_cyccb);
	if (p_cyccb->cycsta) {
		p_cyccb->cycsta = false;
		tmevtb_dequeue(p_cyccb->p_pcb->p_tevtcb, &(p_cyccb->tmevtb));
	}
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_STP_CYC_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_stp_cyc */

/*
 *  周期ハンドラの状態参照
 */
#ifdef TOPPERS_ref_cyc

ER
ref_cyc(ID cycid, T_RCYC *pk_rcyc)
{
	CYCCB	*p_cyccb;
	ER		ercd;
	PCB		*p_pcb;

	LOG_REF_CYC_ENTER(cycid, pk_rcyc);
	CHECK_TSKCTX_UNL();
	CHECK_CYCID(cycid);
	p_cyccb = get_cyccb(cycid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_cyc(p_cyccb);
	if (p_cyccb->cycsta) {
		pk_rcyc->cycstat = TCYC_STA;
		pk_rcyc->lefttim = tmevt_lefttim(p_cyccb->p_pcb->p_tevtcb, &(p_cyccb->tmevtb));
	}
	else {
		pk_rcyc->cycstat = TCYC_STP;
	}
	pk_rcyc->prcid = p_cyccb->p_pcb->prcid;
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_REF_CYC_LEAVE(ercd, pk_rcyc);
	return(ercd);
}

#endif /* TOPPERS_ref_cyc */

/*
 *  周期ハンドラ起動ルーチン
 */
#ifdef TOPPERS_cyccal

void
call_cychdr(CYCCB *p_cyccb)
{
	PRI		saved_ipm;
	PCB		*my_p_pcb;

	/*
	 *  次回の起動のためのタイムイベントブロックを登録する．
	 *
	 *  同じタイムティックで周期ハンドラを再度起動すべき場合には，この
	 *  関数からsignal_timeに戻った後に，再度この関数が呼ばれることにな
	 *  る．
	 */
	tmevtb_enqueue_cyc(p_cyccb, p_cyccb->evttim + p_cyccb->p_cycinib->cyctim);

	/*
	 *  周期ハンドラを，CPUロック解除状態で呼び出す．
	 */
	saved_ipm = i_get_ipm();
	release_tsk_lock(p_cyccb->p_pcb);
	i_unlock_cpu();

	LOG_CYC_ENTER(p_cyccb);
	(*((CYCHDR)(p_cyccb->p_cycinib->cychdr)))(p_cyccb->p_cycinib->exinf);
	LOG_CYC_LEAVE(p_cyccb);

	if (!i_sense_lock()) {
		i_lock_cpu();
	}

	my_p_pcb = i_acquire_tsk_lock_self();

	i_set_ipm(saved_ipm);
	force_unlock_spin(my_p_pcb);
}

#endif /* TOPPERS_cyccal */
