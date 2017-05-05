/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2007-2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: spin_lock.h 771 2011-01-21 02:06:07Z ertl-honda $
 */

/*
 *		スピンロック機能
 */

#ifndef TOPPERS_SPIN_LOCK_H
#define TOPPERS_SPIN_LOCK_H

#if TTYPE_SPN == NATIVE_SPN

/*
 *  スピンロック初期化ブロック
 */
typedef struct spin_lock_initialization_block {
	ATR			spnatr;			/* スピンロック属性 */
} SPNINIB;

/*
 *  スピンロック管理ブロック
 */
typedef struct spin_lock_control_block {
	SPNLOCK		spn_lock;		/* ロック操作対象 */
	const SPNINIB *p_spninib;	/* 初期化ブロックへのポインタ */
	bool_t		lock_flg;		/* スピンロックの現在のロック状態 */
} SPNCB;

#else /* TTYPE_SPN == EMULATE_SPN */

#if TTYPE_KLOCK == F_KLOCK
#error Can not use EMULATE_SPN with F_KLOCK. Use NATIVE_SPN.
#endif /* TTYPE_KLOCK == F_KLOCK */

/*
 *  スピンロック初期化ブロック
 */
typedef struct spin_lock_initialization_block {
	ATR			spnatr;			/* スピンロック属性 */
#if TTYPE_KLOCK == P_KLOCK
	LOCK		*p_obj_lock;		/* ロックへのポインタ */
#endif /* TTYPE_KLOCK == P_KLOCK */
} SPNINIB;

/*
 *  スピンロック管理ブロック
 */
typedef struct spin_lock_control_block {
	QUEUE		dummy;			/* 他の同期オブジェクトと同じマクロを使うためのダミー */        
	const SPNINIB *p_spninib;	/* 初期化ブロックへのポインタ */
	bool_t		lock_flg;		/* スピンロックの現在のロック状態 */
} SPNCB;

#endif /* TTYPE_SPN == EMULATE_SPN */

/*
 *  スピンロック初期化ブロックのエリア（kernel_cfg.c）
 */
extern const SPNINIB	spninib_table[];

/*
 *  スピンロック管理ブロックへのポインタテーブル（kernel_cfg.c）
 */
extern SPNCB* const	p_spncb_table[];

/*
 *  スピンロックIDの最大値（kernel_cfg.c）
 */
extern const ID	tmax_spnid;

/*
 *  スピンロック機能の初期化
 */
extern void	initialize_spin_lock(void);

/*
 *  スピンロックの強制解放（PCB中のlockspnidを対象にする）
 */
extern void	force_unlock_spin(PCB *my_p_pcb);

#endif /* TOPPERS_SPIN_LOCK_H */
