/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2007-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: trace_dump.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		トレースログのダンプ
 */

#include "kernel_impl.h"
#include "task.h"
#include "cyclic.h"
#include "alarm.h"
#include <log_output.h>

/* 
 *  カーネル情報の取出し
 */
static intptr_t
get_tskid(intptr_t info)
{
	TCB		*p_tcb;
	ID		tskid;

	p_tcb = (TCB *) info;
	if (p_tcb == NULL) {
		tskid = 0;
	}
	else {
		tskid = TSKID(p_tcb);
	}
	return((intptr_t) tskid);
}

static intptr_t
get_tskstat(intptr_t info)
{
	uint_t		tstat;
	const char	*tstatstr;

	tstat = (uint_t) info;
	switch (tstat & (TS_RUNNABLE|TS_WAITING|TS_SUSPENDED)) {
	case TS_DORMANT:
		tstatstr = "DORMANT";
		break;
	case TS_RUNNABLE:
		tstatstr = "RUNNABLE";
		break;
	case TS_WAITING:
		tstatstr = "WAITING";
		break;
	case TS_SUSPENDED:
		tstatstr = "SUSPENDED";
		break;
	case TS_WAITING|TS_SUSPENDED:
		tstatstr = "WAITING-SUSPENDED";
		break;
	default:
		tstatstr = "unknown state";
		break;
	}
	return((intptr_t) tstatstr);
}

/*
 *  タスク管理・タスク付属同期機能 - 入口ログ
 */
static intptr_t
trace_print_tskenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
          
		/* タスク管理機能 */
	  case TFN_MIG_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to mig_tsk tskid=%d. prcid=%d.";
		break;
	  case TFN_ACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to act_tsk tskid=%d.";
		break;
	  case TFN_IACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iact_tsk tskid=%d.";
		break;
	  case TFN_MACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to mact_tsk tskid=%d. prcid=%d.";
		break;
	  case TFN_IMACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to imact_tsk tskid=%d. prcid=%d.";
		break;
	  case TFN_CAN_ACT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to can_act tskid=%d.";
		break;
	  case TFN_EXT_TSK:
		tracemsg = "enter to ext_tsk.";
		break;
	  case TFN_TER_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ter_tsk tskid=%d.";
		break;
	  case TFN_CHG_PRI:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to chg_pri tskid=%d. tskpri=%d.";
		break;
	  case TFN_GET_PRI:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to get_pri tskid=%d. p_tskpri=%d.";
		break;
	  case TFN_GET_INF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_inf p_exinf=%d.";
		break;

		/*  タスク状態参照機能 */
	  case TFN_REF_TSK: 
		info[0] = (intptr_t)trace->loginfo[1]; 
		tracemsg = "enter to ref_tsk tskid=%d."; 
		break; 
        
		/* タスク付属同期機能 */
	case TFN_SLP_TSK:
		tracemsg = "enter to slp_tsk.";
		break;
	  case TFN_TSLP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to tslp_tsk tmout=%d.";
		break;
	  case TFN_WUP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to wup_tsk tskid=%d.";
		break;
	  case TFN_IWUP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iwup_tsk tskid=%d.";
		break;
	  case TFN_CAN_WUP:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to can_wup tskid=%d.";
		break;
	  case TFN_REL_WAI:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to rel_wai tskid=%d.";
		break;
	  case TFN_IREL_WAI:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to irel_wai tskid=%d.";
		break;
	  case TFN_SUS_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to sus_tsk tskid=%d.";
		break;
	  case TFN_RSM_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to rsm_tsk tskid=%d.";
		break;
	  case TFN_DLY_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to dly_tsk dlytim=%d.";
		break;
	  default:
		tracemsg = "unknown tsk service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  タスク管理・タスク付属同期機能 - 出口ログ
 */
static intptr_t
trace_print_tskleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {

		/* タスク管理機能 */
	  case TFN_MIG_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from mig_tsk ercd=%d.";
		break;
	  case TFN_ACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from act_tsk ercd=%d.";
		break;
	  case TFN_IACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iact_tsk ercd=%d.";
		break;
	  case TFN_MACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from mact_tsk ercd=%d.";
		break;
	  case TFN_IMACT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from imact_tsk ercd=%d.";
		break;
	  case TFN_CAN_ACT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from can_act ercd=%d.";
		break;
	  case TFN_EXT_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ext_tsk ercd=%d.";
		break;
	  case TFN_TER_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ter_tsk ercd=%d.";
		break;
	  case TFN_CHG_PRI:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from chg_pri ercd=%d.";
		break;
	  case TFN_GET_PRI:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_pri ercd=%d. tskpri=%d.";
		break;
	  case TFN_GET_INF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_inf ercd=%d. exinf=%d.";
		break;

		/*  タスク状態参照機能 */
	  case TFN_REF_TSK: 
		info[0] = (intptr_t)trace->loginfo[1]; 
		tracemsg = "leave from ref_tsk ercd=%d."; 
		break;
        
		/* タスク付属同期機能 */
	  case TFN_SLP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from slp_tsk ercd=%d.";
		break;
	  case TFN_TSLP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from tslp_tsk ercd=%d.";
		break;
	  case TFN_WUP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from wup_tsk ercd=%d.";
		break;
	  case TFN_IWUP_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iwup_tsk ercd=%d.";
		break;
	  case TFN_CAN_WUP:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from can_wup ercd=%d.";
		break;
	  case TFN_REL_WAI:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from rel_wai ercd=%d.";
		break;
	  case TFN_IREL_WAI:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from irel_wai ercd=%d.";
		break;
	  case TFN_SUS_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sus_tsk ercd=%d.";
		break;
	case TFN_RSM_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from rsm_tsk ercd=%d.";
		break;
	case TFN_DLY_TSK:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from dly_tsk ercd=%d.";
		break;
	default:
		tracemsg = "unknown tsk service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  タスク例外処理機能 - 入口ログ
 */
static intptr_t
trace_print_texenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_RAS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ras_tex tskid=%d. rasptn=%d.";
		break;
	  case TFN_IRAS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iras_tex tskid=%d. rasptn=%d.";
		break;
	  case TFN_DIS_TEX:
		tracemsg = "enter to dis_tex.";
		break;
	  case TFN_ENA_TEX:
		tracemsg = "enter to ena_tex.";
		break;
	  case TFN_SNS_TEX:
		tracemsg = "enter to sns_tex.";
		break;
	  case TFN_REF_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_tex tskid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  タスク例外処理機能 - 出口ログ
 */
static intptr_t
trace_print_texleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_RAS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ras_tex ercd=%d.";
		break;
	  case TFN_IRAS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iras_tex ercd=%d.";
		break;
	  case TFN_DIS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from dis_tex ercd=%d.";
		break;
	  case TFN_ENA_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ena_tex ercd=%d";
		break;
	  case TFN_SNS_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_tex state=%d.";
		break;
	  case TFN_REF_TEX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_tex ercd=%d.";
		break;
	  default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  セマフォ機能 - 入口ログ
 */
static intptr_t
trace_print_sementer(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	case TFN_SIG_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to sig_sem semid=%d.";
		break;
        case TFN_ISIG_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to isig_sem semid=%d.";
		break;
	  case TFN_WAI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to wai_sem semid=%d.";
		break;
	  case TFN_POL_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to pol_sem semid=%d.";
		break;
	  case TFN_TWAI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to twai_sem semid=%d.";
		break;
	  case TFN_INI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_sem semid=%d.";
		break;
	  case TFN_REF_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_sem semid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  セマフォ機能 - 出口ログ
 */
static intptr_t
trace_print_semleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SIG_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sig_sem ercd=%d.";
		break;
	  case TFN_ISIG_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from isig_sem ercd=%d.";
		break;
	  case TFN_WAI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from wai_sem ercd=%d.";
		break;
	  case TFN_POL_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from pol_sem ercd=%d.";
		break;
	  case TFN_TWAI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from twai_sem ercd=%d.";
		break;
	  case TFN_INI_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_sem ercd=%d.";
		break;
	  case TFN_REF_SEM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_sem ercd=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  イベントフラグ機能 - 入口ログ
 */
static intptr_t
trace_print_flgenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SET_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to set_flg flgid=%d. setptn=%d.";
		break;
	  case TFN_ISET_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to iset_flg flgid=%d. setptn=%d.";
		break;
	  case TFN_CLR_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to clr_flg flgid=%d. clrptn=%d.";
		break;
	  case TFN_WAI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		info[3] = (intptr_t)trace->loginfo[4];
		tracemsg = "enter to wai_flg flgid=%d. waiptn=%d. wfmode=%d. p_flgptn=%d.";
		break;
	  case TFN_POL_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		info[3] = (intptr_t)trace->loginfo[4];
		tracemsg = "enter to pol_flg flgid=%d. waiptn=%d. wfmode=%d. p_flgptn=%d.";
		break;
	  case TFN_TWAI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		info[3] = (intptr_t)trace->loginfo[4];
		info[4] = (intptr_t)trace->loginfo[5];
		tracemsg = "enter to twai_flg flgid=%d. waiptn=%d. wfmode=%d. p_flgptn=%d. tmout=%d.";
		break;
	  case TFN_INI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_flg flgid=%d.";
		break;
	  case TFN_REF_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_flg flgid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  イベントフラグ機能 - 出口ログ
 */
static intptr_t
trace_print_flgleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SET_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from set_flg ercd=%d.";
		break;
	  case TFN_ISET_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iset_flg ercd=%d.";
		break;
	  case TFN_CLR_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from clr_flg ercd=%d.";
		break;
	  case TFN_WAI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from wai_flg ercd=%d. flgptn=%d";
		break;
	  case TFN_POL_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from pol_flg ercd=%d. flgptn=%d.";
		break;
	  case TFN_TWAI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from twai_flg ercd=%d. flgptn=%d.";
		break;
	  case TFN_INI_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_flg ercd=%d.";
		break;
	  case TFN_REF_FLG:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_flg ercd=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  データキュー機能 - 入口ログ
 */
static intptr_t
trace_print_dtqenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to snd_dtq dtqid=%d. data=%d.";
		break;
	  case TFN_PSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to psnd_dtq dtqid=%d. data=%d.";
		break;
	  case TFN_IPSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to ipsnd_dtq dtqid=%d. data=%d.";
		break;
	  case TFN_TSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to tsnd_dtq dtqid=%d. data=%d. tmout=%d.";
		break;
	  case TFN_FSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to fsnd_dtq dtqid=%d. data=%d.";
		break;
	  case TFN_IFSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to ifsnd_dtq dtqid=%d. data=%d.";
		break;
	  case TFN_RCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to rcv_dtq dtqid=%d. p_data=%d.";
		break;
	  case TFN_PRCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to prcv_dtq dtqid=%d. p_data=%d.";
		break;
	  case TFN_TRCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to trcv_dtq dtqid=%d. p_data=%d. tmout=%d.";
		break;
	  case TFN_INI_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_dtq dtqid=%d.";
		break;
	  case TFN_REF_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_dtq dtqid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  データキュー機能 - 出口ログ
 */
static intptr_t
trace_print_dtqleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from snd_dtq ercd=%d.";
		break;
	  case TFN_PSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from psnd_dtq ercd=%d.";
		break;
	  case TFN_IPSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ipsnd_dtq ercd=%d.";
		break;
	  case TFN_TSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from tsnd_dtq ercd=%d.";
		break;
	  case TFN_FSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from fsnd_dtq ercd=%d.";
		break;
	  case TFN_IFSND_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ifsnd_dtq ercd=%d.";
		break;
	  case TFN_RCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from rcv_dtq ercd=%d. data=%d.";
		break;
	  case TFN_PRCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from prcv_dtq ercd=%d. data=%d.";
		break;
	  case TFN_TRCV_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from trcv_dtq ercd=%d. data=%d.";
		break;
	  case TFN_INI_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_dtq ercd=%d.";
		break;
	  case TFN_REF_DTQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_dtq ercd=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}

/*
 *  優先度データキュー機能 -入口ログ
 */
static intptr_t
trace_print_pdqenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to snd_pdq pdqid=%d. data=%d. datapri=%d.";
		break;
	  case TFN_PSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to psnd_pdq pdqid=%d. data=%d. datapri=%d.";
		break;
	  case TFN_IPSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to ipsnd_pdq pdqid=%d. data=%d. datapri=%d.";
		break;
	  case TFN_TSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		info[3] = (intptr_t)trace->loginfo[4];
		tracemsg = "enter to tsnd_pdq pdqid=%d. data=%d. datapri=%d. tmout=%d.";
		break;
	  case TFN_RCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to rcv_pdq pdqid=%d. p_data=%d. p_datapri=%d.";
		break;
	  case TFN_PRCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to prcv_pdq pdqid=%d. p_data=%d. p_datapri=%d.";
		break;
	  case TFN_TRCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		info[3] = (intptr_t)trace->loginfo[4];
		tracemsg = "enter to trcv_pdq pdqid=%d. p_data=%d. p_datapri=%d. tmout=%d.";
		break;
	  case TFN_INI_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_pdq pdqid=%d.";
		break;
	  case TFN_REF_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_pdq pdqid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  優先度データキュー機能 - 出口ログ
 */
static intptr_t
trace_print_pdqleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from snd_pdq ercd=%d.";
		break;
	  case TFN_PSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from psnd_pdq ercd=%d.";
		break;
	  case TFN_IPSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ipsnd_pdq ercd=%d.";
		break;
	  case TFN_TSND_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from tsnd_pdq ercd=%d.";
		break;
	  case TFN_RCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "leave from rcv_pdq ercd=%d. data=%d. datapri=%d.";
		break;
	  case TFN_PRCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "leave from prcv_pdq ercd=%d. data=%d. datapri=%d.";
		break;
	  case TFN_TRCV_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "leave from trcv_pdq ercd=%d. data=%d. datapri=%d.";
		break;
	  case TFN_INI_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_pdq ercd=%d.";
		break;
	  case TFN_REF_PDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_pdq ercd=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  メールボックス機能 - 入口ログ
 */
static intptr_t
trace_print_mbxenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to snd_mbx mbxid=%d. pk_msg=%s.";
		break;
	  case TFN_RCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to rcv_mbx mbxid=%d. ppk_msg=%s.";
		break;
	  case TFN_PRCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to prcv_mbx mbxid=%d. ppk_msg=%s.";
		break;
	  case TFN_TRCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to trcv_mbx mbxid=%d. ppk_msg=%s. tmout=%d.";
		break;
	  case TFN_INI_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_mbx mbxid=%d.";
		break;
	  case TFN_REF_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_mbx mbxid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  メールボックス機能 - 出口ログ
 */
static intptr_t
trace_print_mbxleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_SND_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from snd_mbx ercd=%d.";
		break;
	  case TFN_RCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from rcv_mbx ercd=%d. pk_msg=%s.";
		break;
	  case TFN_PRCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from prcv_mbx ercd=%d. pk_msg=%s.";
		break;
	  case TFN_TRCV_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from trcv_mbx ercd=%d. pk_msg=%s.";
		break;
	  case TFN_INI_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_mbx ercd=%d.";
		break;
	  case TFN_REF_MBX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_mbx ercd=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  スピンロック機能 - 入口ログ
 */
static intptr_t
trace_print_spnenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_LOC_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to loc_spn spnid=%d.";
		break;
	  case TFN_ILOC_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iloc_spn spnid=%d.";
		break;
	  case TFN_TRY_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to try_spn spnid=%d.";
		break;
	  case TFN_ITRY_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to itry_spn spnid=%d.";
		break;
	  case TFN_UNL_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to unl_spn spnid=%d.";
		break;
	  case TFN_IUNL_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iunl_spn spnid=%d.";
		break;
	  case TFN_REF_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_spn spnid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  スピンロック機能 - 出口ログ
 */
static intptr_t
trace_print_spnleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_LOC_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from loc_spn ercd=%d.";
		break;
	  case TFN_ILOC_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iloc_spn ercd=%d.";
		break;
	  case TFN_TRY_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from try_spn ercd=%d.";
		break;
	  case TFN_ITRY_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from itry_spn ercd=%d.";
		break;
	  case TFN_UNL_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from unl_spn ercd=%d.";
		break;
	  case TFN_IUNL_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iunl_spn ercd=%d.";
		break;
	  case TFN_REF_SPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_spn ercd=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  固定長メモリプール機能 - 入口ログ
 */
static intptr_t
trace_print_mpfenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_GET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to get_mpf mpfid=%d. p_blk=%d.";
		break;
	  case TFN_PGET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to pget_mpf mpfid=%d. p_blk=%d.";
		break;
	  case TFN_TGET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to tget_mpf mpfid=%d. p_blk=%d. tmout=%d.";
		break;
	  case TFN_REL_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to rel_mpf mpfid=%d. blk=%d.";
		break;
	  case TFN_INI_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ini_mpf mpfid=%d.";
		break;
	  case TFN_REF_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_mpf mpfid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  固定長メモリプール機能 - 出口ログ
 */
static intptr_t
trace_print_mpfleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_GET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_mpf ercd=%d. blk=%d.";
		break;
	  case TFN_PGET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from pget_mpf ercd=%d. blk=%d.";
		break;
	  case TFN_TGET_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from tget_mpf ercd=%d. blk=%d.";
		break;
	  case TFN_REL_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from rel_mpf ercd=%d";
		break;
	  case TFN_INI_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ini_mpf ercd=%d.";
		break;
	  case TFN_REF_MPF:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_mpf ercd=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  システム時刻管理機能 - 入口ログ
 */
static intptr_t
trace_print_timenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_GET_TIM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_tim p_systim=%d.";
		break;
	  case TFN_GET_UTM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_utm p_systum=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  システム時刻管理機能 - 出口ログ
 */
static intptr_t
trace_print_timleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_GET_TIM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_tim ercd=%d. systim=%d.";
		break;
	  case TFN_GET_UTM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_utm ercd=%d. systum=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  周期ハンドラ機能 - 入口ログ
 */
static intptr_t
trace_print_cycenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_STA_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to sta_cyc cycid=%d.";
		break;
	  case TFN_MSTA_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to msta_cyc cycid=%d. prcid=%d.";
		break;
	  case TFN_STP_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to stp_cyc cycid=%d.";
		break;
	  case TFN_REF_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_cyc cycid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  周期ハンドラ機能 - 出口ログ
 */
static intptr_t
trace_print_cycleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_STA_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sta_cyc ercd=%d.";
		break;
	  case TFN_MSTA_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from msta_cyc ercd=%d. prcid=%d.";
		break;
	  case TFN_STP_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from stp_cyc ercd=%d.";
		break;
	  case TFN_REF_CYC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_cyc ercd=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  アラームハンドラ機能 - 入口ログ
 */
static intptr_t
trace_print_almenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_STA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to sta_alm almid=%d. almtim=%d.";
		break;
	  case TFN_MSTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to msta_alm almid=%d. almtim=%d. prcid=%d.";
		break;
	  case TFN_ISTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to ista_alm almid=%d. almtim=%d.";
		break;
	  case TFN_IMSTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		info[2] = (intptr_t)trace->loginfo[3];
		tracemsg = "enter to imsta_alm almid=%d. almtim=%d. prcid=%d.";
		break;
	  case TFN_STP_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to stp_alm almid=%d.";
		break;
	  case TFN_ISTP_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to istp_alm almid=%d.";
		break;
	  case TFN_REF_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ref_alm almid=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  アラームハンドラ機能 - 出口ログ
 */
static intptr_t
trace_print_almleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_STA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sta_alm ercd=%d.";
		break;
	  case TFN_MSTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from msta_alm ercd=%d.";
		break;
	  case TFN_ISTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ista_alm ercd=%d.";
		break;
	  case TFN_IMSTA_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from imsta_alm ercd=%d.";
		break;
	  case TFN_STP_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from stp_alm ercd=%d.";
		break;
	  case TFN_ISTP_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from istp_alm ercd=%d.";
		break;
	  case TFN_REF_ALM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ref_alm ercd=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  割込み管理機能 - 入口ログ
 */
static intptr_t
trace_print_intenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_DIS_INT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to dis_int intno=%d.";
		break;
	  case TFN_ENA_INT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to ena_int intno=%d.";
		break;
	  case TFN_CHG_IPM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to chg_ipm intpri=%d.";
		break;
	  case TFN_GET_IPM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_ipm p_intpri=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  割込み管理機能 - 出口ログ
 */
static intptr_t
trace_print_intleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_DIS_INT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from dis_int ercd=%d.";
		break;
	  case TFN_ENA_INT:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ena_int ercd=%d.";
		break;
	  case TFN_CHG_IPM:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from chg_ipm ercd=%d.";
		break;
	  case TFN_GET_IPM:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_ipm ercd=%d. intpri=%d.";
		break;
	  default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  CPU例外処理機能 - 入口ログ
 */
static intptr_t
trace_print_dpnenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_XSNS_DPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to xsns_dpn p_excinf=%d.";
		break;
	  case TFN_XSNS_XPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to xsns_xpn p_excinf=%d.";
		break;
	default:
		tracemsg = "unknown service call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  CPU例外処理機能 - 出口ログ
 */
static intptr_t
trace_print_dpnleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_XSNS_DPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from xsns_dpn state=%d.";
		break;
	  case TFN_XSNS_XPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from xsns_xpn state=%d.";
		break;
	  default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 *  システム状態管理機能 - 入口ログ
 */
static intptr_t
trace_print_sysenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_ROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to rot_rdq tskpri=%d.";
		break;
	  case TFN_IROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to irot_rdq tskpri=%d.";
		break;
	  case TFN_MROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to rot_rdq tskpri=%d. prcid=%d.";
		break;
	  case TFN_IMROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "enter to irot_rdq tskpri=%d. prcid=%d.";
		break;
	  case TFN_GET_TID:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_tid p_tskid=%d.";
		break;
	  case TFN_IGET_TID:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iget_tid p_tskid=%d.";
		break;
	  case TFN_LOC_CPU:
		tracemsg = "enter to loc_cpu.";
		break;
	  case TFN_ILOC_CPU:
		tracemsg = "enter to iloc_cpu.";
		break;
	  case TFN_UNL_CPU:
		tracemsg = "enter to unl_cpu.";
		break;
	  case TFN_IUNL_CPU:
		tracemsg = "enter to iunl_cpu.";
		break;
	  case TFN_DIS_DSP:
		tracemsg = "enter to dis_dsp.";
		break;
	  case TFN_ENA_DSP:
		tracemsg = "enter to ena_dsp.";
		break;
	  case TFN_SNS_CTX:
		tracemsg = "enter to sns_ctx.";
		break;
	  case TFN_SNS_LOC:
		tracemsg = "enter to sns_loc.";
		break;
	  case TFN_SNS_DSP:
		tracemsg = "enter to sns_dsp.";
		break;
	  case TFN_SNS_DPN:
		tracemsg = "enter to sns_dpn.";
		break;
	  case TFN_SNS_KER:
		tracemsg = "enter to sns_ker.";
		break;
	  case TFN_GET_PID:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to get_pid p_prcid=%d.";
		break;
	  case TFN_IGET_PID:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "enter to iget_pid p_prcid=%d.";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/*
 * システム状態管理機能 - 出口ログ
 */
static intptr_t
trace_print_sysleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {
	  case TFN_ROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from rot_rdq ercd=%d.";
		break;
	  case TFN_IROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from irot_rdq ercd=%d.";
		break;
	  case TFN_MROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from mrot_rdq ercd=%d.";
		break;
	  case TFN_IMROT_RDQ:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from imrot_rdq ercd=%d.";
		break;
	  case TFN_GET_TID:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_tid ercd=%d. tskid=%d";
		break;
	  case TFN_IGET_TID:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from iget_tid ercd=%d. tskid=%d";
		break;
	  case TFN_LOC_CPU:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from loc_cpu ercd=%d.";
		break;
	  case TFN_ILOC_CPU:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iloc_cpu ercd=%d.";
		break;
	  case TFN_UNL_CPU:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from unl_cpu ercd=%d.";
		break;
	  case TFN_IUNL_CPU:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from iunl_cpu ercd=%d.";
		break;
	  case TFN_DIS_DSP:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from dis_dsp ercd=%d.";
		break;
	  case TFN_ENA_DSP:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from ena_dsp ercd=%d.";
		break;
	  case TFN_SNS_CTX:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_ctx state=%d.";
		break;
	  case TFN_SNS_LOC:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_loc state=%d.";
		break;
	  case TFN_SNS_DSP:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_dsp state=%d.";
		break;
	  case TFN_SNS_DPN:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_dpn state=%d.";
		break;
	  case TFN_SNS_KER:
		info[0] = (intptr_t)trace->loginfo[1];
		tracemsg = "leave from sns_ker state=%d.";
		break;
	  case TFN_GET_PID:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from get_pid ercd=%d. prcid=%d";
		break;
	  case TFN_IGET_PID:
		info[0] = (intptr_t)trace->loginfo[1];
		info[1] = (intptr_t)trace->loginfo[2];
		tracemsg = "leave from iget_pid ercd=%d. prcid=%d";
		break;
	default:
		tracemsg = "unknown servic call";
		break;
	}

	return((intptr_t) tracemsg);
}



/* 
 *  カーネルの初期化と終了処理 - 入口ログ
 */ 
static intptr_t 
trace_print_kerenter(TRACE *trace, intptr_t *info) 
{ 
	uint_t          type; 
	const char      *tracemsg; 
	
	type = (uint_t)trace->loginfo[0]; 
	
	switch (type) { 
	  case TFN_EXT_KER: 
		tracemsg = "enter to ext_ker."; 
		break; 
	  default: 
		tracemsg = "unknown service call";
		break;
	}
	return((intptr_t) tracemsg);
}

/*
 *  システムコールの入口ログ(LOG_TYPE_SVC|ENTER)
 */
static intptr_t
trace_print_svcenter(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {

		/*タスク管理・タスク付属同期機能 */
	  case TFN_MIG_TSK:
	  case TFN_ACT_TSK:
	  case TFN_IACT_TSK:
	  case TFN_MACT_TSK:
	  case TFN_IMACT_TSK:
	  case TFN_CAN_ACT:
	  case TFN_EXT_TSK:
	  case TFN_TER_TSK:
	  case TFN_CHG_PRI:
	  case TFN_GET_PRI:
	  case TFN_GET_INF:
	  case TFN_REF_TSK:
	  case TFN_SLP_TSK:
	  case TFN_TSLP_TSK:
	  case TFN_WUP_TSK:
	  case TFN_IWUP_TSK:
	  case TFN_CAN_WUP:
	  case TFN_REL_WAI:
	  case TFN_IREL_WAI:
	  case TFN_SUS_TSK:
	  case TFN_RSM_TSK:
	  case TFN_DLY_TSK:
		tracemsg = (const char*)trace_print_tskenter(trace, info);
		break;

		/* タスク例外処理機能 */
	  case TFN_RAS_TEX:
	  case TFN_IRAS_TEX:
	  case TFN_DIS_TEX:
	  case TFN_ENA_TEX:
	  case TFN_SNS_TEX:
	  case TFN_REF_TEX:
		tracemsg = (const char*)trace_print_texenter(trace, info);
		break;

		/* セマフォ機能 */
	  case TFN_SIG_SEM:
	  case TFN_ISIG_SEM:
	  case TFN_WAI_SEM:
	  case TFN_POL_SEM:
	  case TFN_TWAI_SEM:
	  case TFN_INI_SEM:
	  case TFN_REF_SEM:
		tracemsg = (const char*)trace_print_sementer(trace, info);
		break;

		/* イベントフラグ機能 */
	  case TFN_SET_FLG:
	  case TFN_ISET_FLG:
	  case TFN_CLR_FLG:
	  case TFN_WAI_FLG:
	  case TFN_POL_FLG:
	  case TFN_TWAI_FLG:
	  case TFN_INI_FLG:
	  case TFN_REF_FLG:
		tracemsg = (const char*)trace_print_flgenter(trace, info);
		break;

		/* データキュー機能 */
	  case TFN_SND_DTQ:
	  case TFN_PSND_DTQ:
	  case TFN_IPSND_DTQ:
	  case TFN_TSND_DTQ:
	  case TFN_FSND_DTQ:
	  case TFN_IFSND_DTQ:
	  case TFN_RCV_DTQ:
	  case TFN_PRCV_DTQ:
	  case TFN_TRCV_DTQ:
	  case TFN_INI_DTQ:
	  case TFN_REF_DTQ:
		tracemsg = (const char*)trace_print_dtqenter(trace, info);
		break;

		/* 優先度データキュー機能 */
	  case TFN_SND_PDQ:
	  case TFN_PSND_PDQ:
	  case TFN_IPSND_PDQ:
	  case TFN_TSND_PDQ:
	  case TFN_RCV_PDQ:
	  case TFN_PRCV_PDQ:
	  case TFN_TRCV_PDQ:
	  case TFN_INI_PDQ:
	  case TFN_REF_PDQ:
		tracemsg = (const char*)trace_print_pdqenter(trace, info);
		break;

		/* 固定長メモリプール機能 */
	  case TFN_GET_MPF:
	  case TFN_PGET_MPF:
	  case TFN_TGET_MPF:
	  case TFN_REL_MPF:
	  case TFN_INI_MPF:
	  case TFN_REF_MPF:
		tracemsg = (const char*)trace_print_mpfenter(trace, info);
		break;

		/* システム時刻管理機能  */
	  case TFN_GET_TIM:
	  case TFN_GET_UTM:
		tracemsg = (const char*)trace_print_timenter(trace, info);
		break;

		/* 周期ハンドラ機能 */
	  case TFN_STA_CYC:
	  case TFN_STP_CYC:
	  case TFN_REF_CYC:
	  case TFN_MSTA_CYC:
		tracemsg = (const char*)trace_print_cycenter(trace, info);
		break;

		/* アラームハンドラ機能 */
	  case TFN_STA_ALM:
	  case TFN_ISTA_ALM:
	  case TFN_STP_ALM:
	  case TFN_ISTP_ALM:
	  case TFN_REF_ALM:
	  case TFN_MSTA_ALM:
	  case TFN_IMSTA_ALM:
		tracemsg = (const char*)trace_print_almenter(trace, info);
		break;

		/* メールボックス機能 */
	  case TFN_SND_MBX:
	  case TFN_RCV_MBX:
	  case TFN_PRCV_MBX:
	  case TFN_TRCV_MBX:
	  case TFN_INI_MBX:
	  case TFN_REF_MBX:
		tracemsg = (const char*)trace_print_mbxenter(trace, info);
		break;

		/* スピンロック機能 */
	  case TFN_LOC_SPN:
	  case TFN_ILOC_SPN:
	  case TFN_TRY_SPN:
	  case TFN_ITRY_SPN:
	  case TFN_UNL_SPN:
	  case TFN_IUNL_SPN:
		tracemsg = (const char*)trace_print_spnenter(trace, info);
		break;

		/* 割込み管理機能 */
	  case TFN_DIS_INT:
	  case TFN_ENA_INT:
	  case TFN_CHG_IPM:
	  case TFN_GET_IPM:
		tracemsg = (const char*)trace_print_intenter(trace, info);
		break;
          
		/* CPU例外処理 */
	  case TFN_XSNS_DPN:
	  case TFN_XSNS_XPN:
		tracemsg = (const char*)trace_print_dpnenter(trace, info);
		break;

		/* システム状態管理機能 */
	  case TFN_ROT_RDQ:
	  case TFN_IROT_RDQ:
	  case TFN_MROT_RDQ:
	  case TFN_IMROT_RDQ:
	  case TFN_GET_TID:
	  case TFN_IGET_TID:
	  case TFN_LOC_CPU:
	  case TFN_ILOC_CPU:
	  case TFN_UNL_CPU:
	  case TFN_IUNL_CPU:
	  case TFN_DIS_DSP:
	  case TFN_ENA_DSP:
	  case TFN_SNS_CTX:
	  case TFN_SNS_LOC:
	  case TFN_SNS_DSP:
	  case TFN_SNS_DPN:
	  case TFN_SNS_KER:
	  case TFN_GET_PID:
	  case TFN_IGET_PID:
		tracemsg = (const char*)trace_print_sysenter(trace, info);
		break;

		/* カーネルの初期化と終了処理 */ 
	  case TFN_EXT_KER: 
		tracemsg = (const char*)trace_print_kerenter(trace, info); 
		break; 
          
	default:
		tracemsg = "unknown LOG_TYPE_SVC|ENTER service call";
		break;
	}
	return((intptr_t) tracemsg);
}


/*
 *  システムコールの出口ログ(LOG_TYPE_SVC|LEAVE)
 */
static intptr_t
trace_print_svcleave(TRACE *trace, intptr_t *info)
{
	uint_t		type;
	const char	*tracemsg;

	type = (uint_t)trace->loginfo[0];

	switch (type) {

		/* タスク管理・タスク付属同期機能 */
	  case TFN_MIG_TSK:
	  case TFN_ACT_TSK:
	  case TFN_IACT_TSK:
	  case TFN_MACT_TSK:
	  case TFN_IMACT_TSK:
	  case TFN_CAN_ACT:
	  case TFN_EXT_TSK:
	  case TFN_TER_TSK:
	  case TFN_CHG_PRI:
	  case TFN_GET_PRI:
	  case TFN_GET_INF:
	  case TFN_REF_TSK:
	  case TFN_SLP_TSK:
	  case TFN_TSLP_TSK:
	  case TFN_WUP_TSK:
	  case TFN_IWUP_TSK:
	  case TFN_CAN_WUP:
	  case TFN_REL_WAI:
	  case TFN_IREL_WAI:
	  case TFN_SUS_TSK:
	  case TFN_RSM_TSK:
	  case TFN_DLY_TSK:
		tracemsg = (const char*)trace_print_tskleave(trace, info);
		break;

		/* タスク例外処理機能 */
	  case TFN_RAS_TEX:
	  case TFN_IRAS_TEX:
	  case TFN_DIS_TEX:
	  case TFN_ENA_TEX:
	  case TFN_SNS_TEX:
	  case TFN_REF_TEX:
		tracemsg = (const char*)trace_print_texleave(trace, info);
		break;
          
		/* セマフォ機能 */
	  case TFN_SIG_SEM:
	  case TFN_ISIG_SEM:
	  case TFN_WAI_SEM:
	  case TFN_POL_SEM:
	  case TFN_TWAI_SEM:
	  case TFN_INI_SEM:
	  case TFN_REF_SEM:
		tracemsg = (const char*)trace_print_semleave(trace, info);
		break;

		/* イベントフラグ機能 */
	  case TFN_SET_FLG:
	  case TFN_ISET_FLG:
	  case TFN_CLR_FLG:
	  case TFN_WAI_FLG:
	  case TFN_POL_FLG:
	  case TFN_TWAI_FLG:
	  case TFN_INI_FLG:
	  case TFN_REF_FLG:
		tracemsg = (const char*)trace_print_flgleave(trace, info);
		break;

		/* データキュー機能 */
	  case TFN_SND_DTQ:
	  case TFN_PSND_DTQ:
	  case TFN_IPSND_DTQ:
	  case TFN_TSND_DTQ:
	  case TFN_FSND_DTQ:
	  case TFN_IFSND_DTQ:
	  case TFN_RCV_DTQ:
	  case TFN_PRCV_DTQ:
	  case TFN_TRCV_DTQ:
	  case TFN_INI_DTQ:
	  case TFN_REF_DTQ:
		tracemsg = (const char*)trace_print_dtqleave(trace, info);
		break;

		/* 優先度データキュー機能 */
	  case TFN_SND_PDQ:
	  case TFN_PSND_PDQ:
	  case TFN_IPSND_PDQ:
	  case TFN_TSND_PDQ:
	  case TFN_RCV_PDQ:
	  case TFN_PRCV_PDQ:
	  case TFN_TRCV_PDQ:
	  case TFN_INI_PDQ:
	  case TFN_REF_PDQ:
		tracemsg = (const char*)trace_print_pdqleave(trace, info);
		break;

		/* 固定長メモリプール機能 */
	  case TFN_GET_MPF:
	  case TFN_PGET_MPF:
	  case TFN_TGET_MPF:
	  case TFN_REL_MPF:
	  case TFN_INI_MPF:
	  case TFN_REF_MPF:
		tracemsg = (const char*)trace_print_mpfleave(trace, info);
		break;

		/* システム時刻管理機能  */
	  case TFN_GET_TIM:
	  case TFN_GET_UTM:
		tracemsg = (const char*)trace_print_timleave(trace, info);
		break;

		/* 周期ハンドラ機能 */
	  case TFN_STA_CYC:
	  case TFN_STP_CYC:
	  case TFN_REF_CYC:
	  case TFN_MSTA_CYC:
		tracemsg = (const char*)trace_print_cycleave(trace, info);
		break;

		/* アラームハンドラ機能 */
	  case TFN_STA_ALM:
	  case TFN_ISTA_ALM:
	  case TFN_STP_ALM:
	  case TFN_ISTP_ALM:
	  case TFN_REF_ALM:
	  case TFN_MSTA_ALM:
	  case TFN_IMSTA_ALM:
		tracemsg = (const char*)trace_print_almleave(trace, info);
		break;

		/* メールボックス機能 */
	  case TFN_SND_MBX:
	  case TFN_RCV_MBX:
	  case TFN_PRCV_MBX:
	  case TFN_TRCV_MBX:
	  case TFN_INI_MBX:
	  case TFN_REF_MBX:
		tracemsg = (const char*)trace_print_mbxleave(trace, info);
		break;

		/* スピンロック機能 */
	  case TFN_LOC_SPN:
	  case TFN_ILOC_SPN:
	  case TFN_TRY_SPN:
	  case TFN_ITRY_SPN:
	  case TFN_UNL_SPN:
	  case TFN_IUNL_SPN:
		tracemsg = (const char*)trace_print_spnleave(trace, info);
		break;

		/* 割込み管理機能 */
	  case TFN_DIS_INT:
	  case TFN_ENA_INT:
	  case TFN_CHG_IPM:
	  case TFN_GET_IPM:
		tracemsg = (const char*)trace_print_intleave(trace, info);
		break;

		/* CPU例外処理機能 */
	  case TFN_XSNS_DPN:
	  case TFN_XSNS_XPN:
		tracemsg = (const char*)trace_print_dpnleave(trace, info);
		break;

		/* システム状態管理機能 */
	  case TFN_ROT_RDQ:
	  case TFN_IROT_RDQ:
	  case TFN_MROT_RDQ:
	  case TFN_IMROT_RDQ:
	  case TFN_GET_TID:
	  case TFN_IGET_TID:
	  case TFN_LOC_CPU:
	  case TFN_ILOC_CPU:
	  case TFN_UNL_CPU:
	  case TFN_IUNL_CPU:
	  case TFN_DIS_DSP:
	  case TFN_ENA_DSP:
	  case TFN_SNS_CTX:
	  case TFN_SNS_LOC:
	  case TFN_SNS_DSP:
	  case TFN_SNS_DPN:
	  case TFN_SNS_KER:
	  case TFN_GET_PID:
	  case TFN_IGET_PID:
		tracemsg = (const char*)trace_print_sysleave(trace, info);
		break;

	  default:
		tracemsg = "unknown LOG_TYPE_SVC|LEAVE servic call";
		break;
	}

	return((intptr_t) tracemsg);
}


/* 
 *  トレースログの表示
 */
static void
trace_print(TRACE *p_trace, void (*putc)(char))
{
	intptr_t	traceinfo[TMAX_LOGINFO + 1];
	const char	*tracemsg;
	int_t		i;

	traceinfo[0] = (intptr_t)(p_trace->logtim);
	traceinfo[1] = (intptr_t)(p_trace->prcid);
	syslog_printf("[%d]:[%d]: ", traceinfo, putc);

	switch (p_trace->logtype) {
	case LOG_TYPE_TSKSTAT:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = get_tskstat(p_trace->loginfo[1]);
		tracemsg = "task %d becomes %s.";
		break;
	case LOG_TYPE_TSKMIG:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = p_trace->loginfo[1];
		traceinfo[2] = p_trace->loginfo[2];
		tracemsg = "task %d migrates from processor %d to processor %d.";
		break;
	case LOG_TYPE_CYCMIG:
		traceinfo[0] = (intptr_t)((ID)(((((CYCCB*)(p_trace->loginfo[0]))->p_cycinib) - cycinib_table) + TMIN_CYCID));
		traceinfo[1] = p_trace->loginfo[1];
		traceinfo[2] = p_trace->loginfo[2];
		tracemsg = "cyclic handler %d migrates from processor %d to processor %d.";
		break;
	case LOG_TYPE_ALMMIG:
		traceinfo[0] = (intptr_t)((ID)(((((ALMCB*)(p_trace->loginfo[0]))->p_alminib) - alminib_table) + TMIN_ALMID));
		traceinfo[1] = p_trace->loginfo[1];
		traceinfo[2] = p_trace->loginfo[2];
		tracemsg = "alarm handler %d migrates from processor %d to processor %d.";
		break;
	case LOG_TYPE_DSP|LOG_ENTER:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		tracemsg = "dispatch from task %d.";
		break;
	case LOG_TYPE_DSP|LOG_LEAVE:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		tracemsg = "dispatch to task %d.";
		break;
	case LOG_TYPE_INH|LOG_ENTER:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "enter to int handler %d.";
		break;
	case LOG_TYPE_INH|LOG_LEAVE:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "leave from int handler %d.";
		break;
	case LOG_TYPE_ISR|LOG_ENTER:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "enter to isr %d.";
		break;
	case LOG_TYPE_ISR|LOG_LEAVE:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "leave from isr %d.";
		break;
	case LOG_TYPE_CYC|LOG_ENTER:
		traceinfo[0] = (intptr_t)((ID)(((((CYCCB*)(p_trace->loginfo[0]))->p_cycinib) - cycinib_table) + TMIN_CYCID));
		tracemsg = "enter to cyclic handler %d.";
		break;
	case LOG_TYPE_CYC|LOG_LEAVE:
		traceinfo[0] = (intptr_t)((ID)(((((CYCCB*)(p_trace->loginfo[0]))->p_cycinib) - cycinib_table) + TMIN_CYCID));
		tracemsg = "leave from cyclic handler %d.";
		break;
	case LOG_TYPE_ALM|LOG_ENTER:
		traceinfo[0] = (intptr_t)((ID)(((((ALMCB*)(p_trace->loginfo[0]))->p_alminib) - alminib_table) + TMIN_ALMID));
		tracemsg = "enter to alarm handler %d.";
		break;
	case LOG_TYPE_ALM|LOG_LEAVE:
		traceinfo[0] = (intptr_t)((ID)(((((ALMCB*)(p_trace->loginfo[0]))->p_alminib) - alminib_table) + TMIN_ALMID));
		tracemsg = "leave from alarm handler %d.";
		break;
	case LOG_TYPE_EXC|LOG_ENTER:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "enter to exc handler %d.";
		break;
	case LOG_TYPE_EXC|LOG_LEAVE:
		traceinfo[0] = p_trace->loginfo[0];
		tracemsg = "leave from exc handler %d.";
		break;
	case LOG_TYPE_TEX|LOG_ENTER:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = p_trace->loginfo[1];
		tracemsg = "task %d enter to tex %d.";
		break;
	case LOG_TYPE_TEX|LOG_LEAVE:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = p_trace->loginfo[1];
		tracemsg = "task %d leave from tex %d.";
		break;
	case LOG_TYPE_COMMENT:
		for (i = 1; i < TMAX_LOGINFO; i++) {
			traceinfo[i-1] = p_trace->loginfo[i];
		}
		tracemsg = (const char *)(p_trace->loginfo[0]);
		break;
	case LOG_TYPE_ASSERT:
		traceinfo[0] = p_trace->loginfo[0];
		traceinfo[1] = p_trace->loginfo[1];
		traceinfo[2] = p_trace->loginfo[2];
		tracemsg = "%s:%u: Assertion `%s' failed.";
		break;
	case LOG_TYPE_PHASE:
		traceinfo[0] = get_tskid(p_trace->loginfo[0]);
		traceinfo[1] = p_trace->loginfo[1];
		tracemsg = "task %d enter phase %d.";
		break;
	case LOG_TYPE_SVC|LOG_ENTER:
		tracemsg = (const char *)trace_print_svcenter(p_trace, traceinfo);
		break;
	case LOG_TYPE_SVC|LOG_LEAVE:
		tracemsg = (const char *)trace_print_svcleave(p_trace, traceinfo);
		break;
	default:
		traceinfo[0] = p_trace->logtype;
		tracemsg = "unknown trace log type: %d.";
		break;
	}
	syslog_printf(tracemsg, traceinfo, putc);
	(*putc)('\n');
}

/* 
 *  トレースログのダンプ
 */
void
trace_dump(intptr_t exinf)
{
	TRACE	trace;
	void	(*putc)(char);

	putc = (void (*)(char)) exinf;
	while (trace_rea_log(&trace) >= 0) {
		trace_print(&trace, putc);
	}
}
