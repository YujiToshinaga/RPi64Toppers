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
 *  @(#) $Id: sys_manage.c 792 2011-03-10 14:18:33Z ertl-honda $
 */

/*
 *		システム状態管理機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_ROT_RDQ_ENTER
#define LOG_ROT_RDQ_ENTER(tskpri)
#endif /* LOG_ROT_RDQ_ENTER */

#ifndef LOG_ROT_RDQ_LEAVE
#define LOG_ROT_RDQ_LEAVE(ercd)
#endif /* LOG_ROT_RDQ_LEAVE */

#ifndef LOG_IROT_RDQ_ENTER
#define LOG_IROT_RDQ_ENTER(tskpri)
#endif /* LOG_IROT_RDQ_ENTER */

#ifndef LOG_IROT_RDQ_LEAVE
#define LOG_IROT_RDQ_LEAVE(ercd)
#endif /* LOG_IROT_RDQ_LEAVE */

#ifndef LOG_MROT_RDQ_ENTER
#define LOG_MROT_RDQ_ENTER(tskpri, prcid)
#endif /* LOG_MROT_RDQ_ENTER */

#ifndef LOG_MROT_RDQ_LEAVE
#define LOG_MROT_RDQ_LEAVE(ercd)
#endif /* LOG_MROT_RDQ_LEAVE */

#ifndef LOG_IMROT_RDQ_ENTER
#define LOG_IMROT_RDQ_ENTER(tskpri, prcid)
#endif /* LOG_IMROT_RDQ_ENTER */

#ifndef LOG_IMROT_RDQ_LEAVE
#define LOG_IMROT_RDQ_LEAVE(ercd)
#endif /* LOG_IMROT_RDQ_LEAVE */

#ifndef LOG_GET_TID_ENTER
#define LOG_GET_TID_ENTER(p_tskid)
#endif /* LOG_GET_TID_ENTER */

#ifndef LOG_GET_TID_LEAVE
#define LOG_GET_TID_LEAVE(ercd, tskid)
#endif /* LOG_GET_TID_LEAVE */

#ifndef LOG_IGET_TID_ENTER
#define LOG_IGET_TID_ENTER(p_tskid)
#endif /* LOG_IGET_TID_ENTER */

#ifndef LOG_IGET_TID_LEAVE
#define LOG_IGET_TID_LEAVE(ercd, tskid)
#endif /* LOG_IGET_TID_LEAVE */

#ifndef LOG_GET_PID_ENTER
#define LOG_GET_PID_ENTER(p_prcid)
#endif /* LOG_GET_PID_ENTER */

#ifndef LOG_GET_PID_LEAVE
#define LOG_GET_PID_LEAVE(ercd, p_prcid)
#endif /* LOG_GET_PID_LEAVE */

#ifndef LOG_IGET_PID_ENTER
#define LOG_IGET_PID_ENTER(p_prcid)
#endif /* LOG_IGET_PID_ENTER */

#ifndef LOG_IGET_PID_LEAVE
#define LOG_IGET_PID_LEAVE(ercd, prcid)
#endif /* LOG_IGET_PID_LEAVE */

#ifndef LOG_LOC_CPU_ENTER
#define LOG_LOC_CPU_ENTER()
#endif /* LOG_LOC_CPU_ENTER */

#ifndef LOG_LOC_CPU_LEAVE
#define LOG_LOC_CPU_LEAVE(ercd)
#endif /* LOG_LOC_CPU_LEAVE */

#ifndef LOG_ILOC_CPU_ENTER
#define LOG_ILOC_CPU_ENTER()
#endif /* LOG_ILOC_CPU_ENTER */

#ifndef LOG_ILOC_CPU_LEAVE
#define LOG_ILOC_CPU_LEAVE(ercd)
#endif /* LOG_ILOC_CPU_LEAVE */

#ifndef LOG_UNL_CPU_ENTER
#define LOG_UNL_CPU_ENTER()
#endif /* LOG_UNL_CPU_ENTER */

#ifndef LOG_UNL_CPU_LEAVE
#define LOG_UNL_CPU_LEAVE(ercd)
#endif /* LOG_UNL_CPU_LEAVE */

#ifndef LOG_IUNL_CPU_ENTER
#define LOG_IUNL_CPU_ENTER()
#endif /* LOG_IUNL_CPU_ENTER */

#ifndef LOG_IUNL_CPU_LEAVE
#define LOG_IUNL_CPU_LEAVE(ercd)
#endif /* LOG_IUNL_CPU_LEAVE */

#ifndef LOG_DIS_DSP_ENTER
#define LOG_DIS_DSP_ENTER()
#endif /* LOG_DIS_DSP_ENTER */

#ifndef LOG_DIS_DSP_LEAVE
#define LOG_DIS_DSP_LEAVE(ercd)
#endif /* LOG_DIS_DSP_LEAVE */

#ifndef LOG_ENA_DSP_ENTER
#define LOG_ENA_DSP_ENTER()
#endif /* LOG_ENA_DSP_ENTER */

#ifndef LOG_ENA_DSP_LEAVE
#define LOG_ENA_DSP_LEAVE(ercd)
#endif /* LOG_ENA_DSP_LEAVE */

#ifndef LOG_SNS_CTX_ENTER
#define LOG_SNS_CTX_ENTER()
#endif /* LOG_SNS_CTX_ENTER */

#ifndef LOG_SNS_CTX_LEAVE
#define LOG_SNS_CTX_LEAVE(state)
#endif /* LOG_SNS_CTX_LEAVE */

#ifndef LOG_SNS_LOC_ENTER
#define LOG_SNS_LOC_ENTER()
#endif /* LOG_SNS_LOC_ENTER */

#ifndef LOG_SNS_LOC_LEAVE
#define LOG_SNS_LOC_LEAVE(state)
#endif /* LOG_SNS_LOC_LEAVE */

#ifndef LOG_SNS_DSP_ENTER
#define LOG_SNS_DSP_ENTER()
#endif /* LOG_SNS_DSP_ENTER */

#ifndef LOG_SNS_DSP_LEAVE
#define LOG_SNS_DSP_LEAVE(state)
#endif /* LOG_SNS_DSP_LEAVE */

#ifndef LOG_SNS_DPN_ENTER
#define LOG_SNS_DPN_ENTER()
#endif /* LOG_SNS_DPN_ENTER */

#ifndef LOG_SNS_DPN_LEAVE
#define LOG_SNS_DPN_LEAVE(state)
#endif /* LOG_SNS_DPN_LEAVE */

#ifndef LOG_SNS_KER_ENTER
#define LOG_SNS_KER_ENTER()
#endif /* LOG_SNS_KER_ENTER */

#ifndef LOG_SNS_KER_LEAVE
#define LOG_SNS_KER_LEAVE(state)
#endif /* LOG_SNS_KER_LEAVE */

/*
 *  タスクの優先順位の回転
 */
#ifdef TOPPERS_rot_rdq

ER
rot_rdq(PRI tskpri)
{
	uint_t	pri;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*my_p_pcb;

	LOG_ROT_RDQ_ENTER(tskpri);
	CHECK_TSKCTX_UNL();
	CHECK_TPRI_SELF(tskpri);

	t_lock_cpu();
	my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	pri = (tskpri == TPRI_SELF) ? my_p_pcb->p_runtsk->priority : INT_PRIORITY(tskpri);
	dspreq = rotate_ready_queue(pri, my_p_pcb);
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_ROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_rot_rdq */

/*
 *  タスクの優先順位の回転（非タスクコンテキスト用）
 */
#ifdef TOPPERS_irot_rdq

ER
irot_rdq(PRI tskpri)
{
	ER		ercd;
	PCB		*my_p_pcb;

	LOG_IROT_RDQ_ENTER(tskpri);
	CHECK_INTCTX_UNL();
	CHECK_TPRI(tskpri);

	i_lock_cpu();
	my_p_pcb = i_acquire_tsk_lock_self();
	if (rotate_ready_queue(INT_PRIORITY(tskpri), my_p_pcb)) {
		my_p_pcb->reqflg = true;
	}
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_irot_rdq */

/*
 *  タスクの優先順位の回転（他プロセッサ用）
 */
#ifdef TOPPERS_mrot_rdq

ER
mrot_rdq(PRI tskpri, ID prcid)
{
	uint_t	pri;
	ER		ercd;
	bool_t	dspreq = false;
	PCB		*p_pcb;
	
	LOG_MROT_RDQ_ENTER(tskpri, prcid);
	CHECK_TSKCTX_UNL();
	CHECK_TPRI_SELF(tskpri);
	CHECK_PRCID(prcid);

	t_lock_cpu();
	p_pcb = t_acquire_tsk_lock_prcid(prcid);
	pri = (tskpri == TPRI_SELF) ? (get_my_p_pcb())->p_runtsk->priority : INT_PRIORITY(tskpri);
	if (rotate_ready_queue(pri, p_pcb)) {
		dspreq = dispatch_request(p_pcb);
	}
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_MROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_mrot_rdq */

/*
 *  タスクの優先順位の回転（非タスクコンテキスト/他プロセッサ用）
 */
#ifdef TOPPERS_imrot_rdq

ER
imrot_rdq(PRI tskpri, ID prcid)
{
	ER		ercd;
	PCB		*p_pcb;
	PCB		*my_p_pcb;

	LOG_IMROT_RDQ_ENTER(tskpri, prcid);
	CHECK_INTCTX_UNL();
	CHECK_TPRI(tskpri);
	CHECK_PRCID(prcid);

	i_lock_cpu();
	p_pcb = i_acquire_tsk_lock_prcid(prcid);
	if (rotate_ready_queue(INT_PRIORITY(tskpri), p_pcb)) {
		my_p_pcb = get_my_p_pcb();
		my_p_pcb->reqflg = dispatch_request(p_pcb) ? true : my_p_pcb->reqflg;
	}
	ercd = E_OK;
	release_tsk_lock(p_pcb);
	i_unlock_cpu();

  error_exit:
	LOG_IMROT_RDQ_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_imrot_rdq */

/*
 *  実行状態のタスクIDの参照
 */
#ifdef TOPPERS_get_tid

ER
get_tid(ID *p_tskid)
{
	ER		ercd;
	PCB		*my_p_pcb;

	LOG_GET_TID_ENTER(p_tskid);
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	*p_tskid = TSKID(my_p_pcb->p_runtsk);
	ercd = E_OK;
	t_unlock_cpu();

  error_exit:
	LOG_GET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* TOPPERS_get_tid */

/*
 *  実行状態のタスクIDの参照（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iget_tid

ER
iget_tid(ID *p_tskid)
{
	ER		ercd;
	PCB		*my_p_pcb;

	LOG_IGET_TID_ENTER(p_tskid);
	CHECK_INTCTX_UNL();

	i_lock_cpu();
	my_p_pcb = get_my_p_pcb();
	*p_tskid = (my_p_pcb->p_runtsk == NULL) ? TSK_NONE : TSKID(my_p_pcb->p_runtsk);
	ercd = E_OK;
	i_unlock_cpu();

  error_exit:
	LOG_IGET_TID_LEAVE(ercd, *p_tskid);
	return(ercd);
}

#endif /* TOPPERS_iget_tid */

/*
 *  実行状態のプロセッサIDの参照
 */
#ifdef TOPPERS_get_pid

ER
get_pid(ID *p_prcid)
{
	ER		ercd;

	LOG_GET_PID_ENTER(p_prcid);
	CHECK_TSKCTX_UNL();

	*p_prcid = (get_my_p_pcb())->prcid;
	ercd = E_OK;

  error_exit:
	LOG_GET_PID_LEAVE(ercd, *p_prcid);
	return(ercd);
}

#endif /* TOPPERS_get_pid */

/*
 *  実行状態のプロセッサIDの参照（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iget_pid

ER
iget_pid(ID *p_prcid)
{
	ER		ercd;

	LOG_IGET_PID_ENTER(p_prcid);
	CHECK_INTCTX_UNL();

	*p_prcid = (get_my_p_pcb())->prcid;
	ercd = E_OK;

  error_exit:
	LOG_IGET_PID_LEAVE(ercd, *p_prcid);
	return(ercd);
}

#endif /* TOPPERS_iget_pid */


/*
 *  CPUロック状態への移行
 */
#ifdef TOPPERS_loc_cpu

ER
loc_cpu(void)
{
	ER		ercd;

	LOG_LOC_CPU_ENTER();
	CHECK_TSKCTX();

	if (!t_sense_lock()) {
		t_lock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_LOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_cpu */

/*
 *  CPUロック状態への移行（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iloc_cpu

ER
iloc_cpu(void)
{
	ER		ercd;

	LOG_ILOC_CPU_ENTER();
	CHECK_INTCTX();

	if (!i_sense_lock()) {
		i_lock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_ILOC_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iloc_cpu */

/*
 *  CPUロック状態の解除
 *
 *  CPUロック中は，ディスパッチが必要となるサービスコールを呼び出すこ
 *  とはできないため，CPUロック状態の解除時にディスパッチャを起動する
 *  必要はない．
 *  スピンロックを取得している間（PCB中のlockspnid != 0）は状態は変化しない． 
 */
#ifdef TOPPERS_unl_cpu

ER
unl_cpu(void)
{
	ER		ercd;

	LOG_UNL_CPU_ENTER();
	CHECK_TSKCTX();

	if (t_sense_lock() && ((get_my_p_pcb())->locspnid == 0)) {
		t_unlock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_UNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_cpu */

/*
 *  CPUロック状態の解除（非タスクコンテキスト用）
 *
 *  CPUロック中は，ディスパッチが必要となるサービスコールを呼び出すこ
 *  とはできないため，CPUロック状態の解除時にディスパッチャの起動を要
 *  求する必要はない．
 */
#ifdef TOPPERS_iunl_cpu

ER
iunl_cpu(void)
{
	ER		ercd;

	LOG_IUNL_CPU_ENTER();
	CHECK_INTCTX();
    
	if (i_sense_lock() && ((get_my_p_pcb())->locspnid == 0)) {
		i_unlock_cpu();
	}
	ercd = E_OK;

  error_exit:
	LOG_IUNL_CPU_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iunl_cpu */

/*
 *  ディスパッチの禁止
 */
#ifdef TOPPERS_dis_dsp

ER
dis_dsp(void)
{
	ER		ercd;
	PCB		*my_p_pcb;

	LOG_DIS_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	my_p_pcb->disdsp = true;
	my_p_pcb->dspflg = false;
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	t_unlock_cpu();

  error_exit:
	LOG_DIS_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_dis_dsp */

/*
 *  ディスパッチの許可
 */
#ifdef TOPPERS_ena_dsp

ER
ena_dsp(void)
{
	ER		ercd;
	bool_t dspreq = false;
	PCB		*my_p_pcb;

	LOG_ENA_DSP_ENTER();
	CHECK_TSKCTX_UNL();

	t_lock_cpu();
	my_p_pcb = t_acquire_tsk_lock_self_without_runnable_check();
	my_p_pcb->disdsp = false;
	if (my_p_pcb->ipmflg) {
		my_p_pcb->dspflg = true;
		if (my_p_pcb->p_runtsk != my_p_pcb->p_schedtsk) {
			dspreq = true;
		}
	}
	ercd = E_OK;
	release_tsk_lock(my_p_pcb);
	if (dspreq) {
		dispatch();
	}
	t_unlock_cpu();

  error_exit:
	LOG_ENA_DSP_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_ena_dsp */

/*
 *  コンテキストの参照
 */
#ifdef TOPPERS_sns_ctx

bool_t
sns_ctx(void)
{
	bool_t	state;

	LOG_SNS_CTX_ENTER();
	state = sense_context() ? true : false;
	LOG_SNS_CTX_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_ctx */

/*
 *  CPUロック状態の参照
 */
#ifdef TOPPERS_sns_loc

bool_t
sns_loc(void)
{
	bool_t	state;

	LOG_SNS_LOC_ENTER();
	state = x_sense_lock() ? true : false;
	LOG_SNS_LOC_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_loc */

/*
 *  ディスパッチ禁止状態の参照
 */
#ifdef TOPPERS_sns_dsp

bool_t
sns_dsp(void)
{
	bool_t	state;
	bool_t	locked;

	LOG_SNS_DSP_ENTER();

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	state = (get_my_p_pcb())->disdsp;
	if (!locked) {
		t_unlock_cpu();
	}

	LOG_SNS_DSP_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_dsp */

/*
 *  ディスパッチ保留状態の参照
 */
#ifdef TOPPERS_sns_dpn

bool_t
sns_dpn(void)
{
	bool_t	state;
	bool_t	locked;

	LOG_SNS_DPN_ENTER();

	/*
	 *  PCBへアクセスするためCPUロック状態とする
	 */
	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	state = (sense_context() || locked || !((get_my_p_pcb())->dspflg)) ? true : false;
	if (!locked) {
		t_unlock_cpu();
	}

	LOG_SNS_DPN_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_dpn */

/*
 *  カーネル非動作状態の参照
 */
#ifdef TOPPERS_sns_ker

bool_t
sns_ker(void)
{
	bool_t	state;

	SIL_PRE_LOC;

	LOG_SNS_KER_ENTER();

	/*
	 *  PCBへアクセスするため割込みを禁止する
	 *  sns_ker()は全割込みロック状態で呼び出される場合が
	 *  あるため，SILを用いる．
	 */
	SIL_LOC_INT();
	state = (get_my_p_pcb())->kerflg ? false : true;
	SIL_UNL_INT();

	LOG_SNS_KER_LEAVE(state);
	return(state);
}

#endif /* TOPPERS_sns_ker */
