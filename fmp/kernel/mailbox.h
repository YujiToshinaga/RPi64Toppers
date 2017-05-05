/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: mailbox.h 378 2009-09-11 16:21:24Z ertl-honda $
 */

/*
 *		メールボックス機能
 */

#ifndef TOPPERS_MAILBOX_H
#define TOPPERS_MAILBOX_H

#include <queue.h>

/*
 *  メールボックス初期化ブロック
 *
 *  この構造体は，同期・通信オブジェクトの初期化ブロックの共通部分
 *  （WOBJINIB）を拡張（オブジェクト指向言語の継承に相当）したもので，
 *  最初のフィールドが共通になっている．
 */
typedef struct mailbox_initialization_block {
	ATR			mbxatr;			/* メールボックス属性 */
#if TTYPE_KLOCK == P_KLOCK
	LOCK		*p_obj_lock;		/* ロックへのポインタ */
#endif /* TTYPE_KLOCK == P_KLOCK */
	PRI			maxmpri;		/* メッセージ優先度の最大値 */
} MBXINIB;

/*
 *  メールボックス管理ブロック
 *
 *  この構造体は，同期・通信オブジェクトの管理ブロックの共通部分（WOBJCB）
 *  を拡張（オブジェクト指向言語の継承に相当）したもので，最初の2つの
 *  フィールドが共通になっている．
 *
 *  メッセージキューがメッセージの優先度順の場合には，pk_lastは使わな
 *  い．また，メッセージキューが空の場合（pk_headがNULLの場合）にも，
 *  pk_lastは無効である．
 */
typedef struct mailbox_control_block {
	QUEUE		wait_queue;		/* メールボックス待ちキュー */
	const MBXINIB *p_mbxinib;	/* 初期化ブロックへのポインタ */
#if TTYPE_KLOCK == F_KLOCK
	LOCK		obj_lock;		/* オブジェクトロック */
#endif /* TTYPE_KLOCK == F_KLOCK */
	T_MSG		*pk_head;		/* 先頭のメッセージ */
	T_MSG		*pk_last;		/* 末尾のメッセージ */
} MBXCB;

/*
 *  メールボックスIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_mbxid;

/*
 *  メールボックス初期化ブロックのエリア（kernel_cfg.c）
 */
extern const MBXINIB	mbxinib_table[];

/*
 *  メールボックス管理ブロックへのポインタテーブル（kernel_cfg.c）
 */
extern MBXCB* const		p_mbxcb_table[];

/*
 *  メールボックス管理ブロックからメールボックスIDを取り出すためのマクロ
 */
#define	MBXID(p_mbxcb)	((ID)((((p_mbxcb->p_mbxinib) - mbxinib_table) + TMIN_MBXID)))

/*
 *  メールボックス機能の初期化
 */
extern void	initialize_mailbox(void);

#endif /* TOPPERS_MAILBOX_H */
