/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: time_event.c 1087 2015-02-03 01:04:34Z ertl-honda $
 */

/*
 *		タイムイベント管理モジュール
 */

#include "kernel_impl.h"
#include "time_event.h"
#include "task.h"
#include "mp.h"

/*
 *  タイムイベントヒープ操作マクロ
 */
#define	PARENT(index)		((index) >> 1)		/* 親ノードを求める */
#define	LCHILD(index)		((index) << 1)		/* 左の子ノードを求める */
#define	TMEVT_NODE(p_tevtcb, index)	((p_tevtcb->p_tmevt_heap)[(index) - 1])

/*
 *  イベント発生時刻比較マクロ
 *
 *  イベント発生時刻は，min_timeからの相対値で比較する．すなわち，
 *  min_timeを最小値（最も近い時刻），mit_time-1が最大値（最も遠い時刻）
 *  遠い時刻）とみなして比較する．
 */
#define	EVTTIM_LT(p_tevtcb, t1, t2) (((t1) - p_tevtcb->min_time) < ((t2) - p_tevtcb->min_time))
#define	EVTTIM_LE(p_tevtcb, t1, t2) (((t1) - p_tevtcb->min_time) <= ((t2) - p_tevtcb->min_time))

#ifdef TOPPERS_tmeini

/*
 *  タイマモジュールの初期化
 */
void
initialize_tmevt(void)
{
	PCB		*my_p_pcb;
	TEVTCB	*my_p_tevtcb;

	my_p_pcb = get_my_p_pcb();

	/*
	 *  PCBへのセット
	 */
#ifdef TOPPERS_SYSTIM_LOCAL
	/* ローカルタイマの場合 */
	my_p_pcb->p_tevtcb = p_tevtcb_table[x_prc_index()];
	my_p_pcb->p_tevtcb->p_tmevt_heap = p_tmevt_heap_table[x_prc_index()];
#else /* TOPPERS_SYSTIM_GLOBAL */
	/* グローバルタイマの場合 */
	my_p_pcb->p_tevtcb = &tevtcb;
	my_p_pcb->p_tevtcb->p_tmevt_heap = tmevt_heap;

	if (TOPPERS_SYSTIM_PRCID != ID_PRC(x_prc_index())) {
		return;
	}
#endif /* TOPPERS_SYSTIM_GLOBAL */

	my_p_tevtcb = my_p_pcb->p_tevtcb;

	my_p_tevtcb->min_time = 0U;
	my_p_tevtcb->current_time = 0U;
	my_p_tevtcb->next_time = my_p_tevtcb->current_time + TIC_NUME / TIC_DENO;
#if TIC_DENO != 1U
	my_p_tevtcb->next_subtime = TIC_NUME % TIC_DENO;
#endif /* TIC_DENO != 1U */
	my_p_tevtcb->last_index = 0U;
}

#endif /* TOPPERS_tmeini */

/*
 *  タイムイベントの挿入位置を上向きに探索
 *
 *  時刻timeに発生するタイムイベントを挿入するノードを空けるために，
 *  ヒープの上に向かって空ノードを移動させる．移動前の空ノードの位置を
 *  indexに渡すと，移動後の空ノードの位置（すなわち挿入位置）を返す．
 */
#ifdef TOPPERS_tmeup

uint_t
tmevt_up(TEVTCB *p_tevtcb, uint_t index, EVTTIM time)
{
	uint_t	parent;

	while (index > 1) {
		/*
		 *  親ノードのイベント発生時刻の方が早い（または同じ）
		 *  ならば，index が挿入位置なのでループを抜ける．
		 */
		parent = PARENT(index);
		if (EVTTIM_LE(p_tevtcb, TMEVT_NODE(p_tevtcb, parent).time, time)) {
			break;
		}

		/*
		 *  親ノードを index の位置に移動させる．
		 */
		TMEVT_NODE(p_tevtcb, index) = TMEVT_NODE(p_tevtcb, parent);
		TMEVT_NODE(p_tevtcb, index).p_tmevtb->index = index;

		/*
		 *  index を親ノードの位置に更新．
		 */
		index = parent;
	}
	return(index);
}

#endif /* TOPPERS_tmeup */

/*
 *  タイムイベントの挿入位置を下向きに探索
 *
 *  時刻timeに発生するタイムイベントを挿入するノードを空けるために，
 *  ヒープの下に向かって空ノードを移動させる．移動前の空ノードの位置を 
 *  indexに渡すと，移動後の空ノードの位置（すなわち挿入位置）を返す．
 */
#ifdef TOPPERS_tmedown

uint_t
tmevt_down(TEVTCB *p_tevtcb, uint_t index, EVTTIM time)
{
	uint_t	child;

	while ((child = LCHILD(index)) <= p_tevtcb->last_index) {
		/*
		 *  左右の子ノードのイベント発生時刻を比較し，早い方の
		 *  子ノードの位置を child に設定する．以下の子ノード
		 *  は，ここで選ばれた方の子ノードのこと．
		 */
		if (child + 1 <= p_tevtcb->last_index
						&& EVTTIM_LT(p_tevtcb,
									 TMEVT_NODE(p_tevtcb, child + 1).time,
									 TMEVT_NODE(p_tevtcb, child).time)) {
			child = child + 1;
		}

		/*
		 *  子ノードのイベント発生時刻の方が遅い（または同じ）
		 *  ならば，index が挿入位置なのでループを抜ける．
		 */
		if (EVTTIM_LE(p_tevtcb, time, TMEVT_NODE(p_tevtcb, child).time)) {
			break;
		}

		/*
		 *  子ノードを index の位置に移動させる．
		 */
		TMEVT_NODE(p_tevtcb, index) = TMEVT_NODE(p_tevtcb, child);
		TMEVT_NODE(p_tevtcb, index).p_tmevtb->index = index;

		/*
		 *  index を子ノードの位置に更新．
		 */
		index = child;
	}
	return(index);
}

#endif /* TOPPERS_tmedown */

/*
 *  タイムイベントヒープへの登録
 *
 *  p_tmevtbで指定したタイムイベントブロックを，timeで指定した時間が経
 *  過後にイベントが発生するように，タイムイベントヒープに登録する．
 */
#ifdef TOPPERS_tmeins

void
tmevtb_insert(TEVTCB *p_tevtcb, TMEVTB *p_tmevtb, EVTTIM time)
{
	uint_t	index;

	/*
	 *  last_index をインクリメントし，そこから上に挿入位置を探す．
	 */
	index = tmevt_up(p_tevtcb, ++(p_tevtcb->last_index), time);

	/*
	 *  タイムイベントを index の位置に挿入する．
	 */ 
	TMEVT_NODE(p_tevtcb, index).time = time;
	TMEVT_NODE(p_tevtcb, index).p_tmevtb = p_tmevtb;
	p_tmevtb->index = index;
}

#endif /* TOPPERS_tmeins */

/*
 *  タイムイベントヒープからの削除
 */
#ifdef TOPPERS_tmedel

RELTIM
tmevtb_delete(TEVTCB *p_tevtcb, TMEVTB *p_tmevtb)
{
	uint_t	index = p_tmevtb->index;
	uint_t	parent;
	EVTTIM	event_time = TMEVT_NODE(p_tevtcb, p_tevtcb->last_index).time;
	EVTTIM	time;
	RELTIM	left_time;

	/*
	 *  タイムイベントまでの残り時間を計算
	 *  tmevt_leftim()とは異なり大きい方(current_timeとの差分)に丸める． 
	 */
	time = TMEVT_NODE(p_tevtcb, p_tmevtb->index).time;
	left_time = (RELTIM)(time - p_tevtcb->current_time);

	/*
	 *  削除によりタイムイベントヒープが空になる場合は何もしない．
	 */
	if (--(p_tevtcb->last_index) == 0) {
		return(left_time);
	}

	/*
	 *  削除したノードの位置に最後のノード（last_index+1の位置のノード）
	 *  を挿入し，それを適切な位置へ移動させる．実際には，最後のノード
	 *  を実際に挿入するのではなく，削除したノードの位置が空ノードにな
	 *  るので，最後のノードを挿入すべき位置へ向けて空ノードを移動させ
	 *  る．
	 *  最後のノードのイベント発生時刻が，削除したノードの親ノードのイ
	 *  ベント発生時刻より前の場合には，上に向かって挿入位置を探す．そ
	 *  うでない場合には，下に向かって探す．
	 */
	if (index > 1 && EVTTIM_LT(p_tevtcb, event_time,
								TMEVT_NODE(p_tevtcb, parent = PARENT(index)).time)) {
		/*
		 *  親ノードをindexの位置に移動させる．
		 */
		TMEVT_NODE(p_tevtcb, index) = TMEVT_NODE(p_tevtcb, parent);
		TMEVT_NODE(p_tevtcb, index).p_tmevtb->index = index;

		/*
		 *  削除したノードの親ノードから上に向かって挿入位置を探す．
		 */
		index = tmevt_up(p_tevtcb, parent, event_time);
	}
	else {
		/*
		 *  削除したノードから下に向かって挿入位置を探す．
		 */
		index = tmevt_down(p_tevtcb, index, event_time);
	}

	/*
	 *  最後のノードをindexの位置に挿入する．
	 */ 
	TMEVT_NODE(p_tevtcb, index) = TMEVT_NODE(p_tevtcb, (p_tevtcb->last_index) + 1);
	TMEVT_NODE(p_tevtcb, index).p_tmevtb->index = index;

	return(left_time);
}

#endif /* TOPPERS_tmedel */

/*
 *  タイムイベントヒープの先頭のノードの削除
 */
Inline void
tmevtb_delete_top(TEVTCB* p_tevtcb)
{
	uint_t	index;
	EVTTIM	event_time = TMEVT_NODE(p_tevtcb, p_tevtcb->last_index).time;

	/*
	 *  削除によりタイムイベントヒープが空になる場合は何もしない．
	 */
	if (--(p_tevtcb->last_index) == 0) {
		return;
	}

	/*
	 *  ルートノードに最後のノード（last_index + 1 の位置のノード）を
	 *  挿入し，それを適切な位置へ移動させる．実際には，最後のノードを
	 *  実際に挿入するのではなく，ルートノードが空ノードになるので，最
	 *  後のノードを挿入すべき位置へ向けて空ノードを移動させる．
	 */
	index = tmevt_down(p_tevtcb, 1, event_time);

	/*
	 *  最後のノードをindexの位置に挿入する．
	 */ 
	TMEVT_NODE(p_tevtcb, index) = TMEVT_NODE(p_tevtcb, p_tevtcb->last_index + 1);
	TMEVT_NODE(p_tevtcb, index).p_tmevtb->index = index;
}

/*
 *  タイムイベントまでの残り時間の計算
 */
#ifdef TOPPERS_tmeltim

RELTIM
tmevt_lefttim(TEVTCB *p_tevtcb, TMEVTB *p_tmevtb)
{
	EVTTIM	time;

	time = TMEVT_NODE(p_tevtcb, p_tmevtb->index).time;
	if (EVTTIM_LE(p_tevtcb, time, p_tevtcb->next_time)) {
		/*
		 *  次のタイムティックで処理される場合には0を返す．
		 */
		return(0U);
	}
	else {
		return((RELTIM)(time - base_time(p_tevtcb)));
	}
}

#endif /* TOPPERS_tmeltim */

/*
 *  タイムティックの供給
 */
#ifdef TOPPERS_sigtim

void
signal_time(void)
{
	TMEVTB	*p_tmevtb;
	PCB		*my_p_pcb;
	TEVTCB	*my_p_tevtcb;

	assert(sense_context());
	assert(!i_sense_lock());

	i_lock_cpu();
	my_p_pcb = i_acquire_tsk_lock_self();

	my_p_tevtcb = my_p_pcb->p_tevtcb;

	/*
	 *  current_timeを更新する．
	 */
	my_p_tevtcb->current_time = my_p_tevtcb->next_time;

	/*
	 *  next_time，next_subtimeを更新する．
	 */
#if TIC_DENO == 1U
	my_p_tevtcb->next_time = my_p_tevtcb->current_time + TIC_NUME;
#else /* TIC_DENO == 1U */
	my_p_tevtcb->next_subtime += TIC_NUME % TIC_DENO;
	my_p_tevtcb->next_time = my_p_tevtcb->current_time + TIC_NUME / TIC_DENO;
	if (my_p_tevtcb->next_subtime >= TIC_DENO) {
		my_p_tevtcb->next_subtime -= TIC_DENO;
		my_p_tevtcb->next_time += 1U;
	}
#endif /* TIC_DENO == 1U */

	/*
	 *  curent_timeよりイベント発生時刻の早い（または同じ）タイムイベン
	 *  トを，タイムイベントヒープから削除し，コールバック関数を呼び出
	 *  す．
	 */
	while ((my_p_tevtcb->last_index) > 0 && EVTTIM_LE(my_p_tevtcb, TMEVT_NODE(my_p_tevtcb, 1).time, my_p_tevtcb->current_time)) {
		p_tmevtb = TMEVT_NODE(my_p_tevtcb, 1).p_tmevtb;
		tmevtb_delete_top(my_p_tevtcb);
		(*(p_tmevtb->callback))(p_tmevtb->arg);
	}

	/*
	 *  min_timeを更新する．
	 */
	my_p_tevtcb->min_time = my_p_tevtcb->current_time;

	release_tsk_lock(my_p_pcb);
	i_unlock_cpu();
}

#endif /* TOPPERS_sigtim */


#ifdef TOPPERS_get_my_current_time

/*
 *  syslogで使用するために定義
 */
ulong_t
get_my_current_time(void){
	PCB		*my_p_pcb;

	my_p_pcb = get_my_p_pcb();

	return my_p_pcb->p_tevtcb->current_time;
}

#endif /* TOPPERS_get_my_current_time */
