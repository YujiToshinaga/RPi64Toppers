$ ======================================================================
$ 
$   TOPPERS/FMP Kernel
$       Toyohashi Open Platform for Embedded Real-Time Systems/
$       Flexible MultiProcessor Kernel
$ 
$   Copyright (C) 2007 by TAKAGI Nobuhisa
$   Copyright (C) 2007-2015 by Embedded and Real-Time Systems Laboratory
$               Graduate School of Information Science, Nagoya Univ., JAPAN
$  
$   上記著作権者は，以下の(1)～(4)の条件を満たす場合に限り，本ソフトウェ
$   ア（本ソフトウェアを改変したものを含む．以下同じ）を使用・複製・改
$   変・再配布（以下，利用と呼ぶ）することを無償で許諾する．
$   (1) 本ソフトウェアをソースコードの形で利用する場合には，上記の著作
$       権表示，この利用条件および下記の無保証規定が，そのままの形でソー
$       スコード中に含まれていること．
$   (2) 本ソフトウェアを，ライブラリ形式など，他のソフトウェア開発に使
$       用できる形で再配布する場合には，再配布に伴うドキュメント（利用
$       者マニュアルなど）に，上記の著作権表示，この利用条件および下記
$       の無保証規定を掲載すること．
$   (3) 本ソフトウェアを，機器に組み込むなど，他のソフトウェア開発に使
$       用できない形で再配布する場合には，次のいずれかの条件を満たすこ
$       と．
$     (a) 再配布に伴うドキュメント（利用者マニュアルなど）に，上記の著
$         作権表示，この利用条件および下記の無保証規定を掲載すること．
$     (b) 再配布の形態を，別に定める方法によって，TOPPERSプロジェクトに
$         報告すること．
$   (4) 本ソフトウェアの利用により直接的または間接的に生じるいかなる損
$       害からも，上記著作権者およびTOPPERSプロジェクトを免責すること．
$       また，本ソフトウェアのユーザまたはエンドユーザからのいかなる理
$       由に基づく請求からも，上記著作権者およびTOPPERSプロジェクトを
$       免責すること．
$  
$   本ソフトウェアは，無保証で提供されているものである．上記著作権者お
$   よびTOPPERSプロジェクトは，本ソフトウェアに関して，特定の使用目的
$   に対する適合性も含めて，いかなる保証も行わない．また，本ソフトウェ
$   アの利用により直接的または間接的に生じたいかなる損害に関しても，そ
$   の責任を負わない．
$ 
$   @(#) $Id: kernel.tf 1087 2015-02-03 01:04:34Z ertl-honda $
$  
$ =====================================================================

$ =====================================================================
$ kernel_cfg.hの生成
$ =====================================================================

$FILE "kernel_cfg.h"$
/* kernel_cfg.h */$NL$
#ifndef TOPPERS_KERNEL_CFG_H$NL$
#define TOPPERS_KERNEL_CFG_H$NL$
#define TNUM_TSKID	$LENGTH(TSK.ID_LIST)$$NL$
#define TNUM_SEMID	$LENGTH(SEM.ID_LIST)$$NL$
#define TNUM_FLGID	$LENGTH(FLG.ID_LIST)$$NL$
#define TNUM_DTQID	$LENGTH(DTQ.ID_LIST)$$NL$
#define TNUM_PDQID	$LENGTH(PDQ.ID_LIST)$$NL$
#define TNUM_MBXID	$LENGTH(MBX.ID_LIST)$$NL$
#define TNUM_MPFID	$LENGTH(MPF.ID_LIST)$$NL$
#define TNUM_CYCID	$LENGTH(CYC.ID_LIST)$$NL$
#define TNUM_ALMID	$LENGTH(ALM.ID_LIST)$$NL$
#define TNUM_SPNID	$LENGTH(SPN.ID_LIST)$$NL$
$NL$
$FOREACH id TSK.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id SEM.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id FLG.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id DTQ.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id PDQ.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id MBX.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id MPF.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id CYC.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id ALM.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$FOREACH id SPN.ID_LIST$
	#define $id$	$+id$$NL$
$END$
$NL$
#endif /* TOPPERS_KERNEL_CFG_H */$NL$

$ =====================================================================
$ kernel_cfg.cの生成
$ =====================================================================

$FILE "kernel_cfg.c"$
/* kernel_cfg.c */$NL$
#include "kernel/kernel_int.h"$NL$
#include "kernel_cfg.h"$NL$
$NL$
#if TKERNEL_PRID != 0x08u$NL$
#error The kernel does not match this configuration file.$NL$
#endif$NL$
$NL$

$ 
$  インクルードディレクティブ（#include）
$ 
/*$NL$
$SPC$*  Include Directives (#include)$NL$
$SPC$*/$NL$
$NL$
$INCLUDES$
$NL$

$ 
$  トレースログマクロのデフォルト定義
$ 
/*$NL$
$SPC$*  Default Definitions of Trace Log Macros$NL$
$SPC$*/$NL$
$NL$
#ifndef LOG_ISR_ENTER$NL$
#define LOG_ISR_ENTER(intno)$NL$
#endif /* LOG_ISR_ENTER */$NL$
$NL$
#ifndef LOG_ISR_LEAVE$NL$
#define LOG_ISR_LEAVE(intno)$NL$
#endif /* LOG_ISR_LEAVE */$NL$
$NL$

$ 
$  オブジェクトのID番号を保持する変数
$ 
$IF USE_EXTERNAL_ID$
	/*$NL$
	$SPC$*  Variables for Object ID$NL$
	$SPC$*/$NL$
	$NL$
	$FOREACH id TSK.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id SEM.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id FLG.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id DTQ.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id PDQ.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id MBX.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id MPF.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id CYC.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
	$FOREACH id ALM.ID_LIST$
		const ID $id$_id$SPC$=$SPC$$+id$;$NL$
	$END$
$END$

$
$  スタック領域の確保関数
$
$IF !ISFUNCTION("ALLOC_STACK")$
$FUNCTION ALLOC_STACK$
$	// 大きい方に丸めたサイズで確保する
	static STK_T $ARGV[1]$[COUNT_STK_T($ARGV[2]$)];$NL$
	$RESULT = FORMAT("ROUND_STK_T(%1%)", ARGV[2])$
$END$
$END$

$ 
$  クラスIDのチェック
$ 
$FOREACH id CLS.ID_LIST$
 	$IF id > NUM_CLASS $
 		$ERROR$E_ID: $FORMAT(_("illegal %1% `%2%\'(=%3%) in %4%"), "clsid", id, +id, "CLASS()")$$END$
 	$END$
$END$

$ 
$  プロセッサ
$ 
/*$NL$
$SPC$*  Processor Management Functions$NL$
$SPC$*/$NL$
$NL$

$FOREACH prcid RANGE(1, TNUM_PRCID)$
	$IF ISFUNCTION("GENERATE_PCB")$
		$GENERATE_PCB(prcid)$
	$ELSE$
		PCB _kernel_prc$prcid$_pcb;$NL$
	$END$
$END$$NL$

PCB* const _kernel_p_pcb_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$&_kernel_prc$prcid$_pcb
$END$$NL$
};$NL$
$NL$

$ 
$  タスク
$ 
/*$NL$
$SPC$*  Task Management Functions$NL$
$SPC$*/$NL$
$NL$

$ タスクが1個以上存在することのチェック
$IF !LENGTH(TSK.ID_LIST)$
	$ERROR$$FORMAT(_("no task is registered"))$$END$
$END$

$ タスクID番号の最大値
const ID _kernel_tmax_tskid = (TMIN_TSKID + TNUM_TSKID - 1);$NL$
$NL$

$ エラーチェック
$FOREACH tskid TSK.ID_LIST$
$   //クラスの囲みに入れられているかのチェック
	$IF !LENGTH(TSK.CLASS[tskid])$
		$ERROR TSK.TEXT_LINE[tskid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), tskid, "CRE_TSK")$$END$
	$END$

$	// tskatrが（［TA_ACT］）でない場合（E_RSATR）
	$IF (TSK.TSKATR[tskid] & ~(TA_ACT|TARGET_TSKATR)) != 0$
		$ERROR TSK.TEXT_LINE[tskid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "tskatr", TSK.TSKATR[tskid], tskid, "CRE_TSK")$$END$
	$END$

$	// (TMIN_TPRI <= itskpri && itskpri <= TMAX_TPRI)でない場合（E_PAR）
	$IF !(TMIN_TPRI <= TSK.ITSKPRI[tskid] && TSK.ITSKPRI[tskid] <= TMAX_TPRI)$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "itskpri", TSK.ITSKPRI[tskid], tskid, "CRE_TSK")$$END$
	$END$

$ 	// texatrが（TA_NULL）でない場合（E_RSATR）
	$IF LENGTH(TSK.TEXATR[tskid]) && TSK.TEXATR[tskid] != 0$
		$ERROR DEF_TEX.TEXT_LINE[tskid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "texatr", TSK.TEXATR[tskid], tskid, "DEF_TEX")$$END$
	$END$
$END$

$ スタック領域の生成とそれに関するエラーチェック
$FOREACH tskid TSK.ID_LIST$
$	// stkszが0以下か，ターゲット定義の最小値（TARGET_MIN_STKSZ）よりも
$	// 小さい場合（E_PAR）
	$IF TSK.STKSZ[tskid] <= 0 || (TARGET_MIN_STKSZ
										 && TSK.STKSZ[tskid] < TARGET_MIN_STKSZ)$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("%1% `%2%\' of `%3%\' in %4% is too small"), "stksz", TSK.STKSZ[tskid], tskid, "CRE_TSK")$$END$
	$END$
$ 	// stkszがスタック領域のサイズとして正しくない場合（E_PAR）
	$IF !EQ(TSK.STK[tskid], "NULL") && CHECK_STKSZ_ALIGN
							&& (TSK.STKSZ[tskid] & (CHECK_STKSZ_ALIGN - 1))$
		$ERROR TSK.TEXT_LINE[tskid]$E_PAR: $FORMAT(_("%1% `%2%\' of `%3%\' in %4% is not aligned"), "stksz", TSK.STKSZ[tskid], tskid, "CRE_TSK")$$END$
	$END$

	$IF EQ(TSK.STK[tskid],"NULL")$
		$IF ISFUNCTION("GENERATE_TSKSTK")$
			$GENERATE_TSKSTK(tskid)$
			$TSK.TINIB_STKSZ[tskid] = FORMAT("ROUND_STK_T(%1%)", TSK.STKSZ[tskid])$
		$ELSE$
			$TSK.TINIB_STKSZ[tskid] = ALLOC_STACK(CONCAT("_kernel_stack_",
												tskid), TSK.STKSZ[tskid])$
		$END$
		$TSK.TINIB_STK[tskid] = CONCAT("_kernel_stack_", tskid)$
	$ELSE$
		$TSK.TINIB_STKSZ[tskid] = FORMAT("(%1%)", TSK.STKSZ[tskid])$
		$TSK.TINIB_STK[tskid] = FORMAT("(void *)(%1%)", TSK.STK[tskid])$
	$END$
$END$
$NL$

$ タスク初期化ブロックの生成（タスクは1個以上存在する）
const TINIB _kernel_tinib_table[TNUM_TSKID] = {$NL$
$JOINEACH tskid TSK.ID_LIST ",\n"$
$	// タスク属性，拡張情報，起動番地，起動時優先度
	$TAB${
	$SPC$($TSK.TSKATR[tskid]$), (intptr_t)($TSK.EXINF[tskid]$),
	$SPC$((TASK)($TSK.TASK[tskid]$)), INT_PRIORITY($TSK.ITSKPRI[tskid]$),

$	// タスク初期化コンテキストブロック，スタック領域
	$IF USE_TSKINICTXB$
		$GENERATE_TSKINICTXB(tskid)$
	$ELSE$
		$SPC$$TSK.TINIB_STKSZ[tskid]$, $TSK.TINIB_STK[tskid]$,
	$END$

$	// タスク例外処理ルーチンの属性と起動番地，初期割付プロセッサと割り付け可能プロセッサ
	$SPC$($ALT(TSK.TEXATR[tskid],"TA_NULL")$), ($ALT(TSK.TEXRTN[tskid],"NULL")$), $CLASS_AFFINITY_INI[TSK.CLASS[tskid]]$, (uint_t)($CLASS_AFFINITY_MASK[TSK.CLASS[tskid]]$) }
$END$$NL$
};$NL$
$NL$

$ タスク管理ブロックの生成
$FOREACH tskid TSK.ID_LIST$
	$IF ISFUNCTION("GENERATE_TCB")$
		$GENERATE_TCB(tskid)$
	$ELSE$
		TCB _kernel_tcb_$tskid$;$NL$
	$END$
$END$$NL$
$NL$


$ タスク管理ブロックアクセステーブルの生成
TCB* const _kernel_p_tcb_table[TNUM_TSKID] ={$NL$
$JOINEACH tskid TSK.ID_LIST ",\n"$
	$TAB$&_kernel_tcb_$tskid$
$END$$NL$
};$NL$
$NL$

$ タスク生成順序テーブルの生成
const ID _kernel_torder_table[TNUM_TSKID] = {$NL$
$TAB$$JOINEACH tskid TSK.ORDER_LIST ", "$$tskid$$END$$NL$
};$NL$
$NL$

$ 
$  セマフォ
$ 
/*$NL$
$SPC$*  Semaphore Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH semid SEM.ID_LIST$
	$IF !LENGTH(SEM.CLASS[semid])$
		$ERROR SEM.TEXT_LINE[semid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), semid, "CRE_SEM")$$END$
	$END$
$END$

$ セマフォID番号の最大値
const ID _kernel_tmax_semid = (TMIN_SEMID + TNUM_SEMID - 1);$NL$
$NL$

$ セマフォ初期化ブロックの生成
$IF LENGTH(SEM.ID_LIST)$
	const SEMINIB _kernel_seminib_table[TNUM_SEMID] = {$NL$
	$JOINEACH semid SEM.ID_LIST ",\n"$
$		// sematrが（［TA_TPRI］）でない場合（E_RSATR）
		$IF (SEM.SEMATR[semid] & ~TA_TPRI) != 0$
			$ERROR SEM.TEXT_LINE[semid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "sematr", SEM.SEMATR[semid], semid, "CRE_SEM")$$END$
		$END$

$		// (0 <= isemcnt && isemcnt <= maxsem)でない場合（E_PAR）
		$IF !(0 <= SEM.ISEMCNT[semid] && SEM.ISEMCNT[semid] <= SEM.MAXSEM[semid])$
			$ERROR SEM.TEXT_LINE[semid]$E_PAR: $FORMAT(_("too large %1% `%2%\' of `%3%\' in %4%"), "isemcnt", SEM.ISEMCNT[semid], semid, "CRE_SEM")$$END$
		$END$

$		// (1 <= maxsem && maxsem <= TMAX_MAXSEM)でない場合（E_PAR）
		$IF !(1 <= SEM.MAXSEM[semid] && SEM.MAXSEM[semid] <= TMAX_MAXSEM)$
			$ERROR SEM.TEXT_LINE[semid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxsem", SEM.MAXSEM[semid], semid, "CRE_SEM")$$END$
		$END$

$		// セマフォ初期化ブロック
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($SEM.SEMATR[semid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[SEM.CLASS[semid]]$_pcb.obj_lock)), ($SEM.ISEMCNT[semid]$), ($SEM.MAXSEM[semid]$) }
		$ELSE$
			$TAB${ ($SEM.SEMATR[semid]$), ($SEM.ISEMCNT[semid]$), ($SEM.MAXSEM[semid]$) }
		$END$

	$END$$NL$
	};$NL$
	$NL$

$	// セマフォ管理ブロック
	$FOREACH semid SEM.ID_LIST$
		$IF ISFUNCTION("GENERATE_SEMCB")$
			$GENERATE_SEMCB(semid)$
		$ELSE$
			SEMCB _kernel_semcb_$semid$;$NL$
		$END$
	$END$$NL$

$	// セマフォ管理ブロックアクセステーブル
	SEMCB* const _kernel_p_semcb_table[TNUM_SEMID] = {$NL$
	$JOINEACH semid SEM.ID_LIST ",\n"$
		$TAB$&_kernel_semcb_$semid$
	$END$$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const SEMINIB, _kernel_seminib_table);$NL$
	TOPPERS_EMPTY_LABEL(SEMCB* const, _kernel_p_semcb_table);$NL$
$END$
$NL$

$ 
$  イベントフラグ
$ 
/*$NL$
$SPC$*  Eventflag Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH flgid FLG.ID_LIST$
	$IF !LENGTH(FLG.CLASS[flgid])$
		$ERROR FLG.TEXT_LINE[flgid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), flgid, "CRE_FLG")$$END$
	$END$
$END$

$ イベントフラグID番号の最大値
const ID _kernel_tmax_flgid = (TMIN_FLGID + TNUM_FLGID - 1);$NL$
$NL$

$ イベントフラグ初期化ブロックの生成
$IF LENGTH(FLG.ID_LIST)$
	const FLGINIB _kernel_flginib_table[TNUM_FLGID] = {$NL$
	$JOINEACH flgid FLG.ID_LIST ",\n"$
$		// flgatrが（［TA_TPRI］｜［TA_WMUL］｜［TA_CLR］）でない場合（E_RSATR）
		$IF (FLG.FLGATR[flgid] & ~(TA_TPRI|TA_WMUL|TA_CLR)) != 0$
			$ERROR FLG.TEXT_LINE[flgid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "flgatr", FLG.FLGATR[flgid], flgid, "CRE_FLG")$$END$
		$END$

$		// iflgptnがFLGPTNに格納できない場合（E_PAR）
		$IF (FLG.IFLGPTN[flgid] & ~((1 << TBIT_FLGPTN) - 1)) != 0$
			$ERROR FLG.TEXT_LINE[flgid]$E_PAR: $FORMAT(_("too large %1% `%2%\' of `%3%\' in %4%"), "iflgptn", FLG.IFLGPTN[flgid], flgid, "CRE_FLG")$$END$
		$END$

$		// イベントフラグ初期化ブロック
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($FLG.FLGATR[flgid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[FLG.CLASS[flgid]]$_pcb.obj_lock)), ($FLG.IFLGPTN[flgid]$) }
		$ELSE$
			$TAB${ ($FLG.FLGATR[flgid]$), ($FLG.IFLGPTN[flgid]$) }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// イベントフラグ管理ブロック
	$FOREACH flgid FLG.ID_LIST$
		$IF ISFUNCTION("GENERATE_FLGCB")$
			$GENERATE_FLGCB(flgid)$
		$ELSE$
			FLGCB _kernel_flgcb_$flgid$;$NL$
		$END$
	$END$$NL$

$	// イベントフラグ管理ブロックアクセステーブル
	FLGCB* const _kernel_p_flgcb_table[TNUM_FLGID] = {$NL$
	$JOINEACH flgid FLG.ID_LIST ",\n"$
		$TAB$&_kernel_flgcb_$flgid$
	$END$$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const FLGINIB, _kernel_flginib_table);$NL$
	TOPPERS_EMPTY_LABEL(FLGCB* const, _kernel_p_flgcb_table);$NL$
$END$
$NL$

$ 
$  データキュー
$ 
/*$NL$
$SPC$*  Dataqueue Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH dtqid DTQ.ID_LIST$
	$IF !LENGTH(DTQ.CLASS[dtqid])$
		$ERROR DTQ.TEXT_LINE[dtqid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), dtqid, "CRE_DTQ")$$END$
	$END$
$END$

$ データキューID番号の最大値
const ID _kernel_tmax_dtqid = (TMIN_DTQID + TNUM_DTQID - 1);$NL$
$NL$

$IF LENGTH(DTQ.ID_LIST)$
	$FOREACH dtqid DTQ.ID_LIST$
$		// dtqatrが（［TA_TPRI］）でない場合（E_RSATR）
		$IF (DTQ.DTQATR[dtqid] & ~TA_TPRI) != 0$
			$ERROR DTQ.TEXT_LINE[dtqid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "dtqatr", DTQ.DTQATR[dtqid], dtqid, "CRE_DTQ")$$END$
		$END$

$		// dtqcntが負の場合（E_PAR）
		$IF DTQ.DTQCNT[dtqid] < 0$
			$ERROR DTQ.TEXT_LINE[dtqid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "dtqcnt", DTQ.DTQCNT[dtqid], dtqid, "CRE_DTQ")$$END$
		$END$

$		// dtqmbがNULLでない場合（E_NOSPT）
		$IF !EQ(DTQ.DTQMB[dtqid], "NULL")$
			$ERROR DTQ.TEXT_LINE[dtqid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "dtqmb", DTQ.DTQMB[dtqid], dtqid, "CRE_DTQ")$$END$
		$END$

$		// データキュー管理領域
		$IF DTQ.DTQCNT[dtqid]$
			$IF ISFUNCTION("GENERATE_DTQMB")$
				$GENERATE_DTQMB(dtqid)$
			$ELSE$
				static DTQMB _kernel_dtqmb_$dtqid$[$DTQ.DTQCNT[dtqid]$];$NL$
			$END$
		$END$
	$END$
$NL$
$	// データキュー初期化ブロックの生成
	const DTQINIB _kernel_dtqinib_table[TNUM_DTQID] = {$NL$
	$JOINEACH dtqid DTQ.ID_LIST ",\n"$
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($DTQ.DTQATR[dtqid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[DTQ.CLASS[dtqid]]$_pcb.obj_lock)), ($DTQ.DTQCNT[dtqid]$), $IF DTQ.DTQCNT[dtqid]$(_kernel_dtqmb_$dtqid$)$ELSE$NULL$END$ }
		$ELSE$
			$TAB${ ($DTQ.DTQATR[dtqid]$), ($DTQ.DTQCNT[dtqid]$), $IF DTQ.DTQCNT[dtqid]$(_kernel_dtqmb_$dtqid$)$ELSE$NULL$END$ }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// データキュー管理ブロック
	$FOREACH dtqid DTQ.ID_LIST$
		$IF ISFUNCTION("GENERATE_DTQCB")$
			$GENERATE_DTQCB(dtqid)$
		$ELSE$
			DTQCB _kernel_dtqcb_$dtqid$;$NL$
		$END$
	$END$$NL$

$	// データーキュー管理ブロックアクセステーブル
	DTQCB* const _kernel_p_dtqcb_table[TNUM_DTQID] = {$NL$
	$JOINEACH dtqid DTQ.ID_LIST ",\n"$
		$TAB$&_kernel_dtqcb_$dtqid$
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const DTQINIB, _kernel_dtqinib_table);$NL$
	TOPPERS_EMPTY_LABEL(DTQCB* const, _kernel_p_dtqcb_table);$NL$
$END$
$NL$

$ 
$  優先度データキュー
$ 
/*$NL$
$SPC$*  Priority Dataqueue Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH pdqid PDQ.ID_LIST$
	$IF !LENGTH(PDQ.CLASS[pdqid])$
		$ERROR PDQ.TEXT_LINE[pdqid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), pdqid, "CRE_PDQ")$$END$
	$END$
$END$

$ 優先度データキューID番号の最大値
const ID _kernel_tmax_pdqid = (TMIN_PDQID + TNUM_PDQID - 1);$NL$
$NL$

$IF LENGTH(PDQ.ID_LIST)$
	$FOREACH pdqid PDQ.ID_LIST$
$		// pdqatrが（［TA_TPRI］）でない場合（E_RSATR）
		$IF (PDQ.PDQATR[pdqid] & ~TA_TPRI) != 0$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "pdqatr", PDQ.PDQATR[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// pdqcntが負の場合（E_PAR）
		$IF PDQ.PDQCNT[pdqid] < 0$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "pdqcnt", PDQ.PDQCNT[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// (TMIN_DPRI <= maxdpri && maxdpri <= TMAX_DPRI)でない場合（E_PAR）
		$IF !(TMIN_DPRI <= PDQ.MAXDPRI[pdqid] && PDQ.MAXDPRI[pdqid] <= TMAX_DPRI)$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxdpri", PDQ.MAXDPRI[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// pdqmbがNULLでない場合（E_NOSPT）
		$IF !EQ(PDQ.PDQMB[pdqid], "NULL")$
			$ERROR PDQ.TEXT_LINE[pdqid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "pdqmb", PDQ.PDQMB[pdqid], pdqid, "CRE_PDQ")$$END$
		$END$

$		// 優先度データキュー管理領域
		$IF PDQ.PDQCNT[pdqid]$
			$IF ISFUNCTION("GENERATE_PDQMB")$
				$GENERATE_PDQMB(pdqid)$
			$ELSE$
				static PDQMB _kernel_pdqmb_$pdqid$[$PDQ.PDQCNT[pdqid]$];$NL$
			$END$
		$END$
	$END$
$NL$
$	// 優先度データキュー初期化ブロックの生成
	const PDQINIB _kernel_pdqinib_table[TNUM_PDQID] = {$NL$
	$JOINEACH pdqid PDQ.ID_LIST ",\n"$
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($PDQ.PDQATR[pdqid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[PDQ.CLASS[pdqid]]$_pcb.obj_lock)), ($PDQ.PDQCNT[pdqid]$), ($PDQ.MAXDPRI[pdqid]$), $IF PDQ.PDQCNT[pdqid]$(_kernel_pdqmb_$pdqid$)$ELSE$NULL$END$ }
		$ELSE$
			$TAB${ ($PDQ.PDQATR[pdqid]$), ($PDQ.PDQCNT[pdqid]$), ($PDQ.MAXDPRI[pdqid]$), $IF PDQ.PDQCNT[pdqid]$(_kernel_pdqmb_$pdqid$)$ELSE$NULL$END$ }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// 優先度データキュー管理ブロック
	$FOREACH pdqid PDQ.ID_LIST$
		$IF ISFUNCTION("GENERATE_PDQCB")$
			$GENERATE_PDQCB(pdqid)$
		$ELSE$
			PDQCB _kernel_pdqcb_$pdqid$;$NL$
		$END$
	$END$$NL$

$	// 優先度データーキュー管理ブロックアクセステーブル
	PDQCB* const _kernel_p_pdqcb_table[TNUM_PDQID] = {$NL$
	$JOINEACH pdqid PDQ.ID_LIST ",\n"$
		$TAB$&_kernel_pdqcb_$pdqid$
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const PDQINIB, _kernel_pdqinib_table);$NL$
	TOPPERS_EMPTY_LABEL(PDQCB* const, _kernel_p_pdqcb_table);$NL$
$END$
$NL$

$ 
$  メールボックス
$ 
/*$NL$
$SPC$*  Mailbox Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH mbxid MBX.ID_LIST$
	$IF !LENGTH(MBX.CLASS[mbxid])$
		$ERROR MBX.TEXT_LINE[mbxid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), mbxid, "CRE_MBX")$$END$
	$END$
$END$

$ メールボックスID番号の最大値
const ID _kernel_tmax_mbxid = (TMIN_MBXID + TNUM_MBXID - 1);$NL$
$NL$

$ メールボックス初期化ブロックの生成
$IF LENGTH(MBX.ID_LIST)$
	const MBXINIB _kernel_mbxinib_table[TNUM_MBXID] = {$NL$
	$JOINEACH mbxid MBX.ID_LIST ",\n"$
$		// mbxatrが（［TA_TPRI］｜［TA_MPRI］）でない場合（E_RSATR）
		$IF (MBX.MBXATR[mbxid] & ~(TA_TPRI|TA_MPRI)) != 0$
			$ERROR MBX.TEXT_LINE[mbxid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mbxatr", MBX.MBXATR[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// (TMIN_MPRI <= maxmpri && maxmpri <= TMAX_MPRI)でない場合（E_PAR）
		$IF !(TMIN_MPRI <= MBX.MAXMPRI[mbxid] && MBX.MAXMPRI[mbxid] <= TMAX_MPRI)$
			$ERROR MBX.TEXT_LINE[mbxid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "maxmpri", MBX.MAXMPRI[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// mprihdがNULLでない場合（E_NOSPT）
		$IF !EQ(MBX.MPRIHD[mbxid], "NULL")$
			$ERROR MBX.TEXT_LINE[mbxid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mprihd", MBX.MPRIHD[mbxid], mbxid, "CRE_MBX")$$END$
		$END$

$		// メールボックス初期化ブロック
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($MBX.MBXATR[mbxid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[MBX.CLASS[mbxid]]$_pcb.obj_lock)), ($MBX.MAXMPRI[mbxid]$) }
		$ELSE$
			$TAB${ ($MBX.MBXATR[mbxid]$), ($MBX.MAXMPRI[mbxid]$) }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// メールボックス管理ブロック
	$FOREACH mbxid MBX.ID_LIST$
		$IF ISFUNCTION("GENERATE_MBXCB")$
			$GENERATE_MBXCB(mbxid)$
		$ELSE$
			MBXCB _kernel_mbxcb_$mbxid$;$NL$
		$END$
	$END$$NL$

$	// メールボックス管理ブロックアクセステーブル
	MBXCB* const _kernel_p_mbxcb_table[TNUM_MBXID] = {$NL$
	$JOINEACH mbxid MBX.ID_LIST ",\n"$
		$TAB$&_kernel_mbxcb_$mbxid$
	$END$
	$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const MBXINIB, _kernel_mbxinib_table);$NL$
	TOPPERS_EMPTY_LABEL(MBXCB* const, _kernel_p_mbxcb_table);$NL$
$END$
$NL$

$ 
$  固定長メモリプール
$ 
/*$NL$
$SPC$*  Fixed-sized Memorypool Functions$NL$
$SPC$*/$NL$
$NL$

$ クラスの囲みに入れられているかのチェック
$FOREACH mpfid MPF.ID_LIST$
	$IF !LENGTH(MPF.CLASS[mpfid])$
		$ERROR MPF.TEXT_LINE[mpfid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), mpfid, "CRE_MPF")$$END$
	$END$
$END$

$ 固定長メモリプールID番号の最大値
const ID _kernel_tmax_mpfid = (TMIN_MPFID + TNUM_MPFID - 1);$NL$
$NL$

$IF LENGTH(MPF.ID_LIST)$
	$FOREACH mpfid MPF.ID_LIST$
$		// mpfatrが（［TA_TPRI］）でない場合（E_RSATR）
		$IF (MPF.MPFATR[mpfid] & ~TA_TPRI) != 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mpfatr", MPF.MPFATR[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// blkcntが0以下の場合（E_PAR）
		$IF MPF.BLKCNT[mpfid] <= 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "blkcnt", MPF.BLKCNT[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// blkszが0以下の場合（E_PAR）
		$IF MPF.BLKSZ[mpfid] <= 0$
			$ERROR MPF.TEXT_LINE[mpfid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "blksz", MPF.BLKSZ[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// 固定長メモリプール領域
		$IF EQ(MPF.MPF[mpfid], "NULL")$
			$IF ISFUNCTION("GENERATE_MPF_T")$
				$GENERATE_MPF_T(mpfid)$
			$ELSE$
				static MPF_T _kernel_mpf_$mpfid$[($MPF.BLKCNT[mpfid]$) * COUNT_MPF_T($MPF.BLKSZ[mpfid]$)];$NL$
			$END$
		$END$

$		// mpfmbがNULLでない場合（E_NOSPT）
		$IF !EQ(MPF.MPFMB[mpfid], "NULL")$
			$ERROR MPF.TEXT_LINE[mpfid]$E_NOSPT: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "mpfmb", MPF.MPFMB[mpfid], mpfid, "CRE_MPF")$$END$
		$END$

$		// 固定長メモリプール管理領域
		$IF ISFUNCTION("GENERATE_MPFMB")$
			$GENERATE_MPFMB(mpfid)$
		$ELSE$
			static MPFMB _kernel_mpfmb_$mpfid$[$MPF.BLKCNT[mpfid]$];$NL$
		$END$
	$END$

$	// 固定長メモリプール初期化ブロックの生成
	const MPFINIB _kernel_mpfinib_table[TNUM_MPFID] = {$NL$
	$JOINEACH mpfid MPF.ID_LIST ",\n"$
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$TAB${ ($MPF.MPFATR[mpfid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[MPF.CLASS[mpfid]]$_pcb.obj_lock)), ($MPF.BLKCNT[mpfid]$), ROUND_MPF_T($MPF.BLKSZ[mpfid]$), $IF EQ(MPF.MPF[mpfid],"NULL")$(_kernel_mpf_$mpfid$)$ELSE$(void *)($MPF.MPF[mpfid]$)$END$, (_kernel_mpfmb_$mpfid$) }
		$ELSE$
			$TAB${ ($MPF.MPFATR[mpfid]$), ($MPF.BLKCNT[mpfid]$), ROUND_MPF_T($MPF.BLKSZ[mpfid]$), $IF EQ(MPF.MPF[mpfid],"NULL")$(_kernel_mpf_$mpfid$)$ELSE$(void *)($MPF.MPF[mpfid]$)$END$, (_kernel_mpfmb_$mpfid$) }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	//  固定長メモリプール管理ブロック
	$FOREACH mpfid MPF.ID_LIST$
		$IF ISFUNCTION("GENERATE_MPFCB")$
			$GENERATE_MPFCB(mpfid)$
		$ELSE$
			MPFCB _kernel_mpfcb_$mpfid$;$NL$
		$END$
	$END$$NL$

$	//  固定長メモリプール管理ブロックアクセステーブル
	MPFCB* const _kernel_p_mpfcb_table[TNUM_MPFID] = {$NL$
	$JOINEACH mpfid MPF.ID_LIST ",\n"$
		$TAB$&_kernel_mpfcb_$mpfid$
	$END$
	$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const MPFINIB, _kernel_mpfinib_table);$NL$
	TOPPERS_EMPTY_LABEL(MPFCB* const, _kernel_p_mpfcb_table);$NL$
$END$
$NL$

$ 
$  周期ハンドラ
$ 
/*$NL$
$SPC$*  Cyclic Handler Functions$NL$
$SPC$*/$NL$
$NL$

$ クラス関連のチェック
$FOREACH cycid CYC.ID_LIST$
$	// クラスの囲みに入れられているかのチェック
	$IF !LENGTH(CYC.CLASS[cycid])$
		$ERROR CYC.TEXT_LINE[cycid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), cycid, "CRE_CYC")$$END$
	$END$

$	// グローバルタイマ方式の場合のチェック
	$IF TOPPERS_SYSTIM_GLOBAL$
$		// 初期割付けプロセッサがシステム時刻管理プロセッサ以外ならエラー
		$IF CLASS_AFFINITY_INI[CYC.CLASS[cycid]] != TOPPERS_SYSTIM_PRCID$
			$ERROR CYC.TEXT_LINE[cycid]$E_RSATR: $FORMAT(_("illegal class id(initial affinity) of `%1%\' in %2%"), cycid, "CRE_CYC")$$END$
		$END$
$		// 割付け可能プロセッサにをシステム時刻管理プロセッサ以外が含まれていればエラー
		$IF (CLASS_AFFINITY_MASK[CYC.CLASS[cycid]] & ~(1 << (TOPPERS_SYSTIM_PRCID - 1))) != 0$
			$ERROR CYC.TEXT_LINE[cycid]$E_RSATR: $FORMAT(_("illegal class id(affinity) of `%1%\' in %2%"), cycid, "CRE_CYC")$$END$
		$END$
	$END$

$END$

$ 周期ハンドラID番号の最大値
const ID _kernel_tmax_cycid = (TMIN_CYCID + TNUM_CYCID - 1);$NL$
$NL$

$ 周期ハンドラ初期化テーブルの生成
$IF LENGTH(CYC.ID_LIST)$
	const CYCINIB _kernel_cycinib_table[TNUM_CYCID] = {$NL$
	$JOINEACH cycid CYC.ID_LIST ",\n"$
$		// cycatrが（［TA_STA］）でない場合（E_RSATR）
		$IF (CYC.CYCATR[cycid] & ~TA_STA) != 0$
			$ERROR CYC.TEXT_LINE[cycid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cycatr", CYC.CYCATR[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// (0 < cyctim && cyctim <= TMAX_RELTIM)でない場合（E_PAR）
		$IF !(0 < CYC.CYCTIM[cycid] && CYC.CYCTIM[cycid] <= TMAX_RELTIM)$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cyctim", CYC.CYCTIM[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// (0 <= cycphs && cycphs <= TMAX_RELTIM)でない場合（E_PAR）
		$IF !(0 <= CYC.CYCPHS[cycid] && CYC.CYCPHS[cycid] <= TMAX_RELTIM)$
			$ERROR CYC.TEXT_LINE[cycid]$E_PAR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "cycphs", CYC.CYCPHS[cycid], cycid, "CRE_CYC")$$END$
		$END$

$		// 警告：cycatrにTA_STAが設定されていて，(cycphs == 0)の場合
		$IF (CYC.CYCATR[cycid] & TA_STA) != 0 && CYC.CYCPHS[cycid] == 0$
			$WARNING CYC.TEXT_LINE[cycid]$$FORMAT(_("%1% is not recommended when %2% is set to %3% in %4%"), "cycphs==0", "TA_STA", "cycatr", "CRE_CYC")$$END$
		$END$

$		// 周期ハンドラ初期化ブロック
		$IF TOPPERS_SYSTIM_LOCAL$
			$TAB${ ($CYC.CYCATR[cycid]$), (intptr_t)($CYC.EXINF[cycid]$), ($CYC.CYCHDR[cycid]$), ($CYC.CYCTIM[cycid]$), ($CYC.CYCPHS[cycid]$), $CLASS_AFFINITY_INI[CYC.CLASS[cycid]]$, (uint_t)($CLASS_AFFINITY_MASK[CYC.CLASS[cycid]]$) }
		$ELSE$
			$TAB${ ($CYC.CYCATR[cycid]$), (intptr_t)($CYC.EXINF[cycid]$), ($CYC.CYCHDR[cycid]$), ($CYC.CYCTIM[cycid]$), ($CYC.CYCPHS[cycid]$) }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// 周期ハンドラ管理ブロック
	$FOREACH cycid CYC.ID_LIST$
		$IF ISFUNCTION("GENERATE_CYCCB")$
			$GENERATE_CYCCB(cycid)$
		$ELSE$
			CYCCB _kernel_cyccb_$cycid$;$NL$
		$END$
	$END$$NL$

$	// 周期ハンドラ管理ブロックアクセステーブル
	CYCCB* const _kernel_p_cyccb_table[TNUM_CYCID] = {$NL$
	$JOINEACH cycid CYC.ID_LIST ",\n"$
		$TAB$&_kernel_cyccb_$cycid$
	$END$
	$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const CYCINIB, _kernel_cycinib_table);$NL$
	TOPPERS_EMPTY_LABEL(CYCCB* const, _kernel_p_cyccb_table);$NL$
$END$
$NL$

$ 
$  アラームハンドラ
$ 
/*$NL$
$SPC$*  Alarm Handler Functions$NL$
$SPC$*/$NL$
$NL$

$ クラス関連のチェック
$FOREACH almid ALM.ID_LIST$
$	// クラスの囲みに入れられているかのチェック
	$IF !LENGTH(ALM.CLASS[almid])$
		$ERROR ALM.TEXT_LINE[almid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), almid, "CRE_ALM")$$END$
	$END$

$	// グローバルタイマ方式の場合のチェック
	$IF TOPPERS_SYSTIM_GLOBAL$
$		// 初期割付けプロセッサがシステム時刻管理プロセッサ以外ならエラー
		$IF CLASS_AFFINITY_INI[ALM.CLASS[almid]] != TOPPERS_SYSTIM_PRCID$
			$ERROR ALM.TEXT_LINE[almid]$E_RSATR: $FORMAT(_("illegal class id(initial affinity) of `%1%\' in %2%"), almid, "CRE_ALM")$$END$
		$END$
$		// 割付け可能プロセッサにをシステム時刻管理プロセッサ以外が含まれていればエラー
		$IF (CLASS_AFFINITY_MASK[ALM.CLASS[almid]] & ~(1 << (TOPPERS_SYSTIM_PRCID - 1))) != 0$
			$ERROR ALM.TEXT_LINE[almid]$E_RSATR: $FORMAT(_("illegal class id(affinity) of `%1%\' in %2%"), almid, "CRE_ALM")$$END$
		$END$
	$END$

$END$

$ アラームハンドラID番号の最大値
const ID _kernel_tmax_almid = (TMIN_ALMID + TNUM_ALMID - 1);$NL$
$NL$

$ アラームハンドラ初期化ブロックの生成
$IF LENGTH(ALM.ID_LIST)$
	const ALMINIB _kernel_alminib_table[TNUM_ALMID] = {$NL$
	$JOINEACH almid ALM.ID_LIST ",\n"$
$		// almatrが（TA_NULL）でない場合（E_RSATR）
		$IF ALM.ALMATR[almid] != 0$
			$ERROR ALM.TEXT_LINE[almid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "almatr", ALM.ALMATR[almid], almid, "CRE_ALM")$$END$
		$END$

$		// アラームハンドラ初期化ブロック
		$IF TOPPERS_SYSTIM_LOCAL$
			$TAB${ ($ALM.ALMATR[almid]$), (intptr_t)($ALM.EXINF[almid]$), ($ALM.ALMHDR[almid]$), $CLASS_AFFINITY_INI[ALM.CLASS[almid]]$, (uint_t)($CLASS_AFFINITY_MASK[ALM.CLASS[almid]]$) }
		$ELSE$
			$TAB${ ($ALM.ALMATR[almid]$), (intptr_t)($ALM.EXINF[almid]$), ($ALM.ALMHDR[almid]$) }
		$END$
	$END$$NL$
	};$NL$
	$NL$

$	// アラームハンドラ管理ブロック
	$FOREACH almid ALM.ID_LIST$
		$IF ISFUNCTION("GENERATE_ALMCB")$
			$GENERATE_ALMCB(almid)$
		$ELSE$
			ALMCB _kernel_almcb_$almid$;$NL$
		$END$
	$END$$NL$

$	// アラームハンドラ管理ブロックアクセステーブル
	ALMCB* const _kernel_p_almcb_table[TNUM_ALMID] = {$NL$
	$JOINEACH almid ALM.ID_LIST ",\n"$
		$TAB$&_kernel_almcb_$almid$
	$END$
	$NL$
	};$NL$

$ELSE$
	TOPPERS_EMPTY_LABEL(const ALMINIB, _kernel_alminib_table);$NL$
	TOPPERS_EMPTY_LABEL(ALMCB* const, _kernel_p_almcb_table);$NL$
$END$
$NL$

$ 
$  スピンロック
$ 
/*$NL$
$SPC$*  Spin Lock Functions$NL$
$SPC$*/$NL$
$NL$

$ //クラスの囲みに入れられているかのチェック
$FOREACH spnid SPN.ID_LIST$
	$IF !LENGTH(SPN.CLASS[spnid])$
		$ERROR SPN.TEXT_LINE[spnid]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), spnid, "CRE_SPN")$$END$
	$END$
$END$

$ //スピンロック属性にTA_NULL以外が指定された場合（E_RSATR）
$FOREACH spnid SPN.ID_LIST$
	$IF SPN.SPNATR[spnid] != 0$
		$ERROR SPN.TEXT_LINE[spnid]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of `%3%\' in %4%"), "spnatr", SPN.SPNATR[spnid], spnid, "CRE_SPN")$$END$
	$END$
$END$
$ スピンロックID番号の最大値
const ID _kernel_tmax_spnid = (TMIN_SPNID + TNUM_SPNID - 1);$NL$
$NL$


$IF LENGTH(SPN.ID_LIST)$

$       //ネイティブロック方式の場合のチェック
	$IF EQ(+TTYPE_SPN, +NATIVE_SPN)$
		#if TTYPE_SPN != NATIVE_SPN$NL$
		#error USE_NATIVE_SPIN_LOCK is not defined.$NL$
		#endif /* TTYPE_SPN != NATIVE_SPN */$NL$
		$NL$
		$IF (LENGTH(SPN.ID_LIST) > TMAX_NATIVE_SPN)$
			$ERROR SPN.TEXT_LINE[TMAX_NATIVE_SPN + 1]$E_NORES: $FORMAT(_("maximum number of spin lock is %1%."), TMAX_NATIVE_SPN)$$END$
		$END$
	$END$

$   // スピンロック初期化ブロック
	const SPNINIB _kernel_spninib_table[TNUM_SPNID] = {$NL$
	$IF EQ(+TTYPE_SPN, +NATIVE_SPN)$
		$JOINEACH spnid SPN.ID_LIST ",\n"$
			$TAB${($SPN.SPNATR[spnid]$)}
		$END$$NL$
	$ELSE$
		$IF EQ(+TTYPE_KLOCK, +P_KLOCK)$
			$JOINEACH spnid SPN.ID_LIST ",\n"$
				$TAB${($SPN.SPNATR[spnid]$), (&(_kernel_prc$CLASS_OBJ_LOCK[SPN.CLASS[spnid]]$_pcb.obj_lock))}
			$END$$NL$
		$ELSE$
			$JOINEACH spnid SPN.ID_LIST ",\n"$
				$TAB${($SPN.SPNATR[spnid]$)}
			$END$$NL$
		$END$
	$END$
	};$NL$
	$NL$

$   // スピンロックコントロールブロック
	$FOREACH spnid SPN.ID_LIST$
		$IF ISFUNCTION("GENERATE_SPNCB")$
			$GENERATE_SPNCB(spnid)$
		$ELSE$
			SPNCB _kernel_spncb_$spnid$;$NL$
		$END$
	$END$
	$NL$

$	// スピンロックコントロールブロックアクセステーブル
	SPNCB* const _kernel_p_spncb_table[TNUM_SPNID] = {$NL$
	$JOINEACH spnid SPN.ID_LIST ",\n"$
		$TAB$&_kernel_spncb_$spnid$
	$END$
	$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const SPNINIB, _kernel_spninib_table);$NL$
	TOPPERS_EMPTY_LABEL(SPNCB* const, _kernel_p_spncb_table);$NL$
$END$
$NL$

$ 
$  割込み管理機能
$ 
/*$NL$
$SPC$*  Interrupt Management Functions$NL$
$SPC$*/$NL$
$NL$

$ 割込み番号中のCPU指定を取得するためのシフト量
$intno_cpu_shift = sizeof_INTNO * 4$
$ 割込みハンドラ番号中のCPU指定のアンマスク値
$intno_cpu_umask = ((0x01 << (sizeof_INTNO * 4)) - 1)$
$ 割込み番号中のCPU指定を取得するためのシフト量
$inhno_cpu_shift = sizeof_INHNO * 4$
$ 割込みハンドラ番号中のCPU指定のアンマスク値
$inhno_cpu_umask = ((0x01 << (sizeof_INHNO * 4)) - 1)$

$ 
$ DEF_INH/CFG_INT/ATT_ISRがクラスの囲みに書かれているかのチェック
$ 
$FOREACH inhno INH.ORDER_LIST$
	$IF !LENGTH(INH.CLASS[inhno])$
		$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), inhno, "DEF_INH")$$END$
	$END$
$END$

$FOREACH intno INT.ORDER_LIST$
	$IF !LENGTH(INT.CLASS[intno])$
		$ERROR INT.TEXT_LINE[intno]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), intno, "CFG_INT")$$END$
	$END$
$END$

$FOREACH order ISR.ORDER_LIST$
	$IF !LENGTH(ISR.CLASS[order])$
		$ERROR ISR.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), ISR.INTNO[order], "ATT_ISR")$$END$
	$END$
$END$

$ 
$ DEF_INH/CFG_INT/ATT_ISRで指定した割込み番号・割込みハンドラ番号の上位ビッ
$ トで指定されている割込み可能プロセッサとクラスの割付け可能プロセッサの
$ 整合性のチェック
$ 

$ 
$ DEF_INHでは割込みハンドラ番号を用いるため，初期割付けプロセッサと同じかチェックする
$ 
$FOREACH inhno INH.ORDER_LIST$
	$IF ((inhno >> inhno_cpu_shift) & CLASS_AFFINITY_INI[INH.CLASS[inhno]]) == 0$
		$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("affinity processor of `0x%1$x\' in %2% is not match with class %3% initial processor `0x%4$x\'."), +inhno, "DEF_INH", INH.CLASS[inhno], +CLASS_AFFINITY_INI[INH.CLASS[inhno]])$$END$
	$END$
$END$

$ 
$ ATT_ISR : 属するクラスの割付け可能プロセッサが，intnoで指定した割込み要求
$ ラインが接続されたプロセッサの集合に含まれているかチェックする
$ 
$FOREACH order ISR.ORDER_LIST$
	$IF ((ISR.INTNO[order] >> intno_cpu_shift) != 0) 
		&& (((1 << ((ISR.INTNO[order] >> intno_cpu_shift) - 1)) != CLASS_AFFINITY_MASK[ISR.CLASS[order]]))$
		$ERROR ISR.TEXT_LINE[order]$E_RSATR: $FORMAT(_("processor affinity of `0x%1$x\' in %2% is not match with class %3% processor affinity mask `0x%4$x\'."), +ISR.INTNO[order], "ATT_ISR", ISR.CLASS[order], +CLASS_AFFINITY_MASK[ISR.CLASS[order]])$$END$
	$END$
	$IF ISFUNCTION("TARGET_CHECK_ATT_ISR")$
		$TARGET_CHECK_ATT_ISR(order)$
	$END$
$END$


$ 割込み番号と割込みハンドラ番号の変換テーブルの作成
$IF LENGTH(INTNO_ATTISR_VALID) != LENGTH(INHNO_ATTISR_VALID)$
	$ERROR$length of `INTNO_ATTISR_VALID' is different from length of `INHNO_ATTISR_VALID'$END$
$END$
$i = 0$
$FOREACH intno INTNO_ATTISR_VALID$
	$inhno = AT(INHNO_ATTISR_VALID, i)$
	$INHNO[intno] = inhno$
	$INTNO[inhno] = intno$
	$i = i + 1$
$END$

$ 
$ 割込みハンドラ/割込みサービスルーチンのプロセッサ割り付けのための情報
$ 

$ 割込み要求ラインに関するエラーチェック
$i = 0$
$FOREACH intno INT.ORDER_LIST$
$	// intnoがCFG_INTに対する割込み番号として正しくない場合（E_PAR）
	$IF !LENGTH(FIND(INTNO_CFGINT_VALID, intno))$
		$ERROR INT.TEXT_LINE[intno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intno", intno, "CFG_INT")$$END$
	$END$

$	// intnoがCFG_INTによって設定済みの場合（E_OBJ）
	$j = 0$
	$FOREACH intno2 INT.ORDER_LIST$
		$IF intno == intno2 && j < i$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "intno", intno, "CFG_INT")$$END$
		$END$
		$j = j + 1$
	$END$

$	// intatrが（［TA_ENAINT］｜［TA_EDGE］）でない場合（E_RSATR）
	$IF (INT.INTATR[intno] & ~(TA_ENAINT|TA_EDGE|TARGET_INTATR)) != 0$
		$ERROR INT.TEXT_LINE[intno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "intatr", INT.INTATR[intno], "intno", intno, "CFG_INT")$$END$
	$END$

$	// intpriがCFG_INTに対する割込み優先度として正しくない場合（E_PAR）
	$IF !LENGTH(FIND(INTPRI_CFGINT_VALID, INT.INTPRI[intno]))$
		$ERROR INT.TEXT_LINE[intno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intpri", INT.INTPRI[intno], "CFG_INT")$$END$
	$END$

$	// カーネル管理に固定されているintnoに対して，intpriにTMIN_INTPRI
$	// よりも小さい値が指定された場合（E_OBJ）
	$IF LENGTH(FIND(INTNO_FIX_KERNEL, intno))$
		$IF INT.INTPRI[intno] < TMIN_INTPRI$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' must not have higher priority than %3%"), "intno", INT.INTNO[intno], "TMIN_INTPRI")$$END$
		$END$
	$END$

$	// カーネル管理外に固定されているintnoに対して，intpriにTMIN_INTPRI
$	// よりも小さい値が指定されなかった場合（E_OBJ）
	$IF LENGTH(FIND(INTNO_FIX_NONKERNEL, intno))$
		$IF INT.INTPRI[intno] >= TMIN_INTPRI$
			$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' must have higher priority than %3%"), "intno", INT.INTNO[intno], "TMIN_INTPRI")$$END$
		$END$
	$END$

	$i = i + 1$
$END$

$ 割込みハンドラに関するエラーチェック
$i = 0$
$FOREACH inhno INH.ORDER_LIST$
$	// inhnoがDEF_INHに対する割込みハンドラ番号として正しくない場合（E_PAR）
	$IF !LENGTH(FIND(INHNO_DEFINH_VALID, (inhno)))$
		$ERROR INH.TEXT_LINE[inhno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "inhno", inhno, "DEF_INH")$$END$
	$END$

$	// inhnoがDEF_INHによって設定済みの場合（E_OBJ）
	$j = 0$
	$FOREACH inhno2 INH.ORDER_LIST$
		$IF inhno == inhno2 && j < i$
			$ERROR INH.TEXT_LINE[inhno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "inhno", inhno, "DEF_INH")$$END$
		$END$
		$j = j + 1$
	$END$

$	// inhatrが（［TA_NULL］）でない場合（E_RSATR）
	$IF (INH.INHATR[inhno] & ~TARGET_INHATR) != 0$
		$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "inhatr", INH.INHATR[inhno], "inhno", inhno, "DEF_INH")$$END$
	$END$

$	// カーネル管理に固定されているinhnoに対して，inhatrにTA_NONKERNEL
$	//　が指定されている場合（E_RSATR）
	$IF LENGTH(FIND(INHNO_FIX_KERNEL, inhno))$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) != 0$
			$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("%1% `%2%\' must not be non-kernel interrupt"), "inhno", INH.INHNO[inhno])$$END$
		$END$
	$END$

$	// カーネル管理外に固定されているinhnoに対して，inhatrにTA_NONKERNEL
$	// が指定されていない場合（E_RSATR）
	$IF LENGTH(FIND(INHNO_FIX_NONKERNEL, inhno))$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
			$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("%1% `%2%\' must be non-kernel interrupt"), "inhno", INH.INHNO[inhno])$$END$
		$END$
	$END$

	$IF (LENGTH(INTNO[inhno]) | LENGTH(INTNO[inhno & inhno_cpu_umask]))$
		$IF LENGTH(INTNO[inhno])$
			$intno = INTNO[inhno]$
		$ELSE$
			$intno = INTNO[inhno & inhno_cpu_umask]$
		$END$
$		// inhnoに対応するintnoに対するCFG_INTがない場合（E_OBJ）
		$IF !LENGTH(INT.INTNO[intno])$
			$ERROR INH.TEXT_LINE[inhno]$E_OBJ: $FORMAT(_("%1% `%2%\' corresponding to %3% `%4%\' is not configured with %5%"), "intno", +intno, "inhno", INH.INHNO[inhno], "CFG_INT")$$END$
		$ELSE$
			$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
$				// inhatrにTA_NONKERNELが指定されておらず，inhnoに対応
$				// するintnoに対してCFG_INTで設定された割込み優先度が
$				// TMIN_INTPRIよりも小さい場合（E_OBJ）
				$IF INT.INTPRI[intno] < TMIN_INTPRI$
					$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is higher than %5%"), "intpri", INT.INTPRI[intno], "inhno", inhno, "TMIN_INTPRI")$$END$
				$END$
			$ELSE$
$				// inhatrにTA_NONKERNELが指定されており，inhnoに対応
$				// するintnoに対してCFG_INTで設定された割込み優先度が
$				// TMIN_INTPRI以上である場合（E_OBJ）
				$IF INT.INTPRI[intno] >= TMIN_INTPRI$
					$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is lower than or equal to %5%"), "intpri", INT.INTPRI[intno], "inhno", inhno, "TMIN_INTPRI")$$END$
				$END$
			$END$
$			// CFG_INTが所属するクラスが異なる場合（E_RSATR）
			$IF INH.CLASS[inhno] != INT.CLASS[intno] $
				$ERROR INH.TEXT_LINE[inhno]$E_RSATR: $FORMAT(_("%1% for %2% `%3%\' and %4% for %5% `%6%\' is not defined in same class."), "DEF_INH", "inhno", inhno, "CFG_INT", "intno", intno)$$END$
			$END$
		$END$
	$END$
	$i = i + 1$
$END$

$ 割込みサービスルーチン（ISR）に関するエラーチェック
$FOREACH order ISR.ORDER_LIST$
$	// isratrが（TA_NULL）でない場合（E_RSATR）
	$IF (ISR.ISRATR[order] & ~TARGET_ISRATR) != 0$
		$ERROR ISR.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "isratr", ISR.ISRATR[order], "isr", ISR.ISR[order], "ATT_ISR")$$END$
	$END$

$	// intnoがATT_ISRに対する割込み番号として正しくない場合（E_PAR）
	$IF !LENGTH(FIND(INTNO_ATTISR_VALID, (ISR.INTNO[order])))$
		$ERROR ISR.TEXT_LINE[order]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "intno", ISR.INTNO[order], "ATT_ISR")$$END$
	$END$

$	// (TMIN_ISRPRI <= isrpri && isrpri <= TMAX_ISRPRI)でない場合（E_PAR）
	$IF !(TMIN_ISRPRI <= ISR.ISRPRI[order] && ISR.ISRPRI[order] <= TMAX_ISRPRI)$
		$ERROR ISR.TEXT_LINE[order]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "isrpri", ISR.ISRPRI[order], "ATT_ISR")$$END$
	$END$
$END$

$ 
$ DEF_INHで定義された割込み要求ラインは初期割付けプロセッサでのみ実行される
$  
$FOREACH inhno INH.ORDER_LIST$
	$INH.AFFINITY_MASK[inhno] = 1 << (CLASS_AFFINITY_INI[INH.CLASS[inhno]] - 1)$
	$IF LENGTH(INTNO[inhno])$
		$INT.AFFINITY_MASK[INTNO[inhno]] = INH.AFFINITY_MASK[inhno]$
	$ELSE$
$		// グローバルな割込み要求ラインに対する割込み番号は上位ビットを0として
$		// 対応する割込み要求ラインを取得する．
		$iinhno = inhno & inhno_cpu_umask$
		$IF LENGTH(INTNO[iinhno])$
			$INT.AFFINITY_MASK[INTNO[iinhno]] = INH.AFFINITY_MASK[inhno]$
		$END$
	$END$
$END$

$ 
$ ATT_ISRで定義された割込み要求ラインは割り付け可能プロセッサのいずれかで実行される
$  
$FOREACH order ISR.ORDER_LIST$
	$INH.AFFINITY_MASK[INHNO[ISR.INTNO[order]]] = CLASS_AFFINITY_MASK[ISR.CLASS[order]]$
	$INT.AFFINITY_MASK[ISR.INTNO[order]] = INH.AFFINITY_MASK[INHNO[ISR.INTNO[order]]]$
$END$

$ 割込みサービスルーチン（ISR）の割込みハンドラの生成
$FOREACH intno INTNO_ATTISR_VALID$
	$inhno = INHNO[intno]$

$	// 割込み番号intnoに対して登録されたISRのリストの作成
	$isr_order_list = {}$
	$FOREACH order ISR.ORDER_LIST$
		$IF ISR.INTNO[order] == intno$
			$isr_order_list = APPEND(isr_order_list, order)$
			$order_for_error = order$
		$END$
	$END$

$	// 割込み番号intnoに対して登録されたISRが存在する場合
	$IF LENGTH(isr_order_list) > 0$
$		// intnoに対応するinhnoに対してDEF_INHがある場合（E_OBJ）
		$IF ((intno >> intno_cpu_shift) != 0)$
$			// ローカルな割込み要求ライン
			$IF LENGTH(INH.INHNO[inhno])$
				$ERROR ISR.TEXT_LINE[order_for_error]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated with %4% `%5%\'"), "intno", ISR.INTNO[order_for_error], "ATT_ISR", "inhno", INH.INHNO[inhno])$$END$
			$END$
		$ELSE$
$			// グローバルな割込み要求ライン
			$FOREACH prcid RANGE(1, TNUM_PRCID)$
				$linhno = inhno | (prcid << intno_cpu_shift)$
				$IF LENGTH(INH.INHNO[linhno])$
					$ERROR ISR.TEXT_LINE[order_for_error]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated with %4% `%5%\'"), "intno", ISR.INTNO[order_for_error], "ATT_ISR", "inhno", INH.INHNO[linhno])$$END$
				$END$
			$END$
		$END$

$		// intnoに対するCFG_INTがない場合（E_OBJ）
		$IF !LENGTH(INT.INTNO[intno])$
			$ERROR ISR.TEXT_LINE[order_for_error]$E_OBJ: $FORMAT(_("%1% `%2%\' is not configured with %3%"), "intno", ISR.INTNO[order_for_error], "CFG_INT")$$END$
		$ELSE$
$			// intnoに対してCFG_INTで設定された割込み優先度がTMIN_INTPRI
$			// よりも小さい場合（E_OBJ）
			$IF INT.INTPRI[intno] < TMIN_INTPRI$
				$ERROR INT.TEXT_LINE[intno]$E_OBJ: $FORMAT(_("%1% `%2%\' configured for %3% `%4%\' is higher than %5%"), "intpri", INT.INTPRI[intno], "intno", ISR.INTNO[order_for_error], "TMIN_INTPRI")$$END$
			$END$
		$END$

$		// CFG_INTが所属するクラスが異なる場合（E_RSATR）
		$class = ISR.CLASS[order_for_error]$
		$IF class != INT.CLASS[intno] $
			$ERROR ISR.TEXT_LINE[order_for_error]$E_RSATR: $FORMAT(_("%1% and %2% for %3% `%4%\' is not defined in same class."), "ATT_ISR", "CFG_INT", "intno", ISR.INTNO[order_for_error])$$END$
		$END$

$		// ISRが全て同じクラスに属しているかのチェック
		$FOREACH order isr_order_list$
			$IF class != ISR.CLASS[isr_order_list]$
				$ERROR ISR.TEXT_LINE[order_for_error]$E_RSATR: $FORMAT(_("%1% `%2%\' is not defined in same class"), "intno", ISR.INTNO[order_for_error])$$END$
			$END$
		$END$

$		// DEF_INH(inhno, { TA_NULL, _kernel_inthdr_<intno> } );
		$INH.INHNO[inhno] = inhno$
		$INH.INHATR[inhno] = VALUE("TA_NULL", 0)$
		$INH.INTHDR[inhno] = CONCAT("_kernel_inthdr_", intno)$
		$INH.CLASS[inhno] = class$
		$INH.ORDER_LIST = APPEND(INH.ORDER_LIST, inhno)$

$		// ISR用の割込みハンドラ
		void$NL$
		_kernel_inthdr_$intno$(void)$NL$
		{$NL$
		$IF LENGTH(isr_order_list) > 1$
			$TAB$PRI	saved_ipm;$NL$
			$NL$
			$TAB$i_begin_int($intno$);$NL$
			$TAB$saved_ipm = i_get_ipm();$NL$
		$ELSE$
			$TAB$i_begin_int($intno$);$NL$
		$END$
$		// ISRを優先度順に呼び出す
		$JOINEACH order SORT(isr_order_list, "ISR.ISRPRI") "\tif (i_sense_lock()) {\n\t\ti_unlock_cpu();\n\t}\n\ti_set_ipm(saved_ipm);\n"$
			$TAB$LOG_ISR_ENTER($intno$);$NL$
			$TAB$((ISR)($ISR.ISR[order]$))((intptr_t)($ISR.EXINF[order]$));$NL$ 
			$TAB$LOG_ISR_LEAVE($intno$);$NL$
		$END$
		$TAB$i_end_int($intno$);$NL$
		}$NL$
	$END$
$END$
$NL$

$ 
$  割込み管理機能のための標準的な初期化情報の生成
$ 
$ 割込みハンドラの初期化に必要な情報
$IF !OMIT_INITIALIZE_INTERRUPT || ALT(USE_INTINIB_TABLE,0)$

$ 割込みハンドラ数
#define TNUM_INHNO	$LENGTH(INH.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_inhno = TNUM_INHNO;$NL$
$NL$
$FOREACH inhno INH.ORDER_LIST$
	$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
		INTHDR_ENTRY($inhno$, $FORMAT("0x%x",+inhno)$, $INH.INTHDR[inhno]$)$NL$
	$END$
$END$
$NL$

$ 割込みハンドラ初期化テーブル
$IF LENGTH(INH.ORDER_LIST)$
	const INHINIB _kernel_inhinib_table[TNUM_INHNO] = {$NL$
	$JOINEACH inhno INH.ORDER_LIST ",\n"$
		$IF (INH.INHATR[inhno] & TA_NONKERNEL) == 0$
			$TAB${ ($inhno$), ($INH.INHATR[inhno]$), (FP)(INT_ENTRY($inhno$, $INH.INTHDR[inhno]$)), $CLASS_AFFINITY_INI[INH.CLASS[inhno]]$, (uint_t)($FORMAT("0x%x",+INH.AFFINITY_MASK[inhno])$) }
		$ELSE$
			$TAB${ ($inhno$), ($INH.INHATR[inhno]$), (FP)($INH.INTHDR[inhno]$), $CLASS_AFFINITY_INI[INH.CLASS[inhno]]$, (uint_t)($FORMAT("0x%x",+INH.AFFINITY_MASK[inhno])$) }
		$END$
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const INHINIB, _kernel_inhinib_table);$NL$
$END$$NL$
$END$

$ 割込み要求ラインの初期化に必要な情報
$IF !OMIT_INITIALIZE_INTERRUPT || ALT(USE_INHINTB_TABLE,0)$

$ 割込み要求ライン数
#define TNUM_INTNO	$LENGTH(INT.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_intno = TNUM_INTNO;$NL$
$NL$

$ 割込み要求ライン初期化テーブル
$IF LENGTH(INT.ORDER_LIST)$
	const INTINIB _kernel_intinib_table[TNUM_INTNO] = {$NL$
	$JOINEACH intno INT.ORDER_LIST ",\n"$
		$TAB${ ($intno$), ($INT.INTATR[intno]$), ($INT.INTPRI[intno]$),  $CLASS_AFFINITY_INI[INT.CLASS[intno]]$, (uint_t)($FORMAT("0x%x",+INT.AFFINITY_MASK[intno])$)}
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const INTINIB, _kernel_intinib_table);$NL$
$END$$NL$
$END$

$ 
$  CPU例外管理機能
$ 
/*$NL$
$SPC$*  CPU Exception Management Functions$NL$
$SPC$*/$NL$
$NL$

$ 割込み番号中のCPU指定を取得するためのシフト量
$excno_cpu_shift = sizeof_EXCNO * 4$

$ CPU例外ハンドラに関するエラーチェック
$i = 0$
$FOREACH excno EXC.ORDER_LIST$
$	//  クラスの囲みに入れられていない場合（E_RSATR）
	$IF !LENGTH(EXC.CLASS[excno])$
		$ERROR EXC.TEXT_LINE[excno]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), excno, "DEF_EXC")$$END$
	$END$

$	// excnoがDEF_EXCに対するCPU例外ハンドラ番号として正しくない場合（E_PAR）
	$IF !LENGTH(FIND(EXCNO_DEFEXC_VALID, excno))$
		$ERROR EXC.TEXT_LINE[excno]$E_PAR: $FORMAT(_("illegal %1% `%2%\' in %3%"), "excno", excno, "DEF_EXC")$$END$
	$END$

$	//  所属しているクラスが不正な場合（E_PAR）
$	//    CPU例外ハンドラ番号が示すプロセッサIDと初期割付プロセッサが異なる場合
	$IF ((excno >> excno_cpu_shift) & CLASS_AFFINITY_INI[EXC.CLASS[excno]]) == 0$
		$ERROR EXC.TEXT_LINE[excno]$E_RSATR: $FORMAT(_("affinity processor of `0x%1$x\' in %2% is not match with class %3% initial processor."), +excno, "DEF_EXC", EXC.CLASS[excno])$$END$
	$END$

$	// excnoがDEF_EXCによって設定済みの場合（E_OBJ）
	$j = 0$
	$FOREACH excno2 EXC.ORDER_LIST$
		$IF excno == EXC.EXCNO[excno2] && j < i$
			$ERROR EXC.TEXT_LINE[excno]$E_OBJ: $FORMAT(_("%1% `%2%\' in %3% is duplicated"), "excno", excno, "DEF_EXC")$$END$
		$END$
		$j = j + 1$
	$END$

$	// excatrが（TA_NULL）でない場合（E_RSATR）
	$IF (EXC.EXCATR[excno] & ~TARGET_EXCATR) != 0$
		$ERROR EXC.TEXT_LINE[excno]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "excatr", EXC.EXCATR[excno], "excno", excno, "DEF_EXC")$$END$
	$END$
	$i = i + 1$
$END$

$ CPU例外ハンドラのための標準的な初期化情報の生成
$IF !OMIT_INITIALIZE_EXCEPTION$

$ CPU例外ハンドラ数
#define TNUM_EXCNO	$LENGTH(EXC.ORDER_LIST)$$NL$
const uint_t _kernel_tnum_excno = TNUM_EXCNO;$NL$
$NL$
$FOREACH excno EXC.ORDER_LIST$
	EXCHDR_ENTRY($excno$, $FORMAT("0x%x",+excno)$, $EXC.EXCHDR[excno]$)$NL$
$END$
$NL$

$ CPU例外ハンドラ初期化テーブル
$IF LENGTH(EXC.ORDER_LIST)$
	const EXCINIB _kernel_excinib_table[TNUM_EXCNO] = {$NL$
	$JOINEACH excno EXC.ORDER_LIST ",\n"$
		$TAB${ ($excno$), ($EXC.EXCATR[excno]$), (FP)(EXC_ENTRY($excno$, $EXC.EXCHDR[excno]$)) }
	$END$$NL$
	};$NL$
$ELSE$
	TOPPERS_EMPTY_LABEL(const EXCINIB, _kernel_excinib_table);$NL$
$END$$NL$
$END$

$ 
$  非タスクコンテキスト用のスタック領域
$ 
/*$NL$
$SPC$*  Stack Area for Non-task Context$NL$
$SPC$*/$NL$
$NL$

$	// DEF_ICSがクラスの囲みに入れられているかチェック（E_RSATR）
$FOREACH ics ICS.ORDER_LIST$
	$IF !LENGTH(ICS.CLASS[ics])$
		$ERROR ICS.TEXT_LINE[ics]$E_RSATR: $FORMAT(_("illegal class id of `%1%\' in %2%"), ics, "DEF_ICS")$$END$
	$END$
$END$

$FOREACH prcid RANGE(1, TNUM_PRCID)$

$def_ics_count = 0$
$icsid = 0$
$	// prcidに対してDEF_ICSがあるかチェック
$FOREACH ics ICS.ORDER_LIST$
	$IF CLASS_AFFINITY_INI[ICS.CLASS[ics]] == prcid$
		$def_ics_count = def_ics_count + 1$
		$icsid = ics$
	$END$
$END$

$	// 静的API「DEF_ICS」が複数ある（E_OBJ）
$IF def_ics_count > 1$
	$ERROR$E_OBJ: $FORMAT(_("too many %1% for processor %2%"), "DEF_ICS", prcid)$$END$
$END$

$IF def_ics_count == 0$
$	// DEF_ICSがない場合のデフォルト値の設定
	#ifdef DEFAULT_PRC$prcid$_ISTK$NL$
	#define TOPPERS_PRC$prcid$_ISTKSZ		DEFAULT_PRC$prcid$_ISTKSZ$NL$
	#define TOPPERS_PRC$prcid$_ISTK		DEFAULT_PRC$prcid$_ISTK$NL$
	#else /* DEAULT_PRC$prcid$_ISTK */$NL$
	$IF ISFUNCTION("GENERATE_ISTACK")$
		$GENERATE_ISTACK(prcid)$
		#define TOPPERS_PRC$prcid$_ISTKSZ		ROUND_STK_T(DEFAULT_PRC$prcid$_ISTKSZ)$NL$
	$ELSE$
		$istkname = CONCAT(CONCAT("_kernel_prc",prcid), "_istack")$
		$default_istksz = CONCAT(CONCAT("DEFAULT_PRC",prcid), "_ISTKSZ")$
		$istksz = ALLOC_STACK(istkname, default_istksz)$$NL$
		#define TOPPERS_PRC$prcid$_ISTKSZ		$istksz$$NL$
	$END$
	#define TOPPERS_PRC$prcid$_ISTK		_kernel_prc$prcid$_istack$NL$
	#endif /* DEAULT_PRC$prcid$_ISTK */$NL$
$ELSE$

$	// istkszが0以下か，ターゲット定義の最小値（TARGET_MIN_ISTKSZ）よりも
$	// 小さい場合（E_PAR）
	$IF ICS.ISTKSZ[icsid] <= 0 || (TARGET_MIN_ISTKSZ
									&& ICS.ISTKSZ[icsid] < TARGET_MIN_ISTKSZ)$
		$ERROR ICS.TEXT_LINE[icsid]$E_PAR: $FORMAT(_("%1% `%2%\' in %3% is too small"), "istksz", ICS.ISTKSZ[icsid], "DEF_ICS")$$END$
	$END$

$ 	// istkszがスタック領域のサイズとして正しくない場合（E_PAR）
	$IF !EQ(ICS.ISTK[icsid], "NULL") && CHECK_STKSZ_ALIGN
							&& (ICS.ISTKSZ[icsid] & (CHECK_STKSZ_ALIGN - 1))$
		$ERROR ICS.TEXT_LINE[icsid]$E_PAR: $FORMAT(_("%1% `%2%\' in %3% is not aligned"), "istksz", ICS.ISTKSZ[icsid], "DEF_ICS")$$END$
	$END$

	$IF EQ(ICS.ISTK[icsid], "NULL")$
$		// スタック領域の自動割付け
		$IF ISFUNCTION("GENERATE_ISTACK_ICS")$
			$GENERATE_ISTACK_ICS(prcid, icsid)$
			#define TOPPERS_PRC$prcid$_ISTKSZ		ROUND_STK_T($ICS.ISTKSZ[icsid]$)$NL$
		$ELSE$
			$istkname = CONCAT(CONCAT("_kernel_prc",prcid), "_istack")$
			$istksz = ALLOC_STACK(istkname, ICS.ISTKSZ[icsid])$$NL$
			#define TOPPERS_PRC$prcid$_ISTKSZ		$istksz$$NL$
		$END$
		#define TOPPERS_PRC$prcid$_ISTK		_kernel_prc$prcid$_istack$NL$
	$ELSE$
		#define TOPPERS_PRC$prcid$_ISTKSZ		($ICS.ISTKSZ[icsid]$)$NL$
		#define TOPPERS_PRC$prcid$_ISTK		(void *)($ICS.ISTK[icsid]$)$NL$
	$END$
$END$
$NL$

$END$

$ 非タスクコンテキスト用のスタック領域
const SIZE _kernel_istksz_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
 $TAB$TOPPERS_PRC$prcid$_ISTKSZ
$END$$NL$
};$NL$
$NL$

STK_T *const _kernel_istk_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
 $TAB$TOPPERS_PRC$prcid$_ISTK
$END$$NL$
};$NL$
$NL$

#ifdef TOPPERS_ISTKPT$NL$
STK_T *const _kernel_istkpt_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
 $TAB$TOPPERS_ISTKPT(TOPPERS_PRC$prcid$_ISTK, TOPPERS_PRC$prcid$_ISTKSZ)
$END$$NL$
};$NL$
#endif /* TOPPERS_ISTKPT */$NL$
$NL$

$ 
$  タイムイベント管理
$ 
/*$NL$
$SPC$*  Time Event Management$NL$
$SPC$*/$NL$
$NL$
$IF TOPPERS_SYSTIM_LOCAL$
	$FOREACH prcid RANGE(1, TNUM_PRCID)$
		$IF ISFUNCTION("GENERATE_TMEVT_HEAP")$
			$GENERATE_TMEVT_HEAP(prcid)$
		$ELSE$
			TMEVTN   _kernel_prc$prcid$_tmevt_heap[TNUM_TSKID + TNUM_CYCID + TNUM_ALMID];$NL$
		$END$
	$END$
	$NL$

	TMEVTN* const _kernel_p_tmevt_heap_table[TNUM_PRCID] = {$NL$
	$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
		$TAB$ _kernel_prc$prcid$_tmevt_heap
	$END$$NL$
	};$NL$
	$NL$

$ELSE$
	$IF ISFUNCTION("GENERATE_TMEVT_HEAP_SYSTIM_GLOBAL")$
		$GENERATE_TMEVT_HEAP_SYSTIM_GLOBAL()$
	$ELSE$
		TMEVTN   _kernel_tmevt_heap[TNUM_TSKID + TNUM_CYCID + TNUM_ALMID];$NL$
	$END$
$NL$

$END$

$IF TOPPERS_SYSTIM_LOCAL$

	$FOREACH prcid RANGE(1, TNUM_PRCID)$
		$IF ISFUNCTION("GENERATE_TEVTCB")$
			$GENERATE_TEVTCB(prcid)$
		$ELSE$
			TEVTCB   _kernel_prc$prcid$_tevtcb;$NL$
		$END$
	$END$$NL$

	TEVTCB* const _kernel_p_tevtcb_table[TNUM_PRCID] = {$NL$
	$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
		$TAB$ &_kernel_prc$prcid$_tevtcb
	$END$$NL$
	};$NL$
	$NL$

$ELSE$
	$IF ISFUNCTION("GENERATE_TEVTCB_SYSTIM_GLOBAL")$
		$GENERATE_TEVTCB_SYSTIM_GLOBAL()$
	$ELSE$
		TEVTCB   _kernel_tevtcb;$NL$
	$END$
$END$
$NL$

$ 
$  各モジュールの初期化関数
$ 
/*$NL$
$SPC$*  Module Initialization Function$NL$
$SPC$*/$NL$
$NL$
void$NL$
_kernel_initialize_object(void)$NL$
{$NL$
$TAB$_kernel_initialize_task();$NL$
$IF LENGTH(SEM.ID_LIST)$$TAB$_kernel_initialize_semaphore();$NL$$END$
$IF LENGTH(FLG.ID_LIST)$$TAB$_kernel_initialize_eventflag();$NL$$END$
$IF LENGTH(DTQ.ID_LIST)$$TAB$_kernel_initialize_dataqueue();$NL$$END$
$IF LENGTH(PDQ.ID_LIST)$$TAB$_kernel_initialize_pridataq();$NL$$END$
$IF LENGTH(MBX.ID_LIST)$$TAB$_kernel_initialize_mailbox();$NL$$END$
$IF LENGTH(MPF.ID_LIST)$$TAB$_kernel_initialize_mempfix();$NL$$END$
$IF LENGTH(CYC.ID_LIST)$$TAB$_kernel_initialize_cyclic();$NL$$END$
$IF LENGTH(ALM.ID_LIST)$$TAB$_kernel_initialize_alarm();$NL$$END$
$IF LENGTH(SPN.ID_LIST)$$TAB$_kernel_initialize_spin_lock();$NL$$END$
$TAB$_kernel_initialize_interrupt();$NL$
$TAB$_kernel_initialize_exception();$NL$
}$NL$
$NL$

$ 
$  初期化ルーチンの実行関数
$ 
/*$NL$
$SPC$*  Initialization Routine$NL$
$SPC$*/$NL$
$NL$

void$NL$
_kernel_call_global_inirtn(void)$NL$
{$NL$
$FOREACH order INI.ORDER_LIST$
	$IF !LENGTH(INI.CLASS[order])$
		$IF INI.INIATR[order] != 0$
			$ERROR INI.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "iniatr", INI.INIATR[order], "inirtn", INI.INIRTN[order], "ATT_INI")$$END$
		$END$
		$TAB$((INIRTN)($INI.INIRTN[order]$))((intptr_t)($INI.EXINF[order]$));$NL$ 
	$END$
$END$
}$NL$
$NL$

void$NL$
_kernel_call_local_inirtn(void)$NL$
{$NL$
$FOREACH prcid RANGE(1, TNUM_PRCID)$
$TAB$if (x_prc_index() == INDEX_PRC($prcid$)) {$NL$
	$FOREACH order INI.ORDER_LIST$
		$IF LENGTH(INI.CLASS[order])$
			$IF (CLASS_AFFINITY_INI[INI.CLASS[order]] == prcid) $
				$IF INI.INIATR[order] != 0$
					$ERROR INI.TEXT_LINE[order]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "iniatr", INI.INIATR[order], "inirtn", INI.INIRTN[order], "ATT_INI")$$END$
				$END$
				$TAB$$TAB$((INIRTN)($INI.INIRTN[order]$))((intptr_t)($INI.EXINF[order]$));$NL$ 
			$END$
		$END$
	$END$
$TAB$}$NL$
$NL$
$END$
}$NL$
$NL$

$ 
$  終了処理ルーチンの実行関数
$ 
/*$NL$
$SPC$*  Termination Routine$NL$
$SPC$*/$NL$
$NL$

void$NL$
_kernel_call_global_terrtn(void)$NL$
{$NL$
$FOREACH rorder TER.RORDER_LIST$
$ 	// teratrが（TA_NULL）でない場合（E_RSATR）
	$IF !LENGTH(TER.CLASS[rorder])$
		$IF TER.TERATR[rorder] != 0$
			$ERROR TER.TEXT_LINE[rorder]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "teratr", TER.TERATR[rorder], "terrtn", TER.TERRTN[rorder], "ATT_TER")$$END$
		$END$
		$TAB$((TERRTN)($TER.TERRTN[rorder]$))((intptr_t)($TER.EXINF[rorder]$));$NL$ 
	$END$
$END$
}$NL$
$NL$

void$NL$
_kernel_call_local_terrtn(void)$NL$
{
$FOREACH prcid RANGE(1, TNUM_PRCID)$
$NL$
$TAB$if (x_prc_index() == INDEX_PRC($prcid$)) {$NL$
	$FOREACH rorder TER.RORDER_LIST$
		$IF LENGTH(TER.CLASS[rorder])$
$ 	// teratrが（TA_NULL）でない場合（E_RSATR）
			$IF (CLASS_AFFINITY_INI[TER.CLASS[rorder]] == prcid) $
				$IF TER.TERATR[rorder] != 0$
					$ERROR TER.TEXT_LINE[rorder]$E_RSATR: $FORMAT(_("illegal %1% `%2%\' of %3% `%4%\' in %5%"), "teratr", TER.TERATR[rorder], "terrtn", TER.TERRTN[rorder], "ATT_TER")$$END$
				$END$
				$TAB$$TAB$((TERRTN)($TER.TERRTN[rorder]$))((intptr_t)($TER.EXINF[rorder]$));$NL$
			$END$
		$END$
	$END$
$TAB$}$NL$
$END$
}$NL$
$NL$
