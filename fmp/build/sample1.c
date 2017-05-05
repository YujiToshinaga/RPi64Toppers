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
 *  @(#) $Id: sample1.c 1203 2016-07-18 07:05:08Z ertl-honda $
 */

/* 
 *  サンプルプログラム(1)の本体
 *
 *  FMPカーネルの基本的な動作を確認するためのサンプルプログラム．
 *
 *  プログラムの概要:
 *
 *  ユーザインタフェースを受け持つメインタスク（タスクID: MAIN_TASK，優
 *  先度: MAIN_PRIORITY）と，3つの並行実行されるタスク（タスクID:
 *  TASK1～TASK3，初期優先度: MID_PRIORITY）で構成される．また，起動周
 *  期が2秒の周期ハンドラ（周期ハンドラID: CYCHDR1）を用いる．
 *
 *  並行実行されるタスクは，task_loop回空ループを実行する度に，タスクが
 *  実行中であることをあらわすメッセージを表示する．空ループを実行する
 *  のは，空ループなしでメッセージを出力すると，多量のメッセージが出力
 *  され，プログラムの動作が確認しずらくなるためである．また，低速なシ
 *  リアルポートを用いてメッセージを出力する場合に，すべてのメッセージ
 *  が出力できるように，メッセージの量を制限するという理由もある．
 *
 *  周期ハンドラは，三つの優先度（HIGH_PRIORITY，MID_PRIORITY，
 *  LOW_PRIORITY）のレディキューを回転させる．プログラムの起動直後は，
 *  周期ハンドラは停止状態になっている．
 *
 *  メインタスクは，シリアルI/Oポートからの文字入力を行い（文字入力を
 *  待っている間は，並行実行されるタスクが実行されている），入力された
 *  文字に対応した処理を実行する．入力された文字と処理の関係は次の通り．
 *  Control-Cまたは'Q'が入力されると，プログラムを終了する．
 *
 *  '1' : 対象タスクをTASK1に切り換える（初期設定）．
 *  '2' : 対象タスクをTASK2に切り換える．
 *  '3' : 対象タスクをTASK3に切り換える．
 *  '4' : クラス1に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR1に切り替える．
 *        対象アラームハンドラをALMHDR1に切り替える．
 *  '5' : クラス2に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR2に切り替える．
 *        対象アラームハンドラをALMHDR2に切り替える．
 *  '6' : クラス3に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR3に切り替える．
 *        対象アラームハンドラをALMHDR3に切り替える．
 *  '7' : クラス4に所属するタスクを指定する．
 *        対象周期ハンドラをCYCHDR4に切り替える．
 *        対象アラームハンドラをALMHDR4に切り替える．
 *  '8' : プロセッサ1を指定する．
 *  '9' : プロセッサ2を指定する．
 *  '0' : プロセッサ3を指定する．
 *  '-' : プロセッサ4を指定する．
 *  'a' : 対象タスクをact_tskにより起動する．
 *  'A' : 対象タスクに対する起動要求をcan_actによりキャンセルする．
 *  'e' : 対象タスクにext_tskを呼び出させ，終了させる．
 *  't' : 対象タスクをter_tskにより強制終了する．
 *  '>' : 対象タスクの優先度をHIGH_PRIORITYにする．
 *  '=' : 対象タスクの優先度をMID_PRIORITYにする．
 *  '<' : 対象タスクの優先度をLOW_PRIORITYにする．
 *  'G' : 対象タスクの優先度をget_priで読み出す．
 *  's' : 対象タスクにslp_tskを呼び出させ，起床待ちにさせる．
 *  'S' : 対象タスクにtslp_tsk(10秒)を呼び出させ，起床待ちにさせる．
 *  'w' : 対象タスクをwup_tskにより起床する．
 *  'W' : 対象タスクに対する起床要求をcan_wupによりキャンセルする．
 *  'l' : 対象タスクをrel_waiにより強制的に待ち解除にする．
 *  'u' : 対象タスクをsus_tskにより強制待ち状態にする．
 *  'm' : 対象タスクの強制待ち状態をrsm_tskにより解除する．
 *  'd' : 対象タスクにdly_tsk(10秒)を呼び出させ，時間経過待ちにさせる．
 *  'x' : 対象タスクに例外パターン0x0001の例外処理を要求する．
 *  'X' : 対象タスクに例外パターン0x0002の例外処理を要求する．
 *  'y' : 対象タスクにdis_texを呼び出させ，タスク例外を禁止する．
 *  'Y' : 対象タスクにena_texを呼び出させ，タスク例外を許可する．
 *  'r' : 3つの優先度（HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY）のレ
 *        ディキューを回転させる．
 *  'c' : 周期ハンドラを動作開始させる．
 *  'C' : 周期ハンドラを動作停止させる．
 *  'I' : 周期ハンドラの割付けプロセッサを変更する． 
 *  'b' : アラームハンドラを5秒後に起動するよう動作開始させる．
 *  'B' : アラームハンドラを動作停止させる．
 *  'E' : アラームハンドラの割付けプロセッサを変更する．   
 *  'z' : 対象タスクにCPU例外を発生させる（タスクを終了させる）．
 *  'Z' : 対象タスクにCPUロック状態でCPU例外を発生させる（プログラムを
 *        終了する）．
 *  'i' : 対象タスクをmig_tskにより割付けプロセッサを変更する． 
 *  'g' : 対象タスクにmig_tskを呼び出させ，割付けプロセッサを変更する． 
 *  'f' : 対象タスクをmact_tskにより起動する．  
 *  'p' : 対象タスクにget_pidを呼び出させ，プロセッサ番号を表示させる．
 *  'V' : get_utmで性能評価用システム時刻を2回読む．
 *  'v' : 発行したシステムコールを表示する（デフォルト）．
 *  'q' : 発行したシステムコールを表示しない．
 */

#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

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

#define SERVER_REQ_MIG_TSK  0x01
#define SERVER_REQ_TER_TSK  0x02

ID const server_dtqid[TNUM_PRCID] = {
    SERVER_DTQ1,
#if TNUM_PRCID >= 2
    SERVER_DTQ2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    SERVER_DTQ3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    SERVER_DTQ4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  タスクIDのテーブル
 */
uint_t const sample_tskid[TNUM_PRCID][3] = {
    {TASK1_1,TASK1_2,TASK1_3},
#if TNUM_PRCID >= 2
    {TASK2_1,TASK2_2,TASK2_3},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {TASK3_1,TASK3_2,TASK3_3},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {TASK4_1,TASK4_2,TASK4_3},
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  タスクNO(コンソール表示用)のテーブル
 */
uint_t const sample_tskno[TNUM_PRCID][3] = {
    {0x11,0x12,0x13},
#if TNUM_PRCID >= 2
    {0x21,0x22,0x23},
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    {0x31,0x32,0x33},
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    {0x41,0x42,0x43},
#endif /* TNUM_PRCID >= 4 */
};

#define TSKNO_TO_TSKID(tskno)  sample_tskid[(tskno >> 4) - 1][(tskno & 0xf) -1]

/*
 *  周期ハンドラIDのテーブル
 */
uint_t const sample_cycid[TNUM_PRCID] = {
    CYCHDR1,
#if TNUM_PRCID >= 2
    CYCHDR2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    CYCHDR3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    CYCHDR4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  アラームIDのテーブル
 */
uint_t const sample_almid[TNUM_PRCID] = {
    ALMHDR1,
#if TNUM_PRCID >= 2
    ALMHDR2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    ALMHDR3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    ALMHDR4,
#endif /* TNUM_PRCID >= 4 */
};

/*
 *  シリアルポート番号のテーブル
 */
uint_t const serial_port[TNUM_PRCID] = {
#ifndef G_SYSLOG
	TASK_PORTID_PRC1,
#else /* G_SYSLOG */
	TASK_PORTID_G_SYSLOG,
#endif /* G_SYSLOG */
    
#if TNUM_PRCID >= 2
    TASK_PORTID_PRC2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    TASK_PORTID_PRC3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    TASK_PORTID_PRC4,
#endif /* TNUM_PRCID >= 4 */
};


/*
 *  SERVERタスク
 *  他のタスクからの要求を受けて mig_tsk()/ter_tsk() を実行する． 
 */
void server_task(intptr_t exinf)
{
	intptr_t req;
	ID		tskid;
	int_t	tskno;
	ID		prcid;
	uint8_t syscall;
	ID dtqid = server_dtqid[exinf - 1];

	syslog(LOG_NOTICE, "Server task %d starts.", (int_t) exinf);

	while(1){
		rcv_dtq(dtqid, &req);
		tskno = (req >> 16) & 0xff;
		tskid = TSKNO_TO_TSKID(tskno);
		prcid = (req >>  8) & 0xff;
		syscall = req & 0xff;

		/*
		 *  mig_tsk/ter_tskは，対象のタスクがどのプロセッサに割り付けられて
		 *  いるか分からないため，メインタスクからの要求を全てのサーバータ
		 *  スクで実行するため，対象タスクとは異なるプロセッサに割り付けら
		 *  れているプロセッサではエラーとなる．
		 */
		if (syscall == SERVER_REQ_MIG_TSK) {
			syslog(LOG_INFO, "#Server task %d : mig_tsk(0x%x, %d)", (int_t) exinf, tskno, prcid);
			SVC_PERROR(mig_tsk(tskid, prcid));
		}
		else if (syscall == SERVER_REQ_TER_TSK) {
			syslog(LOG_INFO, "#Server task %d : ter_tsk(0x%x)", (int_t) exinf, tskno);
			SVC_PERROR(ter_tsk(tskid));
		}
	}
}

/*
 *  並行実行されるタスクへのメッセージ領域
 */
char	message[TNUM_PRCID][3];

/*
 *  タスクのマイグレーション先のプロセッサの指定
 */
ID  tsk_mig_prc;

/*
 *  ループ回数
 */
ulong_t	task_loop[TNUM_PRCID];		/* タスク内でのループ回数 */
ulong_t	tex_loop[TNUM_PRCID];			/* 例外処理ルーチン内でのループ回数 */

/*
 *  並行実行されるタスク
 */
void task(intptr_t exinf)
{
	volatile ulong_t	i;
	int_t		n = 0;
	int_t		tskno = (int_t) (exinf & 0xffff);
	int_t		classno = (int_t) (exinf >> 16);
	const char	*graph[] = { "|", "  +", "    *" };
	char		c;
	ID		prcid;
	ER		ercd;
	  
	SVC_PERROR(ena_tex());
	while (true) {
		syslog(LOG_NOTICE, "task%d_%d is running (%03d).   %s",
			   classno, tskno, ++n, graph[tskno-1]);
		for (i = 0; i < task_loop[classno-1]; i++);
		c = message[classno-1][tskno-1];
		message[classno-1][tskno-1] = 0;
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
		case 'g':
			syslog(LOG_INFO, "#%d#mig_tsk(0, %d)", tskno, tsk_mig_prc);
			SVC_PERROR(mig_tsk(0, tsk_mig_prc));
			break;
		case 'y':
			syslog(LOG_INFO, "#%d#dis_tex()", tskno);
			SVC_PERROR(dis_tex());
			break;
		case 'Y':
			syslog(LOG_INFO, "#%d#ena_tex()", tskno);
			SVC_PERROR(ena_tex());
			break;
		case 'p':
			syslog(LOG_INFO, "#%d#get_pid()", tskno);
			SVC_PERROR(ercd = get_pid(&prcid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "Processor id is %d", prcid);
			}			
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
	int_t	tskno = (int_t) (exinf & 0xffff);
	int_t	classno = (int_t) (exinf >> 16);

	syslog(LOG_NOTICE, "task%d receives exception 0x%04x.", tskno, texptn);
	for (i = 0; i < tex_loop[classno-1]; i++);

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

	syslog(LOG_NOTICE, "CPU exception handler1 (p_excinf = %08p).", p_excinf);
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
	ID		prcid = exinf;

	syslog(LOG_NOTICE, "cyclic_handler %d start!", exinf);
	SVC_PERROR(imrot_rdq(HIGH_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(MID_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(LOW_PRIORITY, prcid));
}

/*
 *  アラームハンドラ
 *
 *  HIGH_PRIORITY，MID_PRIORITY，LOW_PRIORITY の各優先度のレディキュー
 *  を回転させる．
 */
void alarm_handler(intptr_t exinf)
{
	ID		prcid = exinf;

	syslog(LOG_NOTICE, "alarm_handler %d start!", exinf);
	SVC_PERROR(imrot_rdq(HIGH_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(MID_PRIORITY, prcid));
	SVC_PERROR(imrot_rdq(LOW_PRIORITY, prcid));
}

static uint_t counter;

/*
 *  グローバル初期化ルーチン
 */
void
global_inirtn(intptr_t exinf)
{
	counter = 1;
}

/*
 *  グローバル終了ルーチン
 */
void
global_terrtn(intptr_t exinf)
{
	syslog(LOG_EMERG, "global_terrtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  ローカル初期化ルーチン
 */
void
local_inirtn(intptr_t exinf)
{
	syslog(LOG_NOTICE, "local_inirtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  ローカル終了ルーチン
 */
void
local_terrtn(intptr_t exinf)
{
	syslog(LOG_NOTICE, "local_terrtn exinf = %d, counter = %d", exinf, counter++);
}

/*
 *  メインタスク
 */
void main_task(intptr_t exinf)
{
	char	c;
	ER_UINT	ercd;
	PRI		tskpri;
	ID		clsid = exinf;
	ID		prcid = exinf;
	ID		tskid = sample_tskid[(int_t)exinf-1][0];
	int_t	tskno = sample_tskno[(int_t)exinf-1][0];
	ID		cycid = sample_cycid[(int_t)exinf-1];
	ID		almid = sample_almid[(int_t)exinf-1];
	bool_t  update_select = true;
	uint_t  tsk_select    = 1;
	uint_t  tme_select    = exinf;
	uint_t  class_select  = exinf;
	uint_t  prc_select    = exinf;
	uint32_t server_req;
#ifdef TOPPERS_SUPPORT_GET_UTM
	SYSUTM	utime1, utime2;
#endif /* TOPPERS_SUPPORT_GET_UTM */ 
#ifndef TASK_LOOP
	volatile ulong_t	i;
	SYSTIM	stime1, stime2;
#endif /* TASK_LOOP */
#ifdef G_SYSLOG
	uint_t e;
#endif /* G_SYSLOG */

	tsk_mig_prc = 1;

	SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
	syslog(LOG_NOTICE, "Sample program starts (exinf = %d).", (int_t) exinf);

	/*
	 *  シリアルポートの初期化
	 *
	 *  システムログタスクと同じシリアルポートを使う場合など，シリアル
	 *  ポートがオープン済みの場合にはここでE_OBJエラーになるが，支障は
	 *  ない．
	 */
	ercd = serial_opn_por(((int_t)serial_port[exinf-1]));
	if (ercd < 0 && MERCD(ercd) != E_OBJ) {
		syslog(LOG_ERROR, "%s (%d) reported by `serial_opn_por'.",
									itron_strerror(ercd), SERCD(ercd));
	}
	SVC_PERROR(serial_ctl_por(serial_port[exinf-1],
							(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV)));

	/*
	 *  ループ回数の設定
	 *
	 *  並行実行されるタスク内での空ループの回数（task_loop）は，空ルー
	 *  プの実行時間が約0.4秒になるように設定する．この設定のために，
	 *  LOOP_REF回の空ループの実行時間を，その前後でget_timを呼ぶことで
	 *  測定し，その測定結果から空ループの実行時間が0.4秒になるループ回
	 *  数を求め，task_loopに設定する．
	 *
	 *  LOOP_REFは，デフォルトでは1,000,000に設定しているが，想定したよ
	 *  り遅いプロセッサでは，サンプルプログラムの実行開始に時間がかか
	 *  りすぎるという問題を生じる．逆に想定したより速いプロセッサでは，
	 *  LOOP_REF回の空ループの実行時間が短くなり，task_loopに設定する値
	 *  の誤差が大きくなるという問題がある．
	 *
	 *  そこで，そのようなターゲットでは，target_test.hで，LOOP_REFを適
	 *  切な値に定義するのが望ましい．
	 *
	 *  また，task_loopの値を固定したい場合には，その値をTASK_LOOPにマ
	 *  クロ定義する．TASK_LOOPがマクロ定義されている場合，上記の測定を
	 *  行わずに，TASK_LOOPに定義された値を空ループの回数とする．
	 *
	 * ターゲットによっては，空ループの実行時間の1回目の測定で，本来よ
	 * りも長めになるものがある．このようなターゲットでは，MEASURE_TWICE
	 * をマクロ定義することで，1回目の測定結果を捨てて，2回目の測定結果
	 * を使う．
	 *
	 *  タスク例外処理ルーチン内での空ループの回数（tex_loop）は，
	 *  task_loopの4分の1の値（空ループの実行時間が0.1秒になるループ回
	 *  数）に設定する．
	 */
#ifdef TASK_LOOP
	task_loop[(int_t)exinf-1] = TASK_LOOP;
#else /* TASK_LOOP */

#ifdef MEASURE_TWICE
	task_loop = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop; i++);
	SVC_PERROR(get_tim(&stime2));
#endif /* MEASURE_TWICE */

	task_loop[(int_t)exinf-1] = LOOP_REF;
	SVC_PERROR(get_tim(&stime1));
	for (i = 0; i < task_loop[(int_t)exinf-1]; i++);
	SVC_PERROR(get_tim(&stime2));
	task_loop[(int_t)exinf-1] = LOOP_REF * 400UL / (stime2 - stime1);
#endif /* TASK_LOOP */
	tex_loop[(int_t)exinf-1] = task_loop[(int_t)exinf-1] / 4;

#ifdef G_SYSLOG
	for (e = 1; e < TNUM_PRCID; e++) {
		task_loop[e] = task_loop[0];
		tex_loop[e] = tex_loop[0];
	}
#endif /* G_SYSLOG */

	/*
	 *  タスクの起動
	 */
	SVC_PERROR(act_tsk(tskid));

	/*
 	 *  メインループ
	 */
	do {
		if (update_select) {
			prcid = prc_select;
			clsid = class_select;
			tskid = sample_tskid[clsid - 1][tsk_select-1];
			tskno = sample_tskno[clsid - 1][tsk_select-1];
			cycid = sample_cycid[tme_select-1];
			almid = sample_almid[tme_select-1];
			tsk_mig_prc = prcid;
			update_select = false;
			syslog(LOG_INFO, "select tskno 0x%x", tskno);
			syslog(LOG_INFO, "select cycid %d", cycid);
			syslog(LOG_INFO, "select almid %d", almid);
			syslog(LOG_INFO, "select processor %d", prcid);
			syslog(LOG_INFO, "select class     %d", clsid);
		}
		
		SVC_PERROR(serial_rea_dat(serial_port[exinf-1], &c, 1));
		
		switch (c) {
		case 'p':
		case 'e':
		case 's':
		case 'S':
		case 'd':
		case 'y':
		case 'Y':
		case 'z':
		case 'Z':
		case 'g':
			message[clsid-1][(tskno & 0x0f) - 1] = c;
			break;
		case '1':
			tsk_select = 1;
			update_select = true;
			break;
		case '2':
			tsk_select = 2;
			update_select = true;
			break;
		case '3':
			tsk_select = 3;
			update_select = true;
			break;
		case '4':
			tme_select = 1;
			class_select = 1;
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '5':
			tme_select = 2;
			class_select = 2;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '6':
			tme_select = 3;
			class_select = 3;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '7':
			tme_select = 4;
			class_select = 4;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case '8':
			prc_select = 1;
			update_select = true;
			break;
#if TNUM_PRCID >= 2
		case '9':
			prc_select = 2;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
		case '0':
			prc_select = 3;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
		case '-':
			prc_select = 4;
			update_select = true;
			break;
#endif /* TNUM_PRCID >= 4 */
		case 'a':
			syslog(LOG_INFO, "#act_tsk(0x%x)", tskno);
			SVC_PERROR(act_tsk(tskid));
			break;
		case 'A':
			syslog(LOG_INFO, "#can_act(0x%x)", tskno);
			SVC_PERROR(ercd = can_act(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_act(0x%x) returns %d", tskno, ercd);
			}
			break;
		case 'f':
			syslog(LOG_INFO, "#mact_tsk(0x%x, 0x%x)", tskno, tsk_mig_prc);
			SVC_PERROR(mact_tsk(tskid, tsk_mig_prc));
			break;
		case 't':
			server_req = tskno << 16 | SERVER_REQ_TER_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case '>':
			syslog(LOG_INFO, "#chg_pri(0x%x, HIGH_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, HIGH_PRIORITY));
			break;
		case '=':
			syslog(LOG_INFO, "#chg_pri(0x%x, MID_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, MID_PRIORITY));
			break;
		case '<':
			syslog(LOG_INFO, "#chg_pri(0x%x, LOW_PRIORITY)", tskno);
			SVC_PERROR(chg_pri(tskid, LOW_PRIORITY));
			break;
		case 'G':
			syslog(LOG_INFO, "#get_pri(0x%x, &tskpri)", tskno);
			SVC_PERROR(ercd = get_pri(tskid, &tskpri));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "priority of task 0x%x is %d", tskno, tskpri);
			}
			break;
		case 'w':
			syslog(LOG_INFO, "#wup_tsk(0x%x)", tskno);
			SVC_PERROR(wup_tsk(tskid));
			break;
		case 'W':
			syslog(LOG_INFO, "#can_wup(0x%x)", tskno);
			SVC_PERROR(ercd = can_wup(tskid));
			if (ercd >= 0) {
				syslog(LOG_NOTICE, "can_wup(0x%x) returns %d", tskno, ercd);
			}
			break;
		case 'l':
			syslog(LOG_INFO, "#rel_wai(0x%x)", tskno);
			SVC_PERROR(rel_wai(tskid));
			break;
		case 'u':
			syslog(LOG_INFO, "#sus_tsk(0x%x)", tskno);
			SVC_PERROR(sus_tsk(tskid));
			break;
		case 'i':
			server_req = (tskno << 16) | (prcid << 8) | SERVER_REQ_MIG_TSK;
			SVC_PERROR(snd_dtq(SERVER_DTQ1, server_req));
#if TNUM_PRCID >= 2
			SVC_PERROR(snd_dtq(SERVER_DTQ2, server_req));
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
			SVC_PERROR(snd_dtq(SERVER_DTQ3, server_req));
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
			SVC_PERROR(snd_dtq(SERVER_DTQ4, server_req));
#endif /* TNUM_PRCID >= 4 */
			break;
		case 'm':
			syslog(LOG_INFO, "#rsm_tsk(0x%x)", tskno);
			SVC_PERROR(rsm_tsk(tskid));
			break;
		case 'x':
			syslog(LOG_INFO, "#ras_tex(0x%x, 0x0001U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0001U));
			break;
		case 'X':
			syslog(LOG_INFO, "#ras_tex(0x%x, 0x0002U)", tskno);
			SVC_PERROR(ras_tex(tskid, 0x0002U));
			break;
		case 'r':
			syslog(LOG_INFO, "#mrot_rdq(three priorities, %d)", prcid);
			SVC_PERROR(mrot_rdq(HIGH_PRIORITY, prcid));
			SVC_PERROR(mrot_rdq(MID_PRIORITY, prcid));
			SVC_PERROR(mrot_rdq(LOW_PRIORITY, prcid));
			break;
		case 'c':
			syslog(LOG_INFO, "#sta_cyc(%d)", cycid);
			SVC_PERROR(sta_cyc(cycid));
			break;
		case 'C':
			syslog(LOG_INFO, "#stp_cyc(%d)", cycid);
			SVC_PERROR(stp_cyc(cycid));
			break;
		case 'I':
			syslog(LOG_INFO, "#msta_cyc(%d, %d)", cycid, prcid);
			SVC_PERROR(msta_cyc(cycid, prcid));
			break;
		case 'b':
			syslog(LOG_INFO, "#sta_alm(%d, %d)", almid, ALM_TIME);
			SVC_PERROR(sta_alm(almid, ALM_TIME));
			break;
		case 'B':
			syslog(LOG_INFO, "#stp_alm(%d)", almid);
			SVC_PERROR(stp_alm(almid));
			break;
		case 'E':
			syslog(LOG_INFO, "#msta_alm(%d, %d, %d)", almid, ALM_TIME, prcid);
			SVC_PERROR(msta_alm(almid, ALM_TIME, prcid));
			break;
		case 'V':
#ifdef TOPPERS_SUPPORT_GET_UTM
			SVC_PERROR(get_utm(&utime1));
			SVC_PERROR(get_utm(&utime2));
			syslog(LOG_NOTICE, "utime1 = %ld, utime2 = %ld",
										(ulong_t) utime1, (ulong_t) utime2);
#else /* TOPPERS_SUPPORT_GET_UTM */
			syslog(LOG_NOTICE, "get_utm is not supported.");
#endif /* TOPPERS_SUPPORT_GET_UTM */
			break;
		case 'v':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG)));
			break;
		case 'q':
			SVC_PERROR(syslog_msk_log(LOG_UPTO(LOG_NOTICE), LOG_UPTO(LOG_EMERG)));
			break;
#ifdef BIT_KERNEL
		case ' ':
			SVC_PERROR(loc_cpu());
			{
				extern ER	bit_kernel(void);

				SVC_PERROR(ercd = bit_kernel());
				if (ercd >= 0) {
					syslog(LOG_NOTICE, "bit_kernel passed.");
				}
			}
			SVC_PERROR(unl_cpu());
			break;
#endif /* BIT_KERNEL */
		default:
			break;
		}
	} while (c != '\003' && c != 'Q');

	syslog(LOG_NOTICE, "Sample program ends.");
	SVC_PERROR(ext_ker());
	assert(0);
}
