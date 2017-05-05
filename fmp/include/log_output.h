/*
 *  TOPPERS Software
 *      Toyohashi Open Platform for Embedded Real-Time Systems
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
 *  @(#) $Id: log_output.h 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		システムログのフォーマット出力
 */

#ifndef TOPPERS_LOG_OUTPUT_H
#define TOPPERS_LOG_OUTPUT_H

#ifdef __cplusplus
extern "C" {
#endif

#include <t_syslog.h>

/*
 *  文字列整形出力
 *
 *  formatで指定されるフォーマット記述とp_argsで指定される引数列から作
 *  成したメッセージを，1文字出力関数putcを用いて出力する．
 */
extern void	syslog_printf(const char *format, const intptr_t *p_args,
										void (*putc)(char)) throw();

/*
 *  ログ情報の出力
 *
 *  p_syslogで指定されるログ情報を文字列に整形し，1文字出力関数putcを用
 *  いて出力する．
 */
extern void	syslog_print(const SYSLOG *p_syslog, void (*putc)(char)) throw();

/*
 *  ログ情報喪失メッセージ文字列
 *
 *  lostlog個のログ情報が失われた旨のメッセージを，1文字出力関数putcを
 *  用いて出力する．
 */
extern void	syslog_lostmsg(uint_t lostlog, void (*putc)(char)) throw();

#ifdef __cplusplus
}
#endif

#endif /* TOPPERS_LOG_OUTPUT_H */
