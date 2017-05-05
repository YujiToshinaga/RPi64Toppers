/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: interrupt.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		割込み管理機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "interrupt.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_DIS_INT_ENTER
#define LOG_DIS_INT_ENTER(intno)
#endif /* LOG_DIS_INT_ENTER */

#ifndef LOG_DIS_INT_LEAVE
#define LOG_DIS_INT_LEAVE(ercd)
#endif /* LOG_DIS_INT_LEAVE */

#ifndef LOG_ENA_INT_ENTER
#define LOG_ENA_INT_ENTER(intno)
#endif /* LOG_ENA_INT_ENTER */

#ifndef LOG_ENA_INT_LEAVE
#define LOG_ENA_INT_LEAVE(ercd)
#endif /* LOG_ENA_INT_LEAVE */

#ifndef LOG_CHG_IPM_ENTER
#define LOG_CHG_IPM_ENTER(intpri)
#endif /* LOG_CHG_IPM_ENTER */

#ifndef LOG_CHG_IPM_LEAVE
#define LOG_CHG_IPM_LEAVE(ercd)
#endif /* LOG_CHG_IPM_LEAVE */

#ifndef LOG_GET_IPM_ENTER
#define LOG_GET_IPM_ENTER(p_intpri)
#endif /* LOG_GET_IPM_ENTER */

#ifndef LOG_GET_IPM_LEAVE
#define LOG_GET_IPM_LEAVE(ercd, intpri)
#endif /* LOG_GET_IPM_LEAVE */

/* 
 *  割込み管理機能の初期化
 */
#ifdef TOPPERS_intini
#ifndef OMIT_INITIALIZE_INTERRUPT

void
initialize_interrupt(void)
{
	uint_t			i;
	const INHINIB	*p_inhinib;
	const INTINIB	*p_intinib;

	for (i = 0; i < tnum_inhno; i++) {
		p_inhinib = &(inhinib_table[i]);
		if (p_inhinib->iaffinity == ID_PRC(x_prc_index())) {
			x_define_inh(p_inhinib->inhno, p_inhinib->int_entry, p_inhinib->affinity_mask);
		}
	}

	for (i = 0; i < tnum_intno; i++) {
		p_intinib = &(intinib_table[i]);
		if (p_intinib->iaffinity == ID_PRC(x_prc_index())) {
			x_config_int(p_intinib->intno, p_intinib->intatr, p_intinib->intpri, p_intinib->affinity_mask);
		}
	}
}

#endif /* OMIT_INITIALIZE_INTERRUPT */
#endif /* TOPPERS_intini */

/*
 *  割込みの禁止
 */
#ifdef TOPPERS_dis_int
#ifdef TOPPERS_SUPPORT_DIS_INT

ER
dis_int(INTNO intno)
{
	bool_t	locked;
	ER		ercd;

	LOG_DIS_INT_ENTER(intno);
	CHECK_TSKCTX();

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	if (!VALID_INTNO_DISINT(ID_PRC(x_prc_index()), intno)) {
		ercd = E_PAR;
	}
	else if (t_disable_int(intno)) {
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	if (!locked) {
		t_unlock_cpu();
	}

  error_exit:
	LOG_DIS_INT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SUPPORT_DIS_INT */
#endif /* TOPPERS_dis_int */

/*
 *  割込みの許可
 */
#ifdef TOPPERS_ena_int
#ifdef TOPPERS_SUPPORT_ENA_INT

ER
ena_int(INTNO intno)
{
	bool_t	locked;
	ER		ercd;

	LOG_ENA_INT_ENTER(intno);
	CHECK_TSKCTX();

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	if (!VALID_INTNO_DISINT(ID_PRC(x_prc_index()), intno)) {
		ercd = E_PAR;
	}
	else if (t_enable_int(intno)) {
		ercd = E_OK;
	}
	else {
		ercd = E_OBJ;
	}
	if (!locked) {
		t_unlock_cpu();
	}

  error_exit:
	LOG_ENA_INT_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_SUPPORT_ENA_INT */
#endif /* TOPPERS_ena_int */

/*
 *  割込み優先度マスクの変更
 */
#ifdef TOPPERS_chg_ipm

ER
chg_ipm(PRI intpri)
{
	ER		ercd;
	PCB		*my_p_pcb;
	bool_t	dspreq = false;

	LOG_CHG_IPM_ENTER(intpri);
	CHECK_TSKCTX_UNL();
	CHECK_INTPRI_CHGIPM(intpri);

	t_lock_cpu();
	my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	t_set_ipm(intpri);
	if (intpri == TIPM_ENAALL) {
		my_p_pcb->ipmflg = true;
		if (!(my_p_pcb->disdsp)) {
			my_p_pcb->dspflg = true;
			if (my_p_pcb->p_runtsk != my_p_pcb->p_schedtsk) {
				dspreq = true;
			}
		}
		/*
		 * タスク例外処理ルーチンの起動条件がそろっている場合でも，dspreq が
		 * trueの場合はディスパッチを優先して実行するため呼び出さない．
		 * この状況はIPMが-1以下に設定された状態でsus_tsk()を発行され過渡状態
		 * となったタスクが，chg_ipm(TIPM_ENAALL) を呼び出した場合に発生する．
		 * dspreq のチェックをしないと，設定されたIPMよりプロセッサ間割込みの
		 * 優先度が低い場合は割込みが保留されており，call_texrtn()の先でCPUロ
		 * ックを解除したときに割込みが入りディスパッチするが，プロセッサ間割
		 * 込みの優先度が高い場合は，割込みを受け付けるが割込みの出口でディス
		 * パッチしない．そのため，call_texrtn()の先で割込みを受け付けず，タス
		 * ク例外処理ルーチンを実行してしまい，振る舞いがプロセッサ間割込みの
		 * 優先度により異なってしまう．
		 */
		if (my_p_pcb->p_runtsk->enatex && my_p_pcb->p_runtsk->texptn != 0U
			&& dspreq != true) {
			call_texrtn(my_p_pcb);
			/* マイグレーションしている可能性があるため再取得 */
			my_p_pcb = get_my_p_pcb();
		}
	}
	else {
		my_p_pcb->ipmflg = false;
		my_p_pcb->dspflg = false;
	}
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_CHG_IPM_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_chg_ipm */

/*
 *  割込み優先度マスクの参照
 */
#ifdef TOPPERS_get_ipm

ER
get_ipm(PRI *p_intpri)
{
	ER		ercd;

	LOG_GET_IPM_ENTER(p_intpri);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	*p_intpri = t_get_ipm();
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_IPM_LEAVE(ercd, *p_intpri);
	return(ercd);
}

#endif /* TOPPERS_get_ipm */
