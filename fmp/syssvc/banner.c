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
 *  @(#) $Id: banner.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		カーネル起動メッセージの出力
 */

#include <kernel.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "banner.h"

#ifndef TARGET_COPYRIGHT
#define TARGET_COPYRIGHT
#endif /* TARGET_COPYRIGHT */

static const char banner[] = "\n"
"TOPPERS/FMP Kernel Release %d.%X.%d for " TARGET_NAME
" (" __DATE__ ", " __TIME__ ")\n"
"Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory\n"
"                            Toyohashi Univ. of Technology, JAPAN\n"
"Copyright (C) 2004-2015 by Embedded and Real-Time Systems Laboratory\n"
"            Graduate School of Information Science, Nagoya Univ., JAPAN\n"
TARGET_COPYRIGHT;

static const char prc_banner[] = "Processor %d start.";

void
print_banner_copyright(intptr_t exinf)
{
	syslog_3(LOG_NOTICE, banner,
			 (TKERNEL_PRVER >> 12) & 0x0fU,
			 (TKERNEL_PRVER >> 4) & 0xffU,
			 TKERNEL_PRVER & 0x0fU
			 );
}

void
print_banner(intptr_t exinf)
{
	ID prcid;

	sil_get_pid(&prcid);
	syslog_1(LOG_NOTICE, prc_banner, prcid);
}
