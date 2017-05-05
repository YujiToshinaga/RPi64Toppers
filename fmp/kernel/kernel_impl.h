/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2004-2014 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: kernel_impl.h 1080 2014-12-25 15:25:16Z ertl-honda $
 */

/*
 *		TOPPERS/FMPカーネル内部向け標準ヘッダファイル
 *
 *  このヘッダファイルは，カーネルを構成するプログラムのソースファイル
 *  で必ずインクルードするべき標準ヘッダファイルである．
 *
 *  アセンブリ言語のソースファイルからこのファイルをインクルードする時
 *  は，TOPPERS_MACRO_ONLYを定義しておく．これにより，マクロ定義以外を
 *  除くようになっている．
 */

#ifndef TOPPERS_KERNEL_IMPL_H
#define TOPPERS_KERNEL_IMPL_H

/*
 *  カーネルの内部識別名のリネーム
 */
#include "kernel_rename.h"

/*
 *  アプリケーションと共通のヘッダファイル
 */
#include <kernel.h>

/*
 *  システムログ機能のための定義
 */
#include <t_syslog.h>

/*
 *  型キャストを行うマクロの定義
 */
#ifndef CAST
#define CAST(type, val)		((type)(val))
#endif /* CAST */

/*
 *  プロセッサIDの最小値の定義
 */
#define TMIN_PRCID		1		/* プロセッサIDの最小値 */

/*
 *  オブジェクトIDの最小値の定義
 */
#define TMIN_TSKID		1		/* タスクIDの最小値 */
#define TMIN_SEMID		1		/* セマフォIDの最小値 */
#define TMIN_FLGID		1		/* フラグIDの最小値 */
#define TMIN_DTQID		1		/* データキューIDの最小値 */
#define TMIN_PDQID		1		/* 優先度データキューIDの最小値 */
#define TMIN_MBXID		1		/* メールボックスIDの最小値 */
#define TMIN_MPFID		1		/* 固定長メモリプールIDの最小値 */
#define TMIN_CYCID		1		/* 周期ハンドラIDの最小値 */
#define TMIN_ALMID		1		/* アラームハンドラIDの最小値 */
#define TMIN_SPNID		1		/* スピンロックIDの最小値 */     

/*
 *  優先度の段階数の定義
 */
#define TNUM_TPRI		(TMAX_TPRI - TMIN_TPRI + 1)
#define TNUM_MPRI		(TMAX_MPRI - TMIN_MPRI + 1)
#define TNUM_INTPRI		(TMAX_INTPRI - TMIN_INTPRI + 1)

/*
 *  PCB関連の定義
 */
#include "pcb.h"

/*
 *  ターゲット依存情報の定義
 */
#include "target_config.h"

/*
 *  すべての関数をコンパイルするための定義
 */
#ifdef ALLFUNC
#include "allfunc.h"
#endif /* ALLFUNC */

/*
 *  ビットフィールドでの符号無し整数型
 *
 *  8ビット以下の幅のビットフィールドで，符号無し整数値を保持したい場合
 *  に用いるべきデータ型．ANSI Cでは，ビットフィールドのデータ型には
 *  intとunsigned intしか許されないため，デフォルトの定義はunsigned
 *  intとしているが，ターゲットおよびツール依存で，unsigned charまたは
 *  unsigned shortに定義した方が効率が良い場合がある．
 */
#ifndef BIT_FIELD_UINT
#define BIT_FIELD_UINT		unsigned int
#endif /* BIT_FIELD_UINT */

/*
 *  ビットフィールドでのブール型
 *
 *  1ビット幅のビットフィールドで，ブール値を保持したい場合に用いるべき
 *  データ型．デフォルトではBIT_FIELD_UINTと同一に定義しているが，ブー
 *  ル値を保持することを明示するために別の名称としている．
 */
#ifndef BIT_FIELD_BOOL
#define BIT_FIELD_BOOL		BIT_FIELD_UINT
#endif /* BIT_FIELD_BOOL */

/*
 *  ヘッダファイルを持たないモジュールの関数・変数の宣言
 */
#ifndef TOPPERS_MACRO_ONLY

/*
 *  各モジュールの初期化（kernel_cfg.c）
 */
extern void	initialize_object(void);

/*
 *  グローバル初期化ルーチンの実行（kernel_cfg.c）
 */
extern void	call_global_inirtn(void);

/*
 *  ローカル初期化ルーチンの実行（kernel_cfg.c）
 */
extern void	call_local_inirtn(void);

/*
 *  グローバル終了処理ルーチンの実行（kernel_cfg.c）
 */
extern void	call_global_terrtn(void);

/*
 *  ローカル終了処理ルーチンの実行（kernel_cfg.c）
 */
extern void	call_local_terrtn(void);

/*
 *  非タスクコンテキスト用のスタック領域（kernel_cfg.c）
 */
extern const SIZE	istksz_table[];		/* スタック領域のサイズ（丸めた値） */
extern STK_T *const	istk_table[];		/* スタック領域の先頭番地 */
#ifdef TOPPERS_ISTKPT
extern STK_T *const	istkpt_table[];		/* スタックポインタの初期値 */
#endif /* TOPPERS_ISTKPT */

/*
 *  カーネルの起動（startup.c）
 */
extern void	sta_ker(void);

/*
 *  カーネルの終了処理（startup.c）
 */
extern void	exit_kernel(void);

#endif /* TOPPERS_MACRO_ONLY */
#endif /* TOPPERS_KERNEL_IMPL_H */
