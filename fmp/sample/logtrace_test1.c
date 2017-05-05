/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: logtrace_test1.c 1203 2016-07-18 07:05:08Z ertl-honda $
 */

/* 
 *		トレースログサンプルプログラム(1)の本体
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "logtrace_test1.h"
#include "logtrace/trace_config.h"

extern void target_fput_log(char c);

/*
 *  サービスコールのエラーのログ出力
 */
Inline void
svc_perror(const char *file, int_t line, const char *expr, ER ercd)
{
	if (ercd < 0) {
		t_perror(LOG_ERROR, file, line, expr, ercd);
	}
}

#define	SVC_PERROR(expr)	svc_perror(__FILE__, __LINE__, #expr, (expr))

/*
 *  並行実行されるタスクへのメッセージ領域
 */
volatile char	message[3];

/*
 *  ループ回数
 */
ulong_t	task_loop;		/* タスク内でのループ回数 */
ulong_t	tex_loop;		/* 例外処理ルーチン内でのループ回数 */

/*
 *  並行実行されるタスク
 */
void task(intptr_t exinf)
{
	int_t		tskno = (int_t) exinf;
	char		c;

	SVC_PERROR(ena_tex());
	while (1) {
		c = message[tskno-1];
		if (message[tskno-1] != 0) {
			message[tskno-1] = 0;
		}
		switch (c) {
		case 'e':
			syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
			SVC_PERROR(ext_tsk());
			assert(0);
		case 's':
			syslog(LOG_INFO, "#%d#slp_tsk()", tskno);
			SVC_PERROR(slp_tsk());
			break;
		case 'S':
			syslog(LOG_INFO, "#%d#tslp_tsk(10000)", tskno);
			SVC_PERROR(tslp_tsk(10000));
			break;
		case 'd':
			syslog(LOG_INFO, "#%d#dly_tsk(10000)", tskno);
			SVC_PERROR(dly_tsk(10000));
			break;
		case 'y':
			syslog(LOG_INFO, "#%d#dis_tex()", tskno);
			SVC_PERROR(dis_tex());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%d#ena_tex()", tskno);
			SVC_PERROR(ena_tex());
			break;
		case 'M':
			syslog(LOG_INFO, "#%d#mig_tsk(PE1)", tskno);
			SVC_PERROR(mig_tsk(0, 1));
			break;
		case 'm':
			syslog(LOG_INFO, "#%d#mig_tsk(PE2)", tskno);
			SVC_PERROR(mig_tsk(0, 2));
			break;
#ifdef CPUEXC1
		case 'z':
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			break;
		case 'Z':
			SVC_PERROR(loc_cpu());
			syslog(LOG_NOTICE, "#%d#raise CPU exception", tskno);
			RAISE_CPU_EXCEPTION;
			SVC_PERROR(unl_cpu());
			break;
#endif /* CPUEXC1 */
		default:
			break;
		}
	}
}

/*
 *  並行して実行されるタスク用のタスク例外処理ルーチン
 */
void tex_routine(TEXPTN texptn, intptr_t exinf)
{
	volatile ulong_t	i;
	int_t	tskno = (int_t) exinf;

	syslog(LOG_NOTICE, "task%d receives exception 0x%04x.", tskno, texptn);
	for (i = 0; i < tex_loop; i++);

	if ((texptn & 0x8000U) != 0U) {
		syslog(LOG_INFO, "#%d#ext_tsk()", tskno);
		SVC_PERROR(ext_tsk());
		assert(0);
	}
}

/*
 *  CPU例外ハンドラ
 */
#ifdef CPUEXC1

void
cpuexc_handler(void *p_excinf)
{
	ID		tskid;

	syslog(LOG_NOTICE, "CPU exception handler (p_excinf = %08p).", p_excinf);
	if (sns_ctx() != true) {
		syslog(LOG_WARNING,
					"sns_ctx() is not true in CPU exception handler.");
	}
	if (sns_dpn() != true) {
		syslog(LOG_WARNING,
					"sns_dpn() is not true in CPU exception handler.");
	}
	syslog(LOG_INFO, "sns_loc = %d sns_dsp = %d sns_tex = %d",
									sns_loc(), sns_dsp(), sns_tex());
	syslog(LOG_INFO, "xsns_dpn = %d xsns_xpn = %d",
									xsns_dpn(p_excinf), xsns_xpn(p_excinf));

	if (xsns_xpn(p_excinf)) {
		syslog(LOG_NOTICE, "Sample program ends with exception.");
		SVC_PERROR(ext_ker());
		assert(0);
	}

	SVC_PERROR(iget_tid(&tskid));
	SVC_PERROR(iras_tex(tskid, 0x8000U));
}

#endif /* CPUEXC1 */

/*
 *  周期ハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void cyclic_handler(intptr_t exinf)
{
	SVC_PERROR(irot_rdq(HIGH_PRIORITY));
	SVC_PERROR(irot_rdq(MID_PRIORITY));
	SVC_PERROR(irot_rdq(LOW_PRIORITY));
}

/*
 *  アラームハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void alarm_handler(intptr_t exinf)
{
	SVC_PERROR(irot_rdq(HIGH_PRIORITY));
	SVC_PERROR(irot_rdq(MID_PRIORITY));
	SVC_PERROR(irot_rdq(LOW_PRIORITY));
}

/*
 *  メインタスク
 */
void main_task(intptr_t exinf)
{
	ER_UINT	ercd; 

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(TASK_PORTID_PRC1);
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(TASK_PORTID_PRC1,
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
	 *  タスクの起動
	 */
	SVC_PERROR(act_tsk(TASK1));
	SVC_PERROR(act_tsk(TASK2));
	SVC_PERROR(act_tsk(TASK3));

	dly_tsk(10);
	rot_rdq(MID_PRIORITY); /* TASK2 に切り替え */
	dly_tsk(10);
	rot_rdq(MID_PRIORITY); /* TASK3 に切り替え */
	dly_tsk(10);
	rot_rdq(MID_PRIORITY); /* TASK1 に切り替え */

	dly_tsk(10);
	message[0] = 's';  /* TASK1 待ち状態へ */
	dly_tsk(10);
	message[1] = 's';  /* TASK2 待ち状態へ */
	dly_tsk(10);
	message[2] = 's';  /* TASK3 待ち状態へ */

	dly_tsk(10);
	wup_tsk(TASK1);    /* TASK1 実行可能状態へ */
	wup_tsk(TASK2);    /* TASK2 実行可能状態へ */
	wup_tsk(TASK3);    /* TASK3 実行可能状態へ */

	dly_tsk(10);
	mig_tsk(TASK1, 2); /* TASK1 をPE2へマイグレート */
	dly_tsk(10);
	mig_tsk(TASK2, 2); /* TASK2 をPE2へマイグレート */
	dly_tsk(10);
	mig_tsk(TASK3, 2); /* TASK3 をPE2へマイグレート */

	dly_tsk(10);
	message[0] = 'M';  /* TASK1 をPE1へマイグレート */
	dly_tsk(10);
	message[1] = 'M';  /* TASK2 をPE1へマイグレート */  
	dly_tsk(10);
	message[2] = 'M';  /* TASK3 をPE1へマイグレート */  

	dly_tsk(10);
	message[0] = 'e';  /* TASK1 終了 */
	dly_tsk(10);
	message[1] = 'e';  /* TASK2 終了 */
	dly_tsk(10);
	message[2] = 'e';  /* TASK3 終了 */

	/*
	 *  周期ハンドラの起動
	 */
	sta_cyc(CYCHDR1);

	dly_tsk(50);

	msta_cyc(CYCHDR1, 2); /* PE2へマイグレート */

	dly_tsk(50);

	msta_cyc(CYCHDR1, 1); /* PE1へマイグレート */

	dly_tsk(50);

	stp_cyc(CYCHDR1);

	/*
	 *  アラームハンドラの起動
	 */
	sta_alm(ALMHDR1, 10);

	dly_tsk(20);

	msta_alm(ALMHDR1, 10, 2); /* PE2へマイグレート */

	dly_tsk(20);

	msta_alm(ALMHDR1, 10, 1); /* PE1へマイグレート */

	dly_tsk(20);

	ext_ker();
}
