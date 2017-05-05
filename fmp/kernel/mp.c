/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2008-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: mp.c 905 2012-02-27 09:01:23Z ertl-honda $
 */

/*
 *		マルチプロセッサ管理モジュール
 */
#include "kernel_impl.h"
#include "mp.h"
#include "target_ipi.h"

#ifdef TOPPERS_mpini

#if TTYPE_KLOCK == G_KLOCK
/*
 *  ジャイアントロック
 */
LOCK giant_lock;
#endif /* TTYPE_KLOCK == G_KLOCK */

#ifndef OMIT_KER_REQ_ON_IPI
/*
 *  終了処理要求フラグ
 */
bool_t ext_ker_reqflg;
#endif /* OMIT_KER_REQ_ON_IPI */

/*
 *  マルチプロセッサ管理モジュールの初期化
 */
void
initialize_pcb(void)
{
	uint_t	i;
	PCB *my_p_pcb = get_my_p_pcb();

	/*
	 *  PCB のprcid を初期化
	 */
	my_p_pcb->prcid = ID_PRC(x_prc_index());

#if TTYPE_KLOCK == G_KLOCK
	/*
	 *  ジャイアントロックの初期化(マスタプロセッサのみ)
	 */
	if (x_sense_mprc()) {
		x_initialize_giant_lock(&giant_lock);
	}
#else /* TTYPE_KLOCK != G_KLOCK */
	/*
	 *  タスクロックの初期化
	 */
	x_initialize_tsk_lock(&(my_p_pcb->tsk_lock));

	/*
	 *  1段目の取得中のロックの初期化
	 */
	my_p_pcb->p_firstlock = NULL;

	/*
	 *  2段目の取得中のロック
	 */
	my_p_pcb->p_secondlock = NULL;
#endif /* TTYPE_KLOCK != G_KLOCK */

#if TTYPE_KLOCK == P_KLOCK
	/*
	 *  オブジェクトロックの初期化
	 */
	x_initialize_obj_lock(&(my_p_pcb->obj_lock));
#endif /* TTYPE_KLOCK == P_KLOCK */

	/*
	 *  タスク管理関係の初期化
	 */
	my_p_pcb->p_runtsk = my_p_pcb->p_schedtsk = NULL;
	my_p_pcb->reqflg = false;
	my_p_pcb->ipmflg = true;
	my_p_pcb->disdsp = false;
	my_p_pcb->dspflg = true;

	for (i = 0; i < TNUM_TPRI; i++) {
		queue_initialize(&(my_p_pcb->ready_queue[i]));
	}

	my_p_pcb->ready_primap = 0U;

	my_p_pcb->locspnid = 0U;

#ifndef OMIT_KER_REQ_ON_IPI
	if (x_sense_mprc()) {
		ext_ker_reqflg = false;
	}
#endif /*  OMIT_KER_REQ_ON_IPI */
}

#endif /* TOPPERS_mpini */

#ifdef TOPPERS_ipi_handler

/*
 *  プロセッサ間割込みハンドラ
 */
void
ipi_handler(void)
{
	PCB *my_p_pcb = get_my_p_pcb();

	target_ipi_clear();

#ifndef OMIT_KER_REQ_ON_IPI
	/*
	 *  終了処理要求をチェック
	 */
	if (ext_ker_reqflg) {
		ext_ker();
	}
#endif /* OMIT_KER_REQ_ON_IPI */

	my_p_pcb->reqflg = true;
}

#endif /* TOPPERS_ipi_handler */

#ifdef TOPPERS_disp_req

/*
 *  ディスパッチ要求
 *  対象が他プロセッサの場合はプロセッサ間割り込みを発行する
 *  ToDo  : 他プロセッサの場合だけ呼ぶように変更できないか
 */
bool_t
dispatch_request(PCB* p_pcb)
{
	if (p_pcb == get_my_p_pcb()) {
		return(true);
	}
	else {
		target_ipi_raise(p_pcb->prcid);
		return(false);
	}
}

#endif /* TOPPERS_disp_req */

#ifdef TOPPERS_ext_ker_req

#ifndef OMIT_KER_REQ_ON_IPI

/*
 *  終了処理要求
 */
void
ext_ker_request(void)
{
	ID prcid;

	/* すでに要求が出ていればリターン */
	if (ext_ker_reqflg) {
		return;
	}

	ext_ker_reqflg = true;

	for(prcid = 1; prcid <= TNUM_PRCID; prcid++){
		if (prcid != ID_PRC(x_prc_index())) {
			target_ipi_raise(prcid);
		}
	}
}

#endif /* OMIT_KER_REQ_ON_IPI */

#endif /* TOPPERS_ext_ker_req */

#ifndef OMIT_BARRIER_SYNC

/*
 *  バリア同期用変数
 */
static volatile uint_t prc_init[TNUM_PRCID];
static volatile uint_t sys_start;

/*
 *  バリア同期
 */
void
barrier_sync(uint_t phase)
{
	volatile uint_t i, j;
	volatile uint_t flag;

	prc_init[x_prc_index()] = phase;

	if (x_sense_mprc()) {
		do{
			flag = 0;
			for(i = 0; i < TNUM_PRCID; i++){
				if(prc_init[i] == phase){
					flag++;
				}
			}
			for(j = 0; j < 100; j++);
		}while (flag < TNUM_PRCID);
		sys_start = phase;
	}
	else {
		while(sys_start != phase) {
			for(j = 0; j < 100; j++);
		}
	}
}

#endif  /* OMIT_BARRIER_SYNC */

#if TTYPE_KLOCK != G_KLOCK

/*
 *  プロセッサロックないし細粒度ロックのロック取得・解放関数
 */

/*
 *		タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（タスクコンテキスト）
 */
PCB*
t_acquire_tsk_lock_prcid(ID prcid)
{
	PCB *p_pcb;

	p_pcb = get_mp_p_pcb(prcid);
 	t_acquire_lock(&(p_pcb->tsk_lock));
	return(p_pcb);
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 *
 *  t_acquire_lock()で割込みを許可するため，マイグレーションチェックも必要
 *  となる．
 * 
 *  TSTAT_RUNNABLE()がfalseの場合に割込みを許可するのは，割込みを受け付ける
 *  ことにより，ディスパッチを発生させるためである．割込みを許可しないと，
 *  再びロックを1回で取得した場合，TSTAT_RUNNABLE()がfalseとなり，無
 *  限にループする．
 */
PCB*
t_acquire_tsk_lock_self(void)
{
	TCB *my_p_tcb;
	PCB *my_p_pcb;

	while(true) {
		my_p_pcb = get_my_p_pcb();
		my_p_tcb = my_p_pcb->p_runtsk;
		TEST_G_LABEL("_test_t_acquire_tsk_lock_self"); 
		t_acquire_lock(&(my_p_pcb->tsk_lock));
		if (!TSTAT_RUNNABLE(my_p_tcb->tstat)) { 
			/* RUNNABLEでない場合 */
			x_release_lock(&(my_p_pcb->tsk_lock));
			t_unlock_cpu();
			t_lock_cpu();
		} else if (my_p_pcb != my_p_tcb->p_pcb) {
			/* 自タスクがマイグレートした場合 */
			x_release_lock(&(my_p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(my_p_pcb);
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/1段目）
 *  RUNNABLEチェックなし
 * 
 *  過度状態に対応するため，自タスクの状態を変更しないシステムコールは
 *  こちらを呼び出す．
 *
 *  t_acquire_lock()で割込みを許可するため，マイグレーションチェックも必要
 *  となる．  
 */
PCB*
t_acquire_tsk_lock_self_without_runnable_check(void)
{
	TCB *my_p_tcb;
	PCB *my_p_pcb;

	while(true) {
		my_p_pcb = get_my_p_pcb();
		my_p_tcb = my_p_pcb->p_runtsk;
		t_acquire_lock(&(my_p_pcb->tsk_lock));
		if (my_p_pcb != my_p_tcb->p_pcb) {
			/* 自タスクがマイグレートした場合 */
			x_release_lock(&(my_p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(my_p_pcb);
}

/*
 *  自プロセッサのタスクロックの取得（タスクコンテキスト/2段目）
 *  
 *  TSTAT_RUNNABLE()がfalseの場合に割込みを許可するのは，割込みを受け付け
 *  ることにより，ディスパッチを発生させるためである．割込みを許可しない
 *  と，再びロックを1回で取得した場合，TSTAT_RUNNABLE()がfalseとなり，無
 *  限にループする．
 * 
 *  t_acquire_nested_lock()内で割込みを許可した結果，割込みが入り，マイグレ
 *  ーションが発生する可能性があるが，その場合は，t_acquire_nested_lock()が
 *  trueで返る．そのため，ロック取得後のマイグレーションチェックの必要性
 *  はない（マイグレーションしていればretryする）．
 *
 *  t_acquire_nested_lock()がfalseで返ってきた場合は，その前後で実行するプ
 *  ロセッサが変わることがないため，p_pcbは前後で有効である．
 */
PCB*
t_acquire_nested_tsk_lock_self(LOCK *p_objlock)
{
	PCB *my_p_pcb;

	my_p_pcb = get_my_p_pcb();
	my_p_pcb->p_firstlock = p_objlock;
	TEST_G_LABEL("_test_t_acquire_nested_tsk_lock_self"); 
	if (t_acquire_nested_lock(&(my_p_pcb->tsk_lock))) {
		return(NULL);
	}
	my_p_pcb->p_firstlock = NULL;
	if (!TSTAT_RUNNABLE(my_p_pcb->p_runtsk->tstat)){
		x_release_lock(&(my_p_pcb->tsk_lock));
		x_release_lock(p_objlock);
		t_unlock_cpu();
		t_lock_cpu();
		return(NULL);
	}
	return(my_p_pcb);
}

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/1段目）
 *
 */
PCB*
t_acquire_tsk_lock(TCB *p_tcb)
{
	PCB *p_pcb;

	while(true) {
		p_pcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_tsk_lock"); 
		t_acquire_lock(&(p_pcb->tsk_lock));
		if (p_pcb != p_tcb->p_pcb) {
			/* 対象タスクがマイグレートした場合 */
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  任意タスク（自タスクも含む）のタスクロックの取得（タスクコンテキスト/2段目）
 * 
 *  t_acquire_nested_lock()内で割込みを許可した結果，割込みが入り，自タ
 *  スクがマイグレーションがする可能性があるが，その場合は，
 *  t_acquire_nested_lock()がtrueで返る．
 *
 *  t_acquire_nested_lock()がfalseで返ってきた場合は，その前後で自タスク
 *  を実行するプロセッサが変わることがないため，my_p_pcbは前後で有効である．
 */
PCB*
t_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock)
{
	PCB *my_p_pcb;
	PCB *p_pcb;
 
	while(true) {
		my_p_pcb = get_my_p_pcb();
		my_p_pcb->p_firstlock = p_objlock;
		p_pcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_nested_tsk_lock"); 
		if (t_acquire_nested_lock(&(p_pcb->tsk_lock))) {
			p_pcb = NULL;
			break;
		}
		my_p_pcb->p_firstlock = NULL;
		if (p_pcb != p_tcb->p_pcb) {
			/* 対象タスクがマイグレートした場合 */
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  任意のアラームハンドラのタスクロックの取得（タスクコンテキスト/1段目）
 */
PCB*
t_acquire_tsk_lock_alm(ALMCB *p_almcb)
{
	PCB *p_pcb;

	while(true){
		p_pcb = p_almcb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_tsk_lock_alm"); 
		t_acquire_lock(&(p_pcb->tsk_lock));
		if (p_pcb != p_almcb->p_pcb) {
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  任意の周期ハンドラのタスクロックの取得（タスクコンテキスト/1段目）
 */
PCB*
t_acquire_tsk_lock_cyc(CYCCB *p_cyccb)
{
	PCB *p_pcb;

	while(true){
		p_pcb = p_cyccb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_tsk_lock_cyc");
		t_acquire_lock(&(p_pcb->tsk_lock));
		if (p_pcb != p_cyccb->p_pcb) {
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}


/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/1段目）
 * 
 *  t_acquire_nested_lock()内で割込みを許可した結果，割込みが入り，マイグレ
 *  ーションが発生する可能性があるが，その場合は，t_acquire_nested_lock()が
 *  trueで返る．
 *
 *  t_acquire_nested_lock()がfalseで返ってきた場合は，その前後で実行するプ
 *  ロセッサが変わることがないため，my_p_pcbは前後で有効である．
 */
void 
t_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
						PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_dual_tsk_lock");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_dstpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_dstpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				/* 2段目のロックが取得できた場合 */
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_tcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
				} else {
					break;
				}
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_srcpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_tcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
				} else {
					break;
				}
			}
		}
		else {
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			if (*pp_srcpcb != p_tcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				break;
			}
		}
	}
}

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */
void
t_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_almcb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_dual_tsk_lock_alm");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_dstpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_dstpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				/* 2段目のロックが取得できた場合 */
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_almcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
				} else {
					break;
				}
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_srcpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_almcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
				} else {
					break;
				}
			}
		}
		else {
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			if (*pp_srcpcb != p_almcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				break;
			}
		}
	}
}

/*
 *  周期ハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */
void
t_acquire_dual_tsk_lock_cyc(CYCCB *p_cyccb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_cyccb->p_pcb;
		TEST_G_LABEL("_test_t_acquire_dual_tsk_lock_cyc");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_dstpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_dstpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				/* 2段目のロックが取得できた場合 */
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_cyccb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
				} else {
					break;
				}
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 1段目のロックを取得 */
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_srcpcb)->tsk_lock);
			if (!t_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_cyccb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
				} else {
					break;
				}
			}
		}
		else {
			t_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			if (*pp_srcpcb != p_cyccb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				break;
			}
		}
	}
}

/*
 *  タスクの2個のタスクロックの取得（タスクコンテキスト/2段目）
 *
 *  t_acquire_nested_lock()内で割込みを許可した結果，割込みが入り，マイグレ
 *  ーションが発生する可能性があるが，その場合は，t_acquire_nested_lock()が
 *  trueで返る．
 *
 *  t_acquire_nested_lock()がfalseで返ってきた場合は，その前後で実行するプ
 *  ロセッサが変わることがないため，my_p_pcbは前後で有効である．
 *
 *  本関数は現状，ter_tsk()からのみ呼び出される．ter_tsk()は発行対象が自タ
 *  スクと同じプロセッサに所属しているタスクに限られる．そのため，2段目の
 *  ロック取得時に割込みが入り対象のタスクがマイグレーションが発生した場合，
 *  対象タスクがマイグレーションした場合のチェックルーチンに達する前に，
 *  t_acquire_nested_lock()内でp_firstlock == NULL に該当してfalseでリター
 *  ンしてくる．すなわち，現状では対象タスクがマイグレーションした場合のチ
 *  ェックルーチンは必要ない．しなしながら，今後，仕様変更等で他のAPIで本
 *  関数を使う可能性があるため， チェックルーチンを残しておく．
 */
bool_t 
t_acquire_nested_dual_tsk_lock(TCB *p_tcb, ID dstprcid, LOCK *p_objlock,
							   PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;
	bool_t result = true;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_tcb->p_pcb;
		my_p_pcb = get_my_p_pcb();
		my_p_pcb->p_firstlock = p_objlock;
		TEST_G_LABEL("_test_t_acquire_nested_dual_tsk_lock");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 2段目のロックを取得 */
			if (t_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				break;
			}
			my_p_pcb->p_secondlock = &((*pp_dstpcb)->tsk_lock);
			/* 3段目のロックを取得 */
			if (t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				break;
			}
			my_p_pcb->p_secondlock = NULL;
			my_p_pcb->p_firstlock = NULL;
			if (*pp_srcpcb != p_tcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
				x_release_lock(&((*pp_dstpcb)->tsk_lock));
			} else {
				result = false;
				break;
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 2段目のロックを取得 */
			if (t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				break;
			}
			my_p_pcb->p_secondlock = &((*pp_srcpcb)->tsk_lock);
			/* 3段目のロックを取得 */
			if (t_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				break;
			}
			my_p_pcb->p_secondlock = NULL;
			my_p_pcb->p_firstlock = NULL;
			if (*pp_srcpcb != p_tcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_dstpcb)->tsk_lock));
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			}else {
				result = false;
				break;
			}
		} else {
			/* 2段目のロックを取得 */
			if (t_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				break;
			}
			my_p_pcb->p_firstlock = NULL;
			if (*pp_srcpcb != p_tcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				result = false;
				break;
			}
		}
	}
	return result;
}

/*
 *		非タスクコンテキスト用のタスクロック取得関数
 */

/*
 *  任意プロセッサのタスクロックの取得（非タスクコンテキスト）
 */
PCB*
i_acquire_tsk_lock_prcid(ID prcid)
{
	PCB *p_pcb;

	p_pcb = get_mp_p_pcb(prcid);
	i_acquire_lock(&(p_pcb->tsk_lock));
	return(get_mp_p_pcb(prcid));
}

/*
 *  自プロセッサのタスクロックの取得（非タスクコンテキスト/1段目）
 *
 *  マイグレーションすることはないため，タスクコンテキストで実施していた
 *  マイグレーションチェックは必要ない．  
 */ 
PCB*
i_acquire_tsk_lock_self(void)
{
	PCB *my_p_pcb;

	my_p_pcb = get_my_p_pcb();
	i_acquire_lock(&(my_p_pcb->tsk_lock));
	return(get_my_p_pcb());
}


/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/1段目）
 *
 */
PCB*
i_acquire_tsk_lock(TCB *p_tcb) 
{
	PCB *p_pcb;

	while(true) {
		p_pcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_i_acquire_tsk_lock");
		i_acquire_lock(&(p_pcb->tsk_lock));
		if (p_pcb != p_tcb->p_pcb) {
			/* 対象タスクがマイグレートした場合 */
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  任意タスクのタスクロックの取得（非タスクコンテキスト/2段目）
 */
PCB*
i_acquire_nested_tsk_lock(TCB *p_tcb, LOCK *p_objlock)
{
	PCB* my_p_pcb;
	PCB* p_pcb;

	while(true) {
		my_p_pcb = get_my_p_pcb();
		my_p_pcb->p_firstlock = p_objlock;
		p_pcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_i_acquire_nested_tsk_lock");
		if (i_acquire_nested_lock(&(p_pcb->tsk_lock))) {
			p_pcb = NULL;
			break;
		}
		my_p_pcb->p_firstlock = NULL;
		if (p_pcb != p_tcb->p_pcb) {
			/* 対象タスクがマイグレートした場合 */
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  任意のアラームハンドラのタスクロックの取得（非タスクコンテキスト）
 */
PCB*
i_acquire_tsk_lock_alm(ALMCB *p_almcb)
{
	PCB *p_pcb;

	while(true){
		p_pcb = p_almcb->p_pcb;
		TEST_G_LABEL("_test_i_acquire_tsk_lock_alm");
		i_acquire_lock(&(p_pcb->tsk_lock));
		if (p_pcb != p_almcb->p_pcb) {
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  2個のタスクロックの取得（非タスクコンテキスト/1段目）
 */
void 
i_acquire_dual_tsk_lock(TCB *p_tcb, ID dstprcid, 
                                    PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_i_acquire_dual_tsk_lock");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 1段目のロックを取得 */
			i_acquire_lock(&((*pp_dstpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_dstpcb)->tsk_lock);
			if (!i_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				/* 2段目のロックが取得できた場合 */
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_tcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
				} else {
					break;
				}
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 1段目のロックを取得 */
			i_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_srcpcb)->tsk_lock);
			if (!i_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_tcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
				} else {
					break;
				}
			}
		}
		else {
			i_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			if (*pp_srcpcb != p_tcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				break;
			}
		}
	}
}

/*
 *  アラームハンドラの2個のタスクロックの取得（タスクコンテキスト/1段目）
 */
void
i_acquire_dual_tsk_lock_alm(ALMCB *p_almcb, ID dstprcid, 
							PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	PCB* my_p_pcb;

	*pp_dstpcb = get_mp_p_pcb(dstprcid);

	while(true) {
		*pp_srcpcb = p_almcb->p_pcb;
		TEST_G_LABEL("_test_i_acquire_dual_tsk_lock_alm");
		if ((*pp_srcpcb)->prcid > dstprcid) {
			/* 1段目のロックを取得 */
			i_acquire_lock(&((*pp_dstpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_dstpcb)->tsk_lock);
			if (!i_acquire_nested_lock(&((*pp_srcpcb)->tsk_lock))) {
				/* 2段目のロックが取得できた場合 */
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_almcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
				} else {
					break;
				}
			}
		} else if ((*pp_srcpcb)->prcid < dstprcid) {
			/* 1段目のロックを取得 */
			i_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			my_p_pcb = get_my_p_pcb();
			my_p_pcb->p_firstlock = &((*pp_srcpcb)->tsk_lock);
			if (!i_acquire_nested_lock(&((*pp_dstpcb)->tsk_lock))) {
				my_p_pcb->p_firstlock = NULL;
				if (*pp_srcpcb != p_almcb->p_pcb) {
					/* 対象タスクがマイグレートした場合 */
					x_release_lock(&((*pp_dstpcb)->tsk_lock));
					x_release_lock(&((*pp_srcpcb)->tsk_lock));
				} else {
					break;
				}
			}
		}
		else {
			i_acquire_lock(&((*pp_srcpcb)->tsk_lock));
			if (*pp_srcpcb != p_almcb->p_pcb) {
				/* 対象タスクがマイグレートした場合 */
				x_release_lock(&((*pp_srcpcb)->tsk_lock));
			} else {
				return;
			}
		}
	}
}

/*
 *		タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 *  オブジェクトロックの取得（タスクコンテキスト）
 */
void
t_acquire_obj_lock(LOCK *p_objlock) 
{ 
	t_acquire_lock(p_objlock);
}

/*
 *		非タスクコンテキスト用のオブジェクトロック取得関数
 */

/*
 * オブジェクトロックの取得（非タスクコンテキスト）
 */
void 
i_acquire_obj_lock(LOCK *p_objlock) 
{ 
	i_acquire_lock(p_objlock);
}

/*
 *		タスクロック解放関数
 */

/*
 *  タスクロックの解放（タスクコンテキスト）
 */ 
void
release_tsk_lock(PCB *p_pcb)
{
	x_release_lock(&(p_pcb->tsk_lock));
}

/*
 *  タスクロックの解放（2段目）
 */ 
void
release_nested_tsk_lock(PCB *p_pcb)
{
	x_release_lock(&(p_pcb->tsk_lock));
}

/*
 *  2個のタスクロックの解放
 */ 
void
release_dual_tsk_lock(PCB *p_srcpcb,  PCB *p_dstpcb)
{
	x_release_lock(&(p_srcpcb->tsk_lock));
	x_release_lock(&(p_dstpcb->tsk_lock));
}

/*
 *		オブジェクトロック解放関数
 */

/*
 * オブジェクトロックの解放（非タスクコンテキスト）
 */ 
void
release_obj_lock(LOCK *p_objlock)
{
	x_release_lock(p_objlock);
}

/*
 *		その他のロック関数
 */

/*
 *  プリエンプションしない自タスクのタスクロックの取得
 */
PCB* 
acquire_tsk_lock_without_preemption_self(void)
{
	PCB *my_p_pcb;

	my_p_pcb = get_my_p_pcb();
	x_acquire_lock_without_preemption(&(my_p_pcb->tsk_lock));

	return(my_p_pcb);
}

/*
 *  プリエンプションしないタスクロックの取得（2段目）
 */
PCB*
acquire_nested_tsk_lock_without_preemption(TCB *p_tcb)
{
	PCB *p_pcb;

	while(true) {
		p_pcb = p_tcb->p_pcb;
		TEST_G_LABEL("_test_acquire_nested_tsk_lock_without_preemption");
		x_acquire_lock_without_preemption(&(p_pcb->tsk_lock));
		if (p_pcb != p_tcb->p_pcb) {
			/* 対象タスクがマイグレートした場合 */
			x_release_lock(&(p_pcb->tsk_lock));
		} else {
			break;
		}
	}
	return(p_pcb);
}

/*
 *  プリエンプションしないタスクの2個のタスクロックの取得（タスクコンテキスト）
 */
void 
acquire_dual_tsk_lock_without_preemption(TCB *p_tcb, ID dstprcid, 
										 PCB **pp_srcpcb, PCB **pp_dstpcb)
{
	*pp_dstpcb = get_mp_p_pcb(dstprcid);
	*pp_srcpcb = p_tcb->p_pcb;

	if ((*pp_srcpcb)->prcid > dstprcid) {
		/* 1段目のロックを取得 */
		x_acquire_lock_without_preemption(&((*pp_dstpcb)->tsk_lock));
		/* 2段目のロックを取得 */
		x_acquire_lock_without_preemption(&((*pp_srcpcb)->tsk_lock));
	} else if ((*pp_srcpcb)->prcid < dstprcid) {
		/* 1段目のロックを取得 */
		x_acquire_lock_without_preemption(&((*pp_srcpcb)->tsk_lock));
		/* 2段目のロックを取得 */
		x_acquire_lock_without_preemption(&((*pp_dstpcb)->tsk_lock));
	}
	else {
		x_acquire_lock_without_preemption(&((*pp_srcpcb)->tsk_lock));
	}
}

#endif /* TTYPE_KLOCK != G_KLOCK */
