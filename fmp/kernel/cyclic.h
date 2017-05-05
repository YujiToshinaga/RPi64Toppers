/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: cyclic.h 378 2009-09-11 16:21:24Z ertl-honda $
 */

/*
 *		周期ハンドラ機能
 */

#ifndef TOPPERS_CYCLIC_H
#define TOPPERS_CYCLIC_H

#include "time_event.h"

/*
 *  周期ハンドラ初期化ブロック
 */
typedef struct cyclic_handler_initialization_block {
	ATR			cycatr;			/* 周期ハンドラ属性 */
	intptr_t	exinf;			/* 周期ハンドラの拡張情報 */
	CYCHDR		cychdr;			/* 周期ハンドラの起動番地 */
	RELTIM		cyctim;			/* 周期ハンドラの起動周期 */
	RELTIM		cycphs;			/* 周期ハンドラの起動位相 */
#ifdef TOPPERS_SYSTIM_LOCAL
	ID			iaffinity;		/* 周期ハンドラの初期割付けプロセッサ */
	uint_t		affinity_mask;	/* 周期ハンドラの割付け可能プロセッサ */
#endif /* TOPPERS_SYSTIM_LOCAL */
} CYCINIB;

/*
 *  周期ハンドラ管理ブロック
 */
typedef struct cyclic_handler_control_block {
	const CYCINIB *p_cycinib;	/* 初期化ブロックへのポインタ */
	bool_t		cycsta;			/* 周期ハンドラの動作状態 */
	EVTTIM		evttim;			/* 次に周期ハンドラを起動する時刻 */
	TMEVTB		tmevtb;			/* タイムイベントブロック */
	PCB			*p_pcb;			/* 動作するプロセッサのPCB */
} CYCCB;

/*
 *  周期ハンドラIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_cycid;

/*
 *  周期ハンドラ初期化ブロックのエリア（kernel_cfg.c）
 */
extern const CYCINIB	cycinib_table[];

/*
 *  周期ハンドラ管理ブロックのエリアへのポインタ（kernel_cfg.c）
 */
extern CYCCB* const	p_cyccb_table[];

/*
 *  周期ハンドラ機能の初期化
 */
extern void	initialize_cyclic(void);

/*
 *  周期ハンドラ起動ルーチン
 */
extern void	call_cychdr(CYCCB *p_cyccb);

#endif /* TOPPERS_CYCLIC_H */
