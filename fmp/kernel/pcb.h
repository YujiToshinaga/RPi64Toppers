/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: pcb.h 792 2011-03-10 14:18:33Z ertl-honda $
 */

/*
 *  プロセッサコントロールブロックモジュール
 */
#ifndef TOPPERS_PCB_H
#define TOPPERS_PCB_H

#include "target_pcb.h"

#ifndef TOPPERS_MACRO_ONLY

#include <queue.h>

/*
 *  前方参照
 */
typedef struct task_control_block TCB;
typedef struct time_event_control_block TEVTCB;

/*
 *		プロセッサ管理ブロック
 */
struct processor_control_block {
#if TTYPE_KLOCK != G_KLOCK
	/*
	 *  タスクロック
	 */
	LOCK	 tsk_lock;
#endif /* TTYPE_KLOCK != G_KLOCK */

#if TTYPE_KLOCK == P_KLOCK
	/*
	 *  オブジェクトロック
	 */
	LOCK	 obj_lock;
#endif /* TTYPE_KLOCK == P_KLOCK */

#if TTYPE_KLOCK != G_KLOCK
	/*
	 *  1段目の取得中のロック
	 */
	LOCK *p_firstlock;

	/*
	 *  2段目の取得中のロック
	 */
	LOCK *p_secondlock;
#endif /* TTYPE_KLOCK != G_KLOCK */

	/*
	 *		タスク関連
	 */

	/*
	 *  実行状態のタスク
	 *
	 *  実行状態のタスク（＝プロセッサがコンテキストを持っているタスク）の
	 *  TCBを指すポインタ．実行状態のタスクがない場合はNULLにする．
	 *
	 *  サービスコールの処理中で，自タスク（サービスコールを呼び出したタス
	 *  ク）に関する情報を参照する場合はp_runtskを使う．p_runtskを書き換え
	 *  るのは，ディスパッチャ（と初期化処理）のみである．
	 */
	TCB		 *p_runtsk;

	/*
	 *  最高優先順位のタスク
	 *
	 *  実行できるタスクの中で最高優先順位のタスクのTCBを指すポインタ．実
	 *  行できるタスクがない場合はNULLにする．
	 *
	 *  ディスパッチ禁止状態など，ディスパッチが保留されている間はp_runtsk
	 *  と一致しているとは限らない．
	 */
	TCB		 *p_schedtsk;

	/*
	 *  ディスパッチ／タスク例外処理ルーチン起動要求フラグ
	 *
	 *  割込みハンドラ／CPU例外ハンドラの出口処理に，ディスパッチまたは
	 *  タスク例外処理ルーチンの起動を要求することを示すフラグ．
	 */
	bool_t	 reqflg;

	/*
	 *  タスクディスパッチ可能状態
	 *
	 *  割込み優先度マスク全解除状態であり，ディスパッチ許可状態である（ディ
	 *  スパッチ禁止状態でない）ことを示すフラグ．
	 */
	bool_t	 dspflg;

	/*
	 *  スピンロック取得しているか
	 *
	 *  スピンロックを取得してなければ'0'，スピンロックを取得していれば，
	 *  取得しているスピンロックのIDを保持する．
	 */
	ID		locspnid;

	/*
	 *  レディキューサーチのためのビットマップ
	 *
	 *  レディキューのサーチを効率よく行うために，優先度ごとのタスクキュー
	 *  にタスクが入っているかどうかを示すビットマップを用意している．ビッ
	 *  トマップを使うことで，メモリアクセスの回数を減らすことができるが，
	 *  ビット操作命令が充実していないプロセッサで，優先度の段階数が少ない
	 *  場合には，ビットマップ操作のオーバーヘッドのために，逆に効率が落ち
	 *  る可能性もある．
	 */
	uint16_t ready_primap;

	/*
	 *  タイムイベントコントロールブロックへのポインタ
	 */ 
	TEVTCB   *p_tevtcb;

#ifndef OMIT_TARGET_PCB
	/*
	 *  ターゲット依存のプロセッサコントロールブロック
	 */
	TPCB	target_pcb;
#endif /* OMIT_TARGET_PCB */

	/*
	 *  構造体アクセス時のオフセットサイズに制限があるターゲットがある
	 *  ため，使用頻度が低いデータ構造と，サイズが大きいデータ構造は後
	 *  ろに配置する．
	 */

	/*
	 *  プロセッサID
	 */
	ID		 prcid;

	/*
	 *  カーネル動作状態フラグ
	 *
	 *  スタートアップルーチンで，false（＝0）に初期化されることを期待して
	 *  いる．
	 */
	bool_t	 kerflg;

	/*
	 *  割込み優先度マスク全解除状態
	 *
	 *  割込み優先度マスク全解除状態であることを示すフラグ．
	 */
	bool_t	ipmflg;

	/*
	 *  ディスパッチ禁止状態
	 *
	 *  ディスパッチ禁止状態であることを示すフラグ．
	 */
	bool_t	 disdsp;

	/*
	 *  レディキュー
	 *
	 *  レディキューは，実行できる状態のタスクを管理するためのキューである．
	 *  実行状態のタスクも管理しているため，レディ（実行可能）キューという
	 *  名称は正確ではないが，レディキューという名称が定着しているため，こ
	 *  の名称で呼ぶことにする．
	 *
	 *  レディキューは，優先度ごとのタスクキューで構成されている．タスクの
	 *  TCBは，該当する優先度のキューに登録される．
	 */
	QUEUE	 ready_queue[TNUM_TPRI];
};

/*
 *  PCBへのアクセステーブル（kernel_cfg.c）
 */
extern PCB* const p_pcb_table[];

Inline PCB*
get_mp_p_pcb(ID prcid)
{
	return p_pcb_table[prcid - 1];
}

#ifndef OMIT_GET_MY_P_PCB
Inline PCB*
get_my_p_pcb(void)
{
	return p_pcb_table[x_prc_index()];
}
#endif /* OMIT_GET_MY_P_PCB */

/*
 *  自プロセッサのプロセッサ依存プロセッサコントロールブロックの取得
 */
#ifndef OMIT_GET_MY_P_TPCB
Inline TPCB*
get_my_p_tpcb(void)
{
	return &((get_my_p_pcb())->target_pcb);
}
#endif /* OMIT_GET_MY_P_TPCB */
#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_PCB_H */
