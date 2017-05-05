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
 *  @(#) $Id: syslog.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		システムログ機能
 */

#include <kernel.h>
#include <sil.h>
#undef TOPPERS_OMIT_SYSLOG
#include <t_syslog.h>
#include <log_output.h>
#include "target_syssvc.h"
#include "syslog.h"
#include "kernel_cfg.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_SYSLOG_WRI_LOG_ENTER
#define LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog)
#endif /* LOG_SYSLOG_WRI_LOG_ENTER */

#ifndef LOG_SYSLOG_WRI_LOG_LEAVE
#define LOG_SYSLOG_WRI_LOG_LEAVE(ercd)
#endif /* LOG_SYSLOG_WRI_LOG_LEAVE */

#ifndef LOG_SYSLOG_REA_LOG_ENTER
#define LOG_SYSLOG_REA_LOG_ENTER(p_syslog)
#endif /* LOG_SYSLOG_REA_LOG_ENTER */

#ifndef LOG_SYSLOG_REA_LOG_LEAVE
#define LOG_SYSLOG_REA_LOG_LEAVE(ercd, p_syslog)
#endif /* LOG_SYSLOG_REA_LOG_LEAVE */

#ifndef LOG_SYSLOG_MSK_LOG_ENTER
#define LOG_SYSLOG_MSK_LOG_ENTER(logmask, lowmask)
#endif /* LOG_SYSLOG_MSK_LOG_ENTER */

#ifndef LOG_SYSLOG_MSK_LOG_LEAVE
#define LOG_SYSLOG_MSK_LOG_LEAVE(ercd)
#endif /* LOG_SYSLOG_MSK_LOG_LEAVE */

#ifndef LOG_SYSLOG_REF_LOG_ENTER
#define LOG_SYSLOG_REF_LOG_ENTER(pk_rlog)
#endif /* LOG_SYSLOG_REF_LOG_ENTER */

#ifndef LOG_SYSLOG_REF_LOG_LEAVE
#define LOG_SYSLOG_REF_LOG_LEAVE(pk_rlog)
#endif /* LOG_SYSLOG_REF_LOG_LEAVE */

/*
 *  システムログ機能の変数配置のディフォルト定義
 */
#ifndef SEC_SYSLOG
#define SEC_SYSLOG(type, variable) type variable
#endif /* SEC_SYSLOG */

#ifndef SEC_SYSLOG_BEGIN
#define SEC_SYSLOG_BEGIN
#endif /* SEC_SYSLOG_BEGIN */

#ifndef SEC_SYSLOG_END
#define SEC_SYSLOG_END
#endif /* SEC_SYSLOG_END */

#ifndef SEC_PRC1_SYSLOG
#define SEC_PRC1_SYSLOG(type, variable) type variable
#endif /* SEC_PRC1_SYSLOG */

#ifndef SEC_PRC1_SYSLOG_BEGIN
#define SEC_PRC1_SYSLOG_BEGIN
#endif /* SEC_PRC1_SYSLOG_BEGIN */

#ifndef SEC_PRC1_SYSLOG_END
#define SEC_PRC1_SYSLOG_END
#endif /* SEC_PRC1_SYSLOG_END */

#ifndef SEC_PRC2_SYSLOG
#define SEC_PRC2_SYSLOG(type, variable) type variable
#endif /* SEC_PRC2_SYSLOG */

#ifndef SEC_PRC2_SYSLOG_BEGIN
#define SEC_PRC2_SYSLOG_BEGIN
#endif /* SEC_PRC2_SYSLOG_BEGIN */

#ifndef SEC_PRC2_SYSLOG_END
#define SEC_PRC2_SYSLOG_END
#endif /* SEC_PRC2_SYSLOG_END */

#ifndef SEC_PRC3_SYSLOG
#define SEC_PRC3_SYSLOG(type, variable) type variable
#endif /* SEC_PRC3_SYSLOG */

#ifndef SEC_PRC3_SYSLOG_BEGIN
#define SEC_PRC3_SYSLOG_BEGIN
#endif /* SEC_PRC3_SYSLOG_BEGIN */

#ifndef SEC_PRC3_SYSLOG_END
#define SEC_PRC3_SYSLOG_END
#endif /* SEC_PRC3_SYSLOG_END */

#ifndef SEC_PRC4_SYSLOG
#define SEC_PRC4_SYSLOG(type, variable) type variable
#endif /* SEC_PRC4_SYSLOG */

#ifndef SEC_PRC4_SYSLOG_BEGIN
#define SEC_PRC4_SYSLOG_BEGIN
#endif /* SEC_PRC4_SYSLOG_BEGIN */

#ifndef SEC_PRC4_SYSLOG_END
#define SEC_PRC4_SYSLOG_END
#endif /* SEC_PRC4_SYSLOG_END */

/*
 *  ポート数のチェック
 */
#if !defined(G_SYSLOG) && (TNUM_PORT < TNUM_PRCID)
#error !defined(G_SYSLOG) && (TNUM_PORT < TNUM_PRCID) : check G_SYSLOG and TNUM_PORT !
#endif /* defined(G_SYSLOG) && (TNUM_PORT == 1) */

/*
 *  システムログコントロールブロック
 */
typedef struct syslog_control_block{
	SYSLOG	syslog_buffer[TCNT_SYSLOG_BUFFER];	/* ログバッファ */
	uint_t	syslog_count;		/* ログバッファ中のログの数 */
	uint_t	syslog_head;		/* 先頭のログの格納位置 */
	uint_t	syslog_tail;		/* 次のログの格納位置 */
	uint_t	syslog_lost;		/* 失われたログの数 */
	uint_t	syslog_logmask;		/* ログバッファに記録すべき重要度 */
	uint_t	syslog_lowmask_not;	/* 低レベル出力すべき重要度（反転）*/
}SYSLOGCB;

#ifdef G_SYSLOG

SEC_SYSLOG_BEGIN
SEC_SYSLOG(SYSLOGCB, syslogcb);
SEC_SYSLOG_END

#else /* G_SYSLOG */

#if TNUM_PRCID >= 1
SEC_PRC1_SYSLOG_BEGIN
SEC_PRC1_SYSLOG(SYSLOGCB, prc1_syslogcb);
SEC_PRC1_SYSLOG_END
#endif /* TNUM_PRCID >= 1 */

#if TNUM_PRCID >= 2
SEC_PRC2_SYSLOG_BEGIN
SEC_PRC2_SYSLOG(SYSLOGCB, prc2_syslogcb);
SEC_PRC2_SYSLOG_END
#endif /* TNUM_PRCID >= 2 */

#if TNUM_PRCID >= 3
SEC_PRC3_SYSLOG_BEGIN
SEC_PRC3_SYSLOG(SYSLOGCB, prc3_syslogcb);
SEC_PRC3_SYSLOG_END
#endif /* TNUM_PRCID >= 3 */

#if TNUM_PRCID >= 4
SEC_PRC4_SYSLOG_BEGIN
SEC_PRC4_SYSLOG(SYSLOGCB, prc4_syslogcb);
SEC_PRC4_SYSLOG_END
#endif /* TNUM_PRCID >= 4 */

#if TNUM_PRCID >= 5
#error Syslog Not Support more than 5 processor!
#endif /* TNUM_PRCID >= 5 */

static SYSLOGCB* const p_syslogcb_table[TNUM_PRCID] = {
#if TNUM_PRCID >= 1
	&prc1_syslogcb,
#endif /* TNUM_PRCID >= 1 */

#if TNUM_PRCID >= 2
	&prc2_syslogcb,
#endif /* TNUM_PRCID >= 2 */

#if TNUM_PRCID >= 3
	&prc3_syslogcb,
#endif /* TNUM_PRCID >= 3 */

#if TNUM_PRCID >= 4
	&prc4_syslogcb
#endif /* TNUM_PRCID >= 4 */
};

#endif /* G_SYSLOG */


#ifdef G_SYSLOG

/*
 *  システムログコントロールブロックへのアクセス
 */

Inline SYSLOGCB*
get_p_syslogcb(void){
	return (&syslogcb);
}

#else /* G_SYSLOG */

Inline SYSLOGCB*
get_p_syslogcb(void){
	ID prcid;

	sil_get_pid(&prcid);

	return p_syslogcb_table[prcid - 1];
}

#endif /* G_SYSLOG */

/*
 *  システムログ機能の初期化
 */
void
syslog_initialize(intptr_t exinf)
{
	SYSLOGCB *p_syslogcb = get_p_syslogcb();

	p_syslogcb->syslog_count = 0U;
	p_syslogcb->syslog_head = 0U;
	p_syslogcb->syslog_tail = 0U;
	p_syslogcb->syslog_lost = 0U;
    
	p_syslogcb->syslog_logmask = 0U;
	p_syslogcb->syslog_lowmask_not = 0U;
}     

/*
 *  現在のシステム時刻
 *
 *  ログ時刻を取り出すために，やむなく，カーネルの内部変数を参照してい
 *  る．
 */
extern ulong_t _kernel_get_my_current_time(void);

/* 
 *  ログ情報の出力
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 *  ただし，グローバル方式の場合は，SILのスピンロック取得中に呼び出した
 *  場合の動作は保証しない．
 */
ER
syslog_wri_log(uint_t prio, const SYSLOG *p_syslog)
{
	SYSLOGCB *p_syslogcb;

	SIL_PRE_LOC;

	LOG_SYSLOG_WRI_LOG_ENTER(prio, p_syslog);

#ifdef G_SYSLOG
	SIL_LOC_SPN();
#else
	SIL_LOC_INT();
#endif /* G_SYSLOG */

	/*
	 * マイグレーション対応のため，ロック後に取得
	 */
	p_syslogcb = get_p_syslogcb();

	/*
	 *  ログ時刻の設定
	 */
	((SYSLOG *) p_syslog)->logtim = _kernel_get_my_current_time();

	/*
	 *  プロセッサIDの設定
	 */
	sil_get_pid(&(((SYSLOG *) p_syslog)->prcid));

	/*
	 *  ログバッファに記録
	 */
	if ((p_syslogcb->syslog_logmask & LOG_MASK(prio)) != 0U) {
		p_syslogcb->syslog_buffer[p_syslogcb->syslog_tail] = *p_syslog;
		(p_syslogcb->syslog_tail)++;
		if (p_syslogcb->syslog_tail >= TCNT_SYSLOG_BUFFER) {
			p_syslogcb->syslog_tail = 0U;
		}
		if (p_syslogcb->syslog_count < TCNT_SYSLOG_BUFFER) {
			p_syslogcb->syslog_count++;
		}
		else {
			p_syslogcb->syslog_head = p_syslogcb->syslog_tail;
			(p_syslogcb->syslog_lost)++;
		}
	}

	/*
	 *  低レベル出力
	 */
	if (((~(p_syslogcb->syslog_lowmask_not)) & LOG_MASK(prio)) != 0U) {
		syslog_print(p_syslog, target_fput_log);
		target_fput_log('\n');
	}

#ifdef G_SYSLOG
	SIL_UNL_SPN();
#else
	SIL_UNL_INT();
#endif /* G_SYSLOG */

	LOG_SYSLOG_WRI_LOG_LEAVE(E_OK);
	return(E_OK);
}

/*
 *  ログバッファからの読出し
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 *  ただし，グローバル方式の場合は，SILのスピンロック取得中に呼び出した
 *  場合の動作は保証しない．
 */
ER_UINT
syslog_rea_log(SYSLOG *p_syslog)
{
	ER_UINT	ercd;
	SYSLOGCB *p_syslogcb;

	SIL_PRE_LOC;

	LOG_SYSLOG_REA_LOG_ENTER(p_syslog);

#ifdef G_SYSLOG
	SIL_LOC_SPN();
#else
	SIL_LOC_INT();
#endif /* G_SYSLOG */

	/*
	 * マイグレーション対応のため，ロック後に取得
	 */
	p_syslogcb = get_p_syslogcb();

	/*
	 *  ログバッファからの取出し
	 */
	if (p_syslogcb->syslog_count > 0U) {
		*p_syslog = p_syslogcb->syslog_buffer[p_syslogcb->syslog_head];
		(p_syslogcb->syslog_count)--;
		(p_syslogcb->syslog_head)++;
		if (p_syslogcb->syslog_head >= TCNT_SYSLOG_BUFFER) {
			p_syslogcb->syslog_head = 0U;
		}
		ercd = (ER_UINT) (p_syslogcb->syslog_lost);
		p_syslogcb->syslog_lost = 0U;
	}
	else {
		ercd = E_OBJ;
	}

#ifdef G_SYSLOG
	SIL_UNL_SPN();
#else
	SIL_UNL_INT();
#endif /* G_SYSLOG */

	LOG_SYSLOG_REA_LOG_LEAVE(ercd, p_syslog);
	return(ercd);
}

/* 
 *  出力すべきログ情報の重要度の設定
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 *  ただし，グローバル方式の場合は，SILのスピンロック取得中に呼び出した
 *  場合の動作は保証しない．
 */
ER
syslog_msk_log(uint_t logmask, uint_t lowmask)
{
	SYSLOGCB *p_syslogcb;

	SIL_PRE_LOC;

	LOG_SYSLOG_MSK_LOG_ENTER(logmask, lowmask);

#ifdef G_SYSLOG
	SIL_LOC_SPN();
#else
	SIL_LOC_INT();
#endif /* G_SYSLOG */

	/*
	 * マイグレーション対応のため，ロック後に取得
	 */
	p_syslogcb = get_p_syslogcb();

	p_syslogcb->syslog_logmask = logmask;
	p_syslogcb->syslog_lowmask_not = ~lowmask;

#ifdef G_SYSLOG
	SIL_UNL_SPN();
#else
	SIL_UNL_INT();
#endif /* G_SYSLOG */

	LOG_SYSLOG_MSK_LOG_LEAVE(E_OK);

	return(E_OK);
}

/*
 *  ログバッファの状態参照
 *
 *  CPUロック状態や実行コンテキストによらず動作できるように実装してある．
 *  ただし，グローバル方式の場合は，SILのスピンロック取得中に呼び出した
 *  場合の動作は保証しない．
 */
ER
syslog_ref_log(T_SYSLOG_RLOG *pk_rlog)
{
	SYSLOGCB *p_syslogcb;

	SIL_PRE_LOC;

	LOG_SYSLOG_REF_LOG_ENTER(pk_rlog);

#ifdef G_SYSLOG
	SIL_LOC_SPN();
#else
	SIL_LOC_INT();
#endif /* G_SYSLOG */

	/*
	 * マイグレーション対応のため，ロック後に取得
	 */
	p_syslogcb = get_p_syslogcb();

	pk_rlog->count = p_syslogcb->syslog_count;
	pk_rlog->lost = p_syslogcb->syslog_lost;
	pk_rlog->logmask = p_syslogcb->syslog_logmask;
	pk_rlog->lowmask = ~(p_syslogcb->syslog_lowmask_not);

#ifdef G_SYSLOG
	SIL_UNL_SPN();
#else
	SIL_UNL_INT();
#endif /* G_SYSLOG */

	LOG_SYSLOG_REF_LOG_LEAVE(pk_rlog);
	return(E_OK);
}
