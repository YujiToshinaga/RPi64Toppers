/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
 * 
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
 *  @(#) $Id: test_lib.c 1203 2016-07-18 07:05:08Z ertl-honda $
 */


/* 
 *		テストプログラム用ライブラリ
 */

#include <kernel.h>
#include <sil.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <log_output.h>
#include "syssvc/syslog.h"
#include "target_syssvc.h"
#include <test_lib.h>

/*
 *  チェックポイント通過カウント変数配列要素を，０で初期化されることを想定
 */ 
static ID	check_count[TNUM_PRCID];

/*
 *	自己診断関数
 */
static BIT_FUNC	check_bit_func = NULL;

/*
 *	自己診断関数の設定
 */
void
set_bit_func(BIT_FUNC bit_func)
{
	check_bit_func = bit_func;
}

/*
 *  テストプログラムの開始
 */
void
test_start(char *progname)
{
	syslog_1(LOG_NOTICE, "Test program: %s", progname);
}

/*
 *  システムログの出力処理
 */
void
syslog_flush(void)
{
	SYSLOG	logbuf;
	ER_UINT	rercd;

	/*
	 *  ログバッファに記録されたログ情報を，低レベル出力機能を用いて出
	 *  力する．
	 */
	while ((rercd = syslog_rea_log(&logbuf)) >= 0) {
		if (rercd > 0) {
			syslog_lostmsg((uint_t) rercd, target_fput_log);
		}
		if (logbuf.logtype >= LOG_TYPE_COMMENT) {
			syslog_print(&logbuf, target_fput_log);
			target_fput_log('\n');
		}
	}
}

/*
 *	テストプログラムの終了
 */
void
test_finish(void)
{
	SIL_PRE_LOC;

	SIL_LOC_INT();
	syslog_flush();
	(void) ext_ker();
}

/*
 *	チェックポイント
 */

void
check_point(uint_t count)
{
	bool_t	errorflag = false;
	ER		rercd;
	ID      prcid;

	SIL_PRE_LOC;

	/*
	 *  割込みロック状態に
	 */
	SIL_LOC_INT();

	/*
	 *  PRCID取得
	 */
	sil_get_pid(&prcid);

	/*
	 *  シーケンスチェック
	 */
	if (++check_count[prcid-1] == count) {
		syslog_2(LOG_NOTICE, "PE %d : Check point %d passed.", prcid, count);
	}
	else {
		syslog_2(LOG_ERROR, "## PE %d : Unexpected check point %d.", prcid, count);
		errorflag = true;
	}

	/*
	 *  カーネルの内部状態の検査
	 */
	if (check_bit_func != NULL) {
		rercd = (*check_bit_func)();
		if (rercd < 0) {
			syslog_2(LOG_ERROR, "## Internal inconsistency detected (%s, %d).",
								itron_strerror(rercd), SERCD(rercd));
			errorflag = true;
		}
	}

	/*
	 *  エラーが検出された場合は，テストプログラムを終了する．
	 */
	if (errorflag) {
		test_finish();
	}

	/*
	 *  割込みロック状態を解除
	 */
	SIL_UNL_INT();
}

/*
 *	完了チェックポイントでの同期用変数
 */
static volatile uint_t check_finish_enter[TNUM_PRCID];
static volatile uint_t check_finish_leave;

/*
 *	完了チェックポイント
 */
void
check_finish(uint_t count)
{
	volatile uint_t i, j;
	volatile uint_t flag;
	ID       prcid;

	/*
	 *  PRCID取得
	 */
	sil_get_pid(&prcid);

	check_point(count);
	syslog_1(LOG_NOTICE, "PE %d : All check points passed.", prcid);

	/*
	 * ext_ker()発行前に全プロセッサの同期をとる
	 */
	check_finish_enter[prcid - 1] = 1;

	if (prcid == TOPPERS_MASTER_PRCID) {
		do{
			flag = 0;
			for(i = 0; i < TNUM_PRCID; i++){
				if (check_finish_enter[i] == 1){
					flag++;
				}
			}
			for (j = 0; j < 100; j++);
		}while (flag < TNUM_PRCID);
		check_finish_leave = 1;
	}
	else {
		while (check_finish_leave != 1) {
			for (j = 0; j < 100; j++);
		}
	}

	test_finish();
}
/*
 *	条件チェックのエラー処理
 */
void
_check_assert(const char *expr, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Assertion `%s' failed at %s:%u.",
								expr, file, line);
	test_finish();
}

/*
 *	エラーコードチェックのエラー処理
 */
void
_check_ercd(ER ercd, const char *file, int_t line)
{
	syslog_3(LOG_ERROR, "## Unexpected error %s detected at %s:%u.",
								itron_strerror(ercd), file, line);
	test_finish();
}

/*
 *  バリア同期用変数
 */
static volatile uint_t local_phase[TNUM_PRCID];
static volatile uint_t global_phase;

/*
 *  バリア同期
 */
void
barrier_sync(uint_t phase)
{
	volatile uint_t i, j;
	volatile uint_t flag;
	ID     prcid;

	SIL_PRE_LOC;

	/*
	 *  割込みロック状態に
	 */
	SIL_LOC_INT();

	/*
	 *  PRCID取得
	 *  どのような状態でも取得できるように sil_get_pid() を使用する．
	 */
	sil_get_pid(&prcid);

	local_phase[prcid - 1] = phase;

	if (prcid == TOPPERS_MASTER_PRCID) {
		do{
			flag = 0;
			for (i = 0; i < TNUM_PRCID; i++){
				if (local_phase[i] == phase){
					flag++;
				}
			}
			for (j = 0; j < 100; j++);
		}while (flag < TNUM_PRCID);
		global_phase = phase;
	}
	else {
		while (global_phase != phase) {
			for (j = 0; j < 100; j++);
		}
	}

	/*
	 *  割込みロック状態を解除
	 */
	SIL_UNL_INT();
}
