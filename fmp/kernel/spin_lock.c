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
 *  @(#) $Id: spin_lock.c 891 2012-02-08 12:34:04Z ertl-honda $
 */

/*
 *		スピンロック機能
 */

#include "kernel_impl.h"
#include "check.h"
#include "task.h"
#include "spin_lock.h"
#include "mp.h"

/*
 *  トレースログマクロのデフォルト定義
 */
#ifndef LOG_LOC_SPN_ENTER
#define LOG_LOC_SPN_ENTER(spnid)
#endif  /* LOG_LOC_SPN_ENTER */

#ifndef LOG_LOC_SPN_LEAVE
#define LOG_LOC_SPN_LEAVE(ercd)
#endif  /* LOG_LOC_SPN_LEAVE */

#ifndef LOG_ILOC_SPN_ENTER
#define LOG_ILOC_SPN_ENTER(spnid)
#endif  /* LOG_ILOC_SPN_ENTER */

#ifndef LOG_ILOC_SPN_LEAVE
#define LOG_ILOC_SPN_LEAVE(ercd)
#endif  /* LOG_ILOC_SPN_LEAVE */

#ifndef LOG_TRY_SPN_ENTER
#define LOG_TRY_SPN_ENTER(spnid)
#endif  /* LOG_TRY_SPN_ENTER */

#ifndef LOG_TRY_SPN_LEAVE
#define LOG_TRY_SPN_LEAVE(ercd)
#endif  /* LOG_TRY_SPN_LEAVE */

#ifndef LOG_ITRY_SPN_ENTER
#define LOG_ITRY_SPN_ENTER(spnid)
#endif  /* LOG_ITRY_SPN_ENTER */

#ifndef LOG_ITRY_SPN_LEAVE
#define LOG_ITRY_SPN_LEAVE(ercd)
#endif  /* LOG_ITRY_SPN_LEAVE */

#ifndef LOG_UNL_SPN_ENTER
#define LOG_UNL_SPN_ENTER(spnid)
#endif  /* LOG_UNL_SPN_ENTER */

#ifndef LOG_UNL_SPN_LEAVE
#define LOG_UNL_SPN_LEAVE(ercd)
#endif  /* LOG_UNL_SPN_LEAVE */

#ifndef LOG_IUNL_SPN_ENTER
#define LOG_IUNL_SPN_ENTER(spnid)
#endif  /* LOG_IUNL_SPN_ENTER */

#ifndef LOG_IUNL_SPN_LEAVE
#define LOG_IUNL_SPN_LEAVE(ercd)
#endif  /* LOG_IUNL_SPN_LEAVE */

#ifndef LOG_REF_SPN_ENTER
#define LOG_REF_SPN_ENTER(spnid, pk_rspn)
#endif  /* LOG_REF_SPN_ENTER */

#ifndef LOG_REF_SPN_LEAVE
#define LOG_REF_SPN_LEAVE(ercd, pk_rspn)
#endif  /* LOG_REF_SPN_LEAVE */

/*
 *  スピンロックの数
 */
#define tnum_spn		((uint_t)(tmax_spnid - TMIN_SPNID + 1))
#define INDEX_SPN(spnid)	((uint_t)((spnid) - TMIN_SPNID))
#define get_spncb(spnid)	(p_spncb_table[INDEX_SPN(spnid)])

#if TTYPE_SPN == NATIVE_SPN

/*
 *  ネイティブ方式
 */

/*
 *  スピンロック機能の初期化
 */
#ifdef TOPPERS_spnini

void
initialize_spin_lock(void)
{
	uint_t	i;
	SPNCB	*p_spncb;
	PCB		*my_p_pcb;

	my_p_pcb = get_my_p_pcb();

	my_p_pcb->locspnid = 0;
	if (x_sense_mprc()) {
		for (i = 0; i < tnum_spn;  i++) {
			p_spncb = p_spncb_table[i];
			p_spncb->p_spninib = &(spninib_table[i]);
			p_spncb->lock_flg = false;
			x_initialize_spin((i + 1), &(p_spncb->spn_lock));
		}
	}
}

#endif /* TOPPERS_spnini */

/*
 *  スピンロックの強制解放（PCB中のlocspnidを対象にする）
 */
#ifdef TOPPERS_force_unlock_spin

void
force_unlock_spin(PCB *my_p_pcb)
{
	SPNCB   *p_spncb;

	if (my_p_pcb->locspnid != 0U) {
		p_spncb = get_spncb(my_p_pcb->locspnid);
		x_unlock_spin(&(p_spncb->spn_lock));
		p_spncb->lock_flg = false;
		my_p_pcb->locspnid = 0;
	}
}

#endif /* TOPPERS_force_unlock_spin */

/*
 *  スピンロックの取得
 */
#ifdef TOPPERS_loc_spn

ER
loc_spn(ID spnid)
{
	ER		ercd;
	SPNCB	*p_spncb;

	LOG_LOC_SPN_ENTER(spnid);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	/*
	 *  すでにロックを取得している場合（locspnid != 0）なら，CHECK_TSKCTX_UNL()
	 *  でエラーとなるので，locspnidのチェックは必要ない．
	 */
	t_lock_cpu();
	t_lock_spin(&(p_spncb->spn_lock));
	p_spncb->lock_flg = true;
	(get_my_p_pcb())->locspnid = spnid;
	ercd = E_OK;

  error_exit:
	LOG_LOC_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_spn */

/*
 *  スピンロックの取得（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iloc_spn

ER
iloc_spn(ID spnid)
{
	ER		ercd;
	SPNCB	*p_spncb;

	LOG_ILOC_SPN_ENTER(spnid);
	CHECK_INTCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	/*
	 *  すでにロックを取得している場合（locspnid != 0）なら，CHECK_TSKCTX_UNL()
	 *  でエラーとなるので，locspnidのチェックは必要ない．
	 */
	i_lock_cpu();
	i_lock_spin(&(p_spncb->spn_lock));
	p_spncb->lock_flg = true;
	(get_my_p_pcb())->locspnid = spnid;
	ercd = E_OK;

  error_exit:
	LOG_ILOC_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iloc_spn */

/*
 *  スピンロックの取得（ポーリング）
 */
#ifdef TOPPERS_try_spn

ER
try_spn(ID spnid)
{
	ER		ercd;
	SPNCB	*p_spncb;

	LOG_TRY_SPN_ENTER(spnid);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	t_lock_cpu();
	if (x_try_lock_spin(&(p_spncb->spn_lock))) {
		t_unlock_cpu();
		ercd = E_OBJ;
	}
	else {
		p_spncb->lock_flg = true;
		(get_my_p_pcb())->locspnid = spnid;
		ercd = E_OK;
	}

  error_exit:
	LOG_TRY_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_try_spn */

/*
 *  スピンロックの取得（ポーリング, 非タスクコンテキスト用）
 */
#ifdef TOPPERS_itry_spn

ER
itry_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;

	LOG_ITRY_SPN_ENTER(spnid);
	CHECK_INTCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	i_lock_cpu();
	if (x_try_lock_spin(&(p_spncb->spn_lock))) {
		i_unlock_cpu();
		ercd = E_OBJ;
	}
	else {
		p_spncb->lock_flg = true;
		(get_my_p_pcb())->locspnid = spnid;
		ercd = E_OK;
	}

  error_exit:
	LOG_ITRY_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_itry_spn */

/*
 *  スピンロックの返却
 */
#ifdef TOPPERS_unl_spn

ER
unl_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;
	bool_t	locked;

	LOG_UNL_SPN_ENTER(spnid);
	CHECK_TSKCTX();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	my_p_pcb = get_my_p_pcb();
	if (my_p_pcb->locspnid != spnid) {
		if (!locked) {
			t_unlock_cpu();
		}
		ercd = E_ILUSE;
	}
	else {
		/*
		 *  スピンロック取得とのレーシングを避けるため，スピンロックを
		 *  解放する前に設定．
		 */
		my_p_pcb->locspnid = 0;
		p_spncb->lock_flg = false;
		x_unlock_spin(&(p_spncb->spn_lock));
		t_unlock_cpu();
		ercd = E_OK;
	}

  error_exit:
	LOG_UNL_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_spn */

/*
 *  スピンロックの返却（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iunl_spn

ER
iunl_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;
	bool_t	locked;

	LOG_IUNL_SPN_ENTER(spnid);
	CHECK_INTCTX();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	locked = i_sense_lock();
	if (!locked) {
		i_lock_cpu();
	}
	my_p_pcb = get_my_p_pcb();
	if (my_p_pcb->locspnid != spnid) {
		if (!locked) {
			i_unlock_cpu();
		}
		ercd = E_ILUSE;
	}
	else {
		/*
		 *  スピンロック取得とのレーシングを避けるため，スピンロックを
		 *  解放する前に設定．
		 */
		p_spncb->lock_flg = false;
		my_p_pcb->locspnid = 0;
		x_unlock_spin(&(p_spncb->spn_lock));
		i_unlock_cpu();
		ercd = E_OK;
	}

  error_exit:
	LOG_IUNL_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iunl_spn */

/*
 *  スピンロックの状態参照
 */
#ifdef TOPPERS_ref_spn

ER
ref_spn(ID spnid, T_RSPN *pk_rspn)
{
	ER		ercd;
	SPNCB   *p_spncb;

	LOG_REF_SPN_ENTER(spnid, pk_rspn);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	pk_rspn->spnstat = (p_spncb->lock_flg) ? TSPN_LOC : TSPN_UNL;
	ercd = E_OK;

  error_exit:
	LOG_REF_SPN_LEAVE(ercd, pk_rspn);
	return(ercd);
}

#endif /* TOPPERS_ref_spn */

#else /* TTYPE_SPN == EMULATE_SPN */

/*
 *  エミュレーション方式
 */

/*
 *  スピンロック機能の初期化
 */
#ifdef TOPPERS_spnini

void
initialize_spin_lock(void)
{
	uint_t	i;
	SPNCB	*p_spncb;
	PCB		*my_p_pcb;

	my_p_pcb = get_my_p_pcb();

	my_p_pcb->locspnid = 0;
	if (x_sense_mprc()) {
		for (i = 0; i < tnum_spn;  i++) {
			p_spncb = p_spncb_table[i];
			p_spncb->lock_flg = false;
			p_spncb->p_spninib = &(spninib_table[i]);
		}
	}
}

#endif /* TOPPERS_spnini */

/*
 *  スピンロックの強制解放（PCB中のlocspnidを対象にする）
 */
#ifdef TOPPERS_force_unlock_spin

void
force_unlock_spin(PCB *my_p_pcb)
{
	SPNCB   *p_spncb;

	if (my_p_pcb->locspnid != 0U) {
		p_spncb = get_spncb(my_p_pcb->locspnid);
		p_spncb->lock_flg = false;
		my_p_pcb->locspnid = 0;
	}
}

#endif /* TOPPERS_force_unlock_spin */

/*
 *  スピンロックの取得
 */
#ifdef TOPPERS_loc_spn

ER
loc_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;

	LOG_LOC_SPN_ENTER(spnid);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	t_lock_cpu();
  retry:
	t_acquire_obj_lock(&GET_OBJLOCK(p_spncb));
	if (p_spncb->lock_flg) {
		/* すでにロックされていた場合 */
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		goto retry;
	}
	else {
		/* ロックされていなかった場合 */
		my_p_pcb = get_my_p_pcb();
		p_spncb->lock_flg = true;
		my_p_pcb->locspnid = spnid;
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		ercd = E_OK;
	}

  error_exit:
	LOG_LOC_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_loc_spn */

/*
 *  スピンロックの取得（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iloc_spn

ER
iloc_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;

	LOG_ILOC_SPN_ENTER(spnid);
	CHECK_INTCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	i_lock_cpu();
  retry:
	i_acquire_obj_lock(&GET_OBJLOCK(p_spncb));
	if (p_spncb->lock_flg) {
		/* すでにロックされていた場合 */
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		goto retry;
	}
	else {
		/* ロックされていなかった場合 */
		my_p_pcb = get_my_p_pcb();
		p_spncb->lock_flg = true;
		my_p_pcb->locspnid = spnid;
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		ercd = E_OK;
	}

  error_exit:
	LOG_ILOC_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iloc_spn */

/*
 *  スピンロックの取得（ポーリング）
 */
#ifdef TOPPERS_try_spn

ER
try_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;

	LOG_TRY_SPN_ENTER(spnid);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	t_lock_cpu();
	t_acquire_obj_lock(&GET_OBJLOCK(p_spncb));
	if (p_spncb->lock_flg) {
		/* すでにロックされていた場合 */
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		t_unlock_cpu();
		ercd = E_OBJ;
	}
	else {
		/* ロックされていなかった場合 */
		my_p_pcb = get_my_p_pcb();
		p_spncb->lock_flg = true;
		my_p_pcb->locspnid = spnid;
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		ercd = E_OK;
	}

  error_exit:
	LOG_TRY_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_try_spn */

/*
 *  スピンロックの取得（ポーリング, 非タスクコンテキスト用）
 */
#ifdef TOPPERS_itry_spn


ER
itry_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;

	LOG_ITRY_SPN_ENTER(spnid);
	CHECK_INTCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	i_lock_cpu();
	i_acquire_obj_lock(&GET_OBJLOCK(p_spncb));
	if (p_spncb->lock_flg) {
		/* すでにロックされていた場合 */
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		i_unlock_cpu();
		ercd = E_OBJ;
	}
	else {
		/* ロックされていなかった場合 */
		my_p_pcb = get_my_p_pcb();
		p_spncb->lock_flg = true;
		my_p_pcb->locspnid = spnid;
		release_obj_lock(&GET_OBJLOCK(p_spncb));
		ercd = E_OK;
	}

  error_exit:
	LOG_ITRY_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_itry_spn */

/*
 *  スピンロックの返却
 */
#ifdef TOPPERS_unl_spn

ER
unl_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;
	bool_t	locked;

	LOG_UNL_SPN_ENTER(spnid);
	CHECK_TSKCTX();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	/*
	 *  スピンロック解放は，1変数（p_spncb->lock_flg）をfalseにするだけな
	 *  ので，プロセッサ間排他制御は行わない． 
	 */
	locked = t_sense_lock();
	if (!locked) {
		t_lock_cpu();
	}
	my_p_pcb = get_my_p_pcb();
	if (my_p_pcb->locspnid != spnid) {
		if (!locked) {
			t_unlock_cpu();
		}
		ercd = E_ILUSE;
	}
	else {
		p_spncb->lock_flg = false;
		my_p_pcb->locspnid = 0;
		t_unlock_cpu();
		ercd = E_OK;
	}

  error_exit:
	LOG_UNL_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_unl_spn */

/*
 *  スピンロックの返却（非タスクコンテキスト用）
 */
#ifdef TOPPERS_iunl_spn

ER
iunl_spn(ID spnid)
{
	ER		ercd;
	SPNCB   *p_spncb;
	PCB		*my_p_pcb;
	bool_t	locked;

	LOG_IUNL_SPN_ENTER(spnid);
	CHECK_INTCTX();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	locked = i_sense_lock();
	if (!locked) {
		i_lock_cpu();
	}
	my_p_pcb = get_my_p_pcb();
	if (my_p_pcb->locspnid != spnid) {
		if (!locked) {
			i_unlock_cpu();
		}
		ercd = E_ILUSE;
	}
	else {
		p_spncb->lock_flg = false;
		my_p_pcb->locspnid = 0;
		i_unlock_cpu();
		ercd = E_OK;
	}

  error_exit:
	LOG_IUNL_SPN_LEAVE(ercd);
	return(ercd);
}

#endif /* TOPPERS_iunl_spn */

/*
 *  スピンロックの状態参照
 */
#ifdef TOPPERS_ref_spn

ER
ref_spn(ID spnid, T_RSPN *pk_rspn)
{
	ER		ercd;
	SPNCB   *p_spncb;

	LOG_REF_SPN_ENTER(spnid, pk_rspn);
	CHECK_TSKCTX_UNL();
	CHECK_SPNID(spnid);
	p_spncb = get_spncb(spnid);

	pk_rspn->spnstat = (p_spncb->lock_flg) ? TSPN_LOC : TSPN_UNL;
	ercd = E_OK;

  error_exit:
	LOG_REF_SPN_LEAVE(ercd, pk_rspn);
	return(ercd);
}

#endif /* TOPPERS_ref_spn */

#endif /* TTYPE_SPN == EMULATE_SPN */
