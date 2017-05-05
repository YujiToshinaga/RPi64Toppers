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
 *  @(#) $Id: logtask.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		システムログタスク
 */

#include <kernel.h>
#include <t_syslog.h>
#include <log_output.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "logtask.h"

/*
 *  システムログタスクの変数配置のディフォルト定義
 */
#ifndef SEC_LOGTASK
#define SEC_LOGTASK(type, variable) type variable
#endif /* SEC_PREC1_SYSLOG */

#ifndef SEC_LOGTASK_BEGIN
#define SEC_LOGTASK_BEGIN
#endif /* SEC_LOGTASK_BEGIN */

#ifndef SEC_LOGTASK_END
#define SEC_LOGTASK_END
#endif /* SEC_LOGTASK_END */

#ifndef SEC_PRC1_LOGTASK
#define SEC_PRC1_LOGTASK(type, variable) type variable
#endif /* SEC_PREC1_SYSLOG */

#ifndef SEC_PRC1_LOGTASK_BEGIN
#define SEC_PRC1_LOGTASK_BEGIN
#endif /* SEC_PRC1_LOGTASK_BEGIN */

#ifndef SEC_PRC1_LOGTASK_END
#define SEC_PRC1_LOGTASK_END
#endif /* SEC_PREC1_LOGTASK_END */

#ifndef SEC_PRC2_LOGTASK
#define SEC_PRC2_LOGTASK(type, variable) type variable
#endif /* SEC_PREC2_SYSLOG */

#ifndef SEC_PRC2_LOGTASK_BEGIN
#define SEC_PRC2_LOGTASK_BEGIN
#endif /* SEC_PRC2_LOGTASK_BEGIN */

#ifndef SEC_PRC2_LOGTASK_END
#define SEC_PRC2_LOGTASK_END
#endif /* SEC_PREC2_LOGTASK_END */

#ifndef SEC_PRC3_LOGTASK
#define SEC_PRC3_LOGTASK(type, variable) type variable
#endif /* SEC_PREC3_SYSLOG */

#ifndef SEC_PRC3_LOGTASK_BEGIN
#define SEC_PRC3_LOGTASK_BEGIN
#endif /* SEC_PRC3_LOGTASK_BEGIN */

#ifndef SEC_PRC3_LOGTASK_END
#define SEC_PRC3_LOGTASK_END
#endif /* SEC_PREC3_LOGTASK_END */

#ifndef SEC_PRC4_LOGTASK
#define SEC_PRC4_LOGTASK(type, variable) type variable
#endif /* SEC_PREC4_SYSLOG */

#ifndef SEC_PRC4_LOGTASK_BEGIN
#define SEC_PRC4_LOGTASK_BEGIN
#endif /* SEC_PRC4_LOGTASK_BEGIN */

#ifndef SEC_PRC4_LOGTASK_END
#define SEC_PRC4_LOGTASK_END
#endif /* SEC_PREC4_LOGTASK_END */

/*
 *  システムログタスクの出力先のポートIDのテーブル
 */
#ifdef G_SYSLOG

SEC_LOGTASK_BEGIN
SEC_LOGTASK(static ID, logtask_portid);
SEC_LOGTASK_END

#else  /* G_SYSLOG */

#if TNUM_PRCID >= 1
SEC_PRC1_LOGTASK_BEGIN
SEC_PRC1_LOGTASK(static ID, logtask_portid_prc1);
SEC_PRC1_LOGTASK_END
#endif /* TNUM_PRCID >= 1 */

#if TNUM_PRCID >= 2
SEC_PRC2_LOGTASK_BEGIN
SEC_PRC2_LOGTASK(static ID, logtask_portid_prc2);
SEC_PRC2_LOGTASK_END
#endif /* TNUM_PRCID >= 2 */

#if TNUM_PRCID >= 3
SEC_PRC3_LOGTASK_BEGIN
SEC_PRC3_LOGTASK(static ID, logtask_portid_prc3);
SEC_PRC3_LOGTASK_END
#endif /* TNUM_PRCID >= 3 */

#if TNUM_PRCID >= 4
SEC_PRC4_LOGTASK_BEGIN
SEC_PRC4_LOGTASK(static ID, logtask_portid_prc4);
SEC_PRC4_LOGTASK_END
#endif /* TNUM_PRCID >= 4 */

#if TNUM_PRCID >= 5
#error Logtask Not Support more than 5 processor!
#endif /* TNUM_PRCID >= 5 */

static ID* const	p_logtask_portid_table[] = {
#if TNUM_PRCID >= 1
	&logtask_portid_prc1,
#endif /* TNUM_PRCID >= 1 */
#if TNUM_PRCID >= 2
	&logtask_portid_prc2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
	&logtask_portid_prc3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
	&logtask_portid_prc4,
#endif /* TNUM_PRCID >= 4 */
};
#endif /* G_SYSLOG */

/*
 *  自コアのポートIDへのアクセスマクロ
 */
#ifdef G_SYSLOG

Inline ID
get_my_logtask_portid(void)
{
	return logtask_portid;
}

Inline void
set_my_logtask_portid(ID id)
{
	logtask_portid = id;
}

#else  /* G_SYSLOG */

Inline ID
get_my_logtask_portid(void)
{
	ID prcid;
	sil_get_pid(&prcid);

	return *(p_logtask_portid_table[prcid - 1]);
}

Inline void
set_my_logtask_portid(ID id)
{
	ID prcid;
	sil_get_pid(&prcid);

	*(p_logtask_portid_table[prcid - 1]) = id;
}

#endif /* G_SYSLOG */

/*
 *  シリアルインタフェースへの1文字出力
 */
static void
logtask_putc(char c)
{
	ID my_logtask_portid = get_my_logtask_portid();

	(void) serial_wri_dat(my_logtask_portid, &c, 1);
}

/*
 *  システムログ出力の待ち合わせ
 */
ER
logtask_flush(uint_t count)
{
	T_SYSLOG_RLOG	rlog;
	T_SERIAL_RPOR	rpor;
	ER				ercd, rercd;
	ID my_logtask_portid = get_my_logtask_portid();

	if (sns_dpn()) {
		ercd = E_CTX;
	}
	else {
		for (;;) {
			if (syslog_ref_log(&rlog) < 0) {
				ercd = E_SYS;
				goto error_exit;
			}
			if (rlog.count <= count) {
				if (count == 0U) {
					/*
					 *  countが0の場合には，シリアルバッファが空かを確
					 *  認する．
					 */
					if (serial_ref_por(my_logtask_portid, &rpor) < 0) {
						ercd = E_SYS;
						goto error_exit;
					}
					if (rpor.wricnt == 0U) {
						ercd = E_OK;
						goto error_exit;
					}
				}
				else {
					ercd = E_OK;
					goto error_exit;
				}
			}

			/*
			 *  LOGTASK_FLUSH_WAITミリ秒待つ．
			 */
			rercd = dly_tsk(LOGTASK_FLUSH_WAIT);
			if (rercd < 0) {
				ercd = (rercd == E_RLWAI) ? rercd : E_SYS;
				goto error_exit;
			}
		}
	}

  error_exit:
	return(ercd);
}

/*
 *  システムログタスクの本体
 */
void
logtask_main(intptr_t exinf)
{
	SYSLOG	logbuf;
	uint_t	lostlog;
	ER_UINT	rercd;
	ID my_logtask_portid;

	my_logtask_portid = (ID) exinf;
	set_my_logtask_portid(my_logtask_portid);
	serial_opn_por(my_logtask_portid);
//	syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG));
	syslog_1(LOG_NOTICE, "System logging task is started on port %d.",
													my_logtask_portid);
	for (;;) {
		lostlog = 0U;
		while ((rercd = syslog_rea_log(&logbuf)) >= 0) {
			lostlog += (uint_t) rercd;
			if (logbuf.logtype >= LOG_TYPE_COMMENT) {
				if (lostlog > 0U) {
					syslog_lostmsg(lostlog, logtask_putc);
					lostlog = 0U;
				}
				syslog_print(&logbuf, logtask_putc);
				logtask_putc('\n');
			}
		}
		if (lostlog > 0U) {
			syslog_lostmsg(lostlog, logtask_putc);
		}
		if(E_OK != dly_tsk(LOGTASK_INTERVAL)){
			syslog_0(LOG_NOTICE, "syslog : Error dly_tsk() !");
		}
	}
}

/*
 *  システムログタスクの終了処理
 */
void
logtask_terminate(intptr_t exinf)
{
	char	c;
	SYSLOG	logbuf;
	bool_t	msgflg = false;
	ER_UINT	rercd;
	ID my_logtask_portid = get_my_logtask_portid();

	/*
	 *  シリアルインタフェースドライバの送信バッファに蓄積されたデータ
	 *  を，低レベル出力機能を用いて出力する．
	 */
	while (serial_get_chr(my_logtask_portid, &c)) {
		target_fput_log(c);
	}

	/*
	 *  ログバッファに記録されたログ情報を，低レベル出力機能を用いて出
	 *  力する．
	 */
	while ((rercd = syslog_rea_log(&logbuf)) >= 0) {
		if (!msgflg) {
			/*
			 *  ログバッファに残ったログ情報であることを示す文字列を出
			 *  力する．
			 */
			syslog_printf("-- buffered messages --\n", NULL, target_fput_log);
			msgflg = true;
		}
		if (rercd > 0) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		if (logbuf.logtype >= LOG_TYPE_COMMENT) {
			syslog_print(&logbuf, target_fput_log);
			target_fput_log('\n');
		}
	}
}
