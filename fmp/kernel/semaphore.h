/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: semaphore.h 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		セマフォ機能
 */

#ifndef TOPPERS_SEMAPHORE_H
#define TOPPERS_SEMAPHORE_H

#include "wait.h"

/*
 *  セマフォ初期化ブロック
 *
 *  この構造体は，同期・通信オブジェクトの初期化ブロックの共通部分
 *  （WOBJINIB）を拡張（オブジェクト指向言語の継承に相当）したもので，
 *  最初のフィールドが共通になっている．
 */
typedef struct semaphore_initialization_block {
	ATR			sematr;			/* セマフォ属性 */
#if TTYPE_KLOCK == P_KLOCK
	LOCK		*p_obj_lock;		/* オブジェクトロックへのポインタ */
#endif /* TTYPE_KLOCK == P_KLOCK */
	uint_t		isemcnt;		/* セマフォの資源数の初期値 */
	uint_t		maxsem;			/* セマフォの最大資源数 */
} SEMINIB;

/*
 *  セマフォ管理ブロック
 *
 *  この構造体は，同期・通信オブジェクトの管理ブロックの共通部分（WOBJCB）
 *  を拡張（オブジェクト指向言語の継承に相当）したもので，最初の2つの
 *  フィールドが共通になっている．
 */
typedef struct semaphore_control_block {
	QUEUE		wait_queue;		/* セマフォ待ちキュー */
	const SEMINIB *p_seminib;	/* 初期化ブロックへのポインタ */
#if TTYPE_KLOCK == F_KLOCK
	LOCK		obj_lock;		/* オブジェクトロック */
#endif /* TTYPE_KLOCK == F_KLOCK */
	uint_t		semcnt;			/* セマフォ現在カウント値 */
} SEMCB;

/*
 *  セマフォIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_semid;

/*
 *  セマフォ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const SEMINIB	seminib_table[];

/*
 *  セマフォ管理ブロックへのポインタテーブル（kernel_cfg.c）
 */
extern SEMCB* const	p_semcb_table[];

/*
 *  セマフォ管理ブロックからセマフォIDを取り出すためのマクロ
 */
#define	SEMID(p_semcb)	((ID)(((p_semcb->p_seminib) - seminib_table) + TMIN_SEMID))

/*
 *  セマフォ機能の初期化
 */
extern void	initialize_semaphore(void);

#endif /* TOPPERS_SEMAPHORE_H */
