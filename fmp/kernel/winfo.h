/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2007-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: winfo.h 378 2009-09-11 16:21:24Z ertl-honda $
 */

/*
 *  待ち情報定義ヘッダー
 *  task.h からののみインクルードすること 
 */
#ifndef TOPPERS_WINFO_H
#define TOPPERS_WINFO_H

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分操作ルーチン
 *
 *  同期・通信オブジェクトの初期化ブロックと管理ブロックの先頭部分は共
 *  通になっている．以下は，その共通部分を扱うための型およびルーチン群
 *  である．
 *
 *  複数の待ちキューを持つ同期・通信オブジェクトの場合，先頭以外の待ち
 *  キューを操作する場合には，これらのルーチンは使えない．また，オブジェ
 *  クト属性のTA_TPRIビットを参照するので，このビットを他の目的に使って
 *  いる場合も，これらのルーチンは使えない．
 */

/*
 *  同期・通信オブジェクトの初期化ブロックの共通部分
 */
typedef struct wait_object_initialization_block {
	ATR			wobjatr;		/* オブジェクト属性 */
#if TTYPE_KLOCK == P_KLOCK
	LOCK		*p_obj_lock;		/* オブジェクトロックへのポインタ */
#endif /* TTYPE_KLOCK == P_KLOCK */
} WOBJINIB;

/*
 *  同期・通信オブジェクトの管理ブロックの共通部分
 */
typedef struct wait_object_control_block {
	QUEUE		wait_queue;		/* 待ちキュー */
	const WOBJINIB *p_wobjinib;	/* 初期化ブロックへのポインタ */
#if TTYPE_KLOCK == F_KLOCK
	LOCK		obj_lock;		/* オブジェクトロック */
#endif /* TTYPE_KLOCK == F_KLOCK */
} WOBJCB;

/*
 *  イベントフラグ待ち情報ブロックの定義
 *
 *  flgptnは，waiptnおよびwfmodeと同時に使うことはないため，共用体を使
 *  えばメモリを節約することが可能である．
 */
typedef struct winfo_flg{
	FLGPTN		waiptn;			/* 待ちパターン */
	MODE		wfmode;			/* 待ちモード */
	FLGPTN		flgptn;			/* 待ち解除時のパターン */
}WINFO_FLG;

/*
 *  データキュー待ち情報ブロックの定義
 *
 *  データキューへの送信待ちとデータキューからの受信待ちで，同じ待ち情
 *  報ブロックを使う．
 */
typedef struct winfo_dtq{
	intptr_t	data;			/* 送受信データ */
}WINFO_DTQ;

/*
 *  優先度データキュー待ち情報ブロックの定義
 *
 *  優先度データキューへの送信待ちと優先度データキューからの受信待ちで，
 *  同じ待ち情報ブロックを使う．
 */
typedef struct winfo_pdq{
	intptr_t	data;			/* 送受信データ */
	PRI			datapri;		/* データ優先度 */
}WINFO_PDQ;

/*
 *  メールボックス待ち情報ブロックの定義
 */
typedef struct winfo_mbx{
	T_MSG		*pk_msg;		/* 受信したメッセージ */
}WINFO_MBX;

/*
 *  固定長メモリプール待ち情報ブロックの定義
 */
typedef struct winfo_mpf{
	void		*blk;			/* 獲得したメモリブロック */
}WINFO_MPF;

/*
 *  TCBに登録するオブジェクト毎の待ち情報ブロック
 */
typedef union winfo_obj{
	WINFO_FLG flg;
	WINFO_DTQ dtq;
	WINFO_PDQ pdq;
	WINFO_MBX mbx;
	WINFO_MPF mpf;
}WINFO_OBJ;

#endif  /* TOPPERS_WINFO_H */
