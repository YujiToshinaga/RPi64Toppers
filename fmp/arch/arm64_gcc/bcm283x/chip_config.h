/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_config.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  チップ依存モジュール（BCM283X用）
 *
 *  カーネルのチップ依存部のインクルードファイル．kernel_impl.hのター
 *  ゲット依存部の位置付けとなる．
 */

#ifndef TOPPERS_CHIP_CONFIG_H
#define TOPPERS_CHIP_CONFIG_H

#include <sil.h>
#include "bcm283x.h"

#ifdef USE_IPI_DIS_HANDER_BYPASS
#define OMIT_KER_REQ_ON_IPI
#endif /* USE_IPI_DIS_HANDER_BYPASS */

/*
 *  割込み番号のマスク（下位16bitのみが有効）
 */
#define INTNO_MASK(intno) (intno & 0xffffU)

/*
 *  割込み番号の接続プロセッサのプロセッサID（上位16bit）
 */
#define INTNO_PRCID(intno) (intno >> 16U)

/*
 *  割込みハンドラ番号のマスク（下位16bitのみが有効）
 */
#define INHNO_MASK(intno) (intno & 0xffffU)

/*
 *  割込みハンドラ番号と割込み番号に関する定義
 */
#define PRI_LEVEL   32

#define TMIN_INTNO  IRQNO_MIN
#define TMAX_INTNO  IRQNO_MAX
#define TNUM_INT    IRQNO_NUM

#define TMIN_INHNO  IRQNO_MIN
#define TMAX_INHNO  IRQNO_MAX
#define TNUM_INH    IRQNO_NUM

#ifndef TOPPERS_MACRO_ONLY

/*
 *  プロセッサINDEX（0オリジン）の取得
 */
Inline uint32_t
x_prc_index(void)
{
	uint64_t mpidr;
	uint32_t index;

	Asm("mrs %0, mpidr_el1":"=r"(mpidr));

	index = (uint32_t)mpidr & 0x000000ff;

	return index;
}

///*
// *  GICのtarget cpuへの変換
// */
//Inline uint8_t
//x_gic_target(uint32_t prc_index)
//{
//	uint8_t target;
//
//	target = 0x1 << prc_index;
//
//	return target;
//}

#ifdef USE_IPI_DIS_HANDER_BYPASS
/*
 *  カーネル終了処理要求
 */
extern void ext_ker_request(void);
#endif /* USE_IPI_DIS_HANDER_BYPASS */

/*
 *  ロック関係
 */

/*
 *  ジャイアントロックの初期化（G_LOCKの場合）
 */
Inline void
x_initialize_giant_lock(LOCK *p_giant_lock)
{
	*p_giant_lock = 0;
}

/*
 *  タスクロックの初期化
 */
Inline void
x_initialize_tsk_lock(LOCK *p_tsk_lock)
{
	*p_tsk_lock = 0;
}

/*
 *  オブジェクトロックの初期化
 */
Inline void
x_initialize_obj_lock(LOCK *p_obj_lock)
{
	*p_obj_lock = 0;
}

/*
 *  割込み番号の範囲の判定
 */
#if TMIN_INTNO == 0
#define VALID_INTNO(prcid, intno) ((INTNO_MASK(intno) <= TMAX_INTNO) \
                                    && (INTNO_PRCID(intno) == 0U || INTNO_PRCID(intno) == prcid))
#else /* !TMIN_INTNO == 0 */
#define VALID_INTNO(prcid, intno) (((TMIN_INTNO <= (INTNO_MASK(intno)) && (INTNO_MASK(intno)) <= TMAX_INTNO)) \
                                    && (INTNO_PRCID(intno) == 0U || INTNO_PRCID(intno) == prcid))
#endif /* TMIN_INTNO == 0 */

#define VALID_INTNO_DISINT(prcid, intno)	VALID_INTNO(prcid, intno)
#define VALID_INTNO_CFGINT(prcid, intno)	VALID_INTNO(prcid, intno)

/*
 *  割込みハンドラの登録用テーブル（kernel_cfg.c）
 */
extern const FP* const p_inh_table[TNUM_PRCID];

/*
 *  割込み優先度の登録用テーブル（kernel_cfg.c）
 */
extern const PRI* const p_inh_iipm_table[TNUM_PRCID];

/*
 *  割込みマスクの登録用テーブル（kernel_cfg.c）
 */
extern const uint32_t* const p_iipm_mask_table[TNUM_PRCID];

/*
 *  割込みハンドラの設定
 *
 *  割込みハンドラ番号inhnoの割込みハンドラの起動番地をinthdrに設定する
 */
Inline void
x_define_inh(INHNO inhno, FP int_entry, uint_t affinity_mask)
{
#if TMIN_INTNO == 0
	assert((INHNO_MASK(inhno) <= TMAX_INHNO));
#else /* !TMIN_INTNO == 0 */
	assert(((TMIN_INHNO <= INHNO_MASK(inhno)) && (INHNO_MASK(inhno) <= TMAX_INHNO)));
#endif /* TMIN_INTNO == 0 */
}

/*
 *  割込みハンドラの出入口処理の生成マクロ
 *
 */
#define INT_ENTRY(inhno, inthdr)    inthdr
#define INTHDR_ENTRY(inhno, inhno_num, inthdr)

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  割込み優先度マスク操作ライブラリ（PL390（GIC）用）
 *
 *  GICでは，GIC内の優先度レジスタにCPUの割込み優先度マスク（IPM）を設定する．  
 *  割込み優先度マスクは値が大きいほど低優先度である．
 *  割込み優先度マスクの段階はGICの実装時のパラメータにより，256,64,32,16 から
 *  選択可能である． 
 * 
 *  IPMを保存しておくために，割込み優先度の外部表現（-1から連続した負の値）
 *  を使うことも可能であるが，余計な計算が必要になる．これを避けるため，IPMを
 *  保存する場合には，GICで扱う優先度の形式とする．この値を割込み優先度マスク
 *  の内部表現と呼び，IIPMと書くことにする． 
 */

/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  アセンブリ言語のソースファイルからインクルードする場合のために，型
 *  キャストしない定義も用意している．
 */
//#ifndef TOPPERS_MACRO_ONLY
//#define EXT_IPM(iipm)    ((PRI)(iipm - GICC_PMR_MASK))    /* 内部表現を外部表現に */
//#define INT_IPM(ipm)    ((uint8_t)(ipm + GICC_PMR_MASK))  /* 外部表現を内部表現に */
//#else /* TOPPERS_MACRO_ONLY */
//#define EXT_IPM(iipm)    (iipm - GICC_PMR_MASK)           /* 内部表現を外部表現に */
//#define INT_IPM(ipm)    (ipm + GICC_PMR_MASK)             /* 外部表現を内部表現に */
//#endif /* TOPPERS_MACRO_ONLY */
#ifndef TOPPERS_MACRO_ONLY
#define EXT_IPM(iipm)    ((PRI)(iipm - 0))    /* 内部表現を外部表現に */
#define INT_IPM(ipm)    ((uint8_t)(ipm + 0))  /* 外部表現を内部表現に */
#else /* TOPPERS_MACRO_ONLY */
#define EXT_IPM(iipm)    (iipm - 0)           /* 内部表現を外部表現に */
#define INT_IPM(ipm)    (ipm + 0)             /* 外部表現を内部表現に */
#endif /* TOPPERS_MACRO_ONLY */

/*
 *  TIPM_ENAALL（割込み優先度マスク全解除）の内部表現
 */
#define IIPM_ENAALL        (INT_IPM(TIPM_ENAALL))

#ifndef TOPPERS_MACRO_ONLY

/*
 *  IPM（ハードウェアの割込み優先度マスク，内部表現）の現在値の読出し
 */
Inline uint8_t
current_iipm(void)
{
//	return(gicc_current_priority());
	return 0;
}

/*
 *  IPM（ハードウェアの割込み優先度マスク，内部表現）の現在値の設定
 */
Inline void
set_iipm(uint8_t iipm)
{
//	gicc_set_priority(iipm);
}

#endif /* TOPPERS_MACRO_ONLY */

//#ifdef USE_GIC_CPULOCK
//
///*
// *  USE_GIC_CPULOCK時の設定
// */
//
//#define IIPM_LOCK 0x00
//
//#ifndef TOPPERS_MACRO_ONLY
//
///*
// *  コンテキストの参照(USE_GIC_CPULOCK時)
// *
// *  ARMでは，タスクコンテキストと非タスクコンテキストの両方をスーパー
// *  バイザーモードで動作させる．そのため，CPSRの内容では判別できない．
// *  そのため，例外（割込み/CPU例外）のネスト回数をカウントする変数
// *  （excpt_nest_count）を用意し，例外の入り口でインクリメントすること
// *  で，コンテキストを判定する．
// */
//Inline bool_t
//sense_context(void)
//{
//	uint32_t tmp;
//	uint8_t saved_iipm;
//	TPCB* my_p_tpcb;
//
//	/*
//	 *  マイグレーションされることを考慮して割込みを禁止してからチェッ
//	 *  クする．
//	 */
//	saved_iipm = current_iipm();
//	set_iipm(IIPM_LOCK);
//	ARM_MEMORY_CHANGED;
//	my_p_tpcb = get_my_p_tpcb();
//	tmp = my_p_tpcb->excpt_nest_count;
//	set_iipm(saved_iipm);
//	ARM_MEMORY_CHANGED;
//
//	return(tmp > 0U);
//}
//
///*
// *  CPUロック状態への移行(USE_GIC_CPULOCK時)
// *
// *  IPM（ハードウェアの割込み優先度マスク）を，saved_iipmに保存し，カー
// *  ネル管理外のものを除くすべての割込みをマスクする値（TIPM_LOCK）に設
// *  定する．また，lock_flagをtrueにする．
// *
// *  IPMが，最初からTIPM_LOCKと同じかそれより高い場合には，それを
// *  saved_iipmに保存するのみで，TIPM_LOCKには設定しない．これはモデル
// *  上の割込み優先度マスクが，TIPM_LOCKと同じかそれより高いレベルに設定
// *  されている状態にあたる．
// *
// *  この関数は，CPUロック状態（lock_flagがtrueの状態）で呼ばれることは
// *  ないものと想定している．
// */
//Inline void
//x_lock_cpu(void)
//{
//	uint8_t iipm;
//
//	/*
//	*  current_iipm()の返り値を直接saved_iipmに保存せず，一時変数iipm
//	*  を用いているのは，current_iipm()を呼んだ直後に割込みが発生し，
//	*  起動された割込み処理でsaved_iipmが変更される可能性があるためで
//	*  ある．
//	*/
//	iipm = current_iipm();
//	set_iipm(IIPM_LOCK);
//	get_my_p_tpcb()->saved_iipm = iipm;
//	get_my_p_tpcb()->lock_flag = true;
//
//	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
//	ARM_MEMORY_CHANGED;
//}
//
//#define t_lock_cpu()    x_lock_cpu()
//#define i_lock_cpu()    x_lock_cpu()
//
///*
// *  CPUロック状態の解除(USE_GIC_CPULOCK時)
// *
// *  lock_flagをfalseにし，IPM（ハードウェアの割込み優先度マスク）を，
// *  saved_iipmに保存した値に戻す．
// *
// *  この関数は，CPUロック状態（lock_flagがtrueの状態）でのみ呼ばれるも
// *  のと想定している．
// */
//Inline void
//x_unlock_cpu(void)
//{
//	/* クリティカルセクションの前後でメモリが書き換わる可能性がある */
//	ARM_MEMORY_CHANGED;
//	get_my_p_tpcb()->lock_flag = false;
//	set_iipm(get_my_p_tpcb()->saved_iipm);
//}
//
//#define t_unlock_cpu()    x_unlock_cpu()
//#define i_unlock_cpu()    x_unlock_cpu()
//
///*
// *  CPUロック状態の参照(USE_GIC_CPULOCK時)
// */
//Inline bool_t
//x_sense_lock(void)
//{
//	return(get_my_p_tpcb()->lock_flag);
//}
//
//#define t_sense_lock()    x_sense_lock()
//#define i_sense_lock()    x_sense_lock()
//
///*
// * （モデル上の）割込み優先度マスクの設定(USE_GIC_CPULOCK時)
// *
// *  CPUロックフラグがクリアされている時は，ハードウェアの割込み優先度マ
// *  スクを設定する．CPUロックフラグがセットされている時は，saved_iipm
// *  を設定し，さらに，ハードウェアの割込み優先度マスクを，設定しようと
// *  した（モデル上の）割込み優先度マスクとTIPM_LOCKの高い方に設定する．
// */
//Inline void
//x_set_ipm(PRI intpri)
//{
//	uint8_t   iipm = INT_IPM(intpri);
//
//	if (!get_my_p_tpcb()->lock_flag) {
//		set_iipm(iipm);
//	}
//	else {
//		get_my_p_tpcb()->saved_iipm = iipm;
//		/*
//		 *  OS内からのみ呼び出されるため，この時点でハードウェアの割込み優先
//		 *  度マスクが必ず最大値に設定されているため，設定する必要はない．
//		 */
//	}
//}
//
//#define t_set_ipm(intpri)    x_set_ipm(intpri)
//#define i_set_ipm(intpri)    x_set_ipm(intpri)
//
///*
// * （モデル上の）割込み優先度マスクの参照(USE_GIC_CPULOCK時)
// *
// *  CPUロックフラグがクリアされている時はハードウェアの割込み優先度マ
// *  スクを，セットされている時はsaved_iipmを参照する．
// */
//Inline PRI
//x_get_ipm(void)
//{
//	uint8_t iipm;
//
//	if (!get_my_p_tpcb()->lock_flag) {
//		iipm = current_iipm();
//	}
//	else {
//		iipm = get_my_p_tpcb()->saved_iipm;
//	}
//	return(EXT_IPM(iipm));
//}
//
//#define t_get_ipm()    x_get_ipm()
//#define i_get_ipm()    x_get_ipm()
//
///*
// *  CPU例外の発生した時の(モデル上の)割込み優先度マスクの参照(USE_GIC_CPULOCK時)
// */
//Inline PRI
//exc_get_ipm(void *p_excinf)
//{
//	if (!((exc_frame_t *)(p_excinf))->lock_flag) {
//		return((PRI)(((exc_frame_t *)(p_excinf))->ipm));
//	}
//	else {
//		return((PRI)(((exc_frame_t *)(p_excinf))->saved_iipm));
//	}
//}
//
///*
// * CPUロック状態か(USE_GIC_CPULOCK時)
// */
//Inline bool_t
//exc_sense_lock(void *p_excinf)
//{
//	return(((exc_frame_t *)(p_excinf))->lock_flag);
//}
//
///*
// * 割込みロック状態か(USE_GIC_CPULOCK時)
// */
//Inline bool_t
//exc_sense_int_lock(void *p_excinf)
//{
//	return(((exc_frame_t *)(p_excinf))->lock_flag);
//}
//
//#endif /* TOPPERS_MACRO_ONLY */
//
//#else /* USE_GIC_CPULOCK */

#ifndef TOPPERS_MACRO_ONLY

/*
 * (モデル上の)割込み優先度マスクの設定
 */
Inline void
x_set_ipm(PRI intpri)
{
	set_iipm(INT_IPM(intpri));
}

#define t_set_ipm(intpri) x_set_ipm(intpri)
#define i_set_ipm(intpri) x_set_ipm(intpri)

/*
 *  (モデル上の)割込み優先度マスクの参照
 */
Inline PRI
x_get_ipm(void)
{
	return(EXT_IPM(current_iipm()));
}

#define t_get_ipm() x_get_ipm()
#define i_get_ipm() x_get_ipm()

#endif /* TOPPERS_MACRO_ONLY */

//#endif /* USE_GIC_CPULOCK */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  割込み属性アクセステーブル
 *
 *  割込み属性テーブルへのポインタ．割込み属性テーブルは，割込み属性が設定
 *  されていれば"1"，設定されていなければ"0"となる
 */
extern const uint8_t* const p_cfgint_table[];

#define my_cfgint_table (p_cfgint_table[x_prc_index()])

/*
 * （モデル上の）割込み要求禁止フラグのセット
 *
 *  指定された，割込み番号の割込み要求禁止フラグのセットして，割込みを
 *  禁止する．
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをクリアしようとした場合には，falseを返す．
 */
Inline bool_t
x_disable_int(INTNO intno)
{
    uint32_t id = INTNO_MASK(intno);

	if (my_cfgint_table[id] == 0){
		return(false);
	}

    if ((0 <= id) && (id < 32)) {
    } else if ((32 <= id) && (id < 64)) {
		sil_wrw_mem((void *)(DISABLE_IRQ_B), (0x1 << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {                   
		sil_wrw_mem((void *)(DISABLE_IRQ_1), (0x1 << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {                  
		sil_wrw_mem((void *)(DISABLE_IRQ_2), (0x1 << (id - 96)));
    }
	return(true);
}

#define t_disable_int(intno)  x_disable_int(intno)
#define i_disable_int(intno)  t_disable_int(intno)

/*
 *  (モデル上の)割り要求禁止フラグの解除
 *
 *  指定された，割込み番号の割込み要求禁止フラグのクリアして，割込みを
 *  許可する．
 *
 *  割込み属性が設定されていない割込み要求ラインに対して割込み要求禁止
 *  フラグをクリアしようとした場合には，falseを返す．
 */
Inline bool_t
x_enable_int(INTNO intno)
{
    uint32_t id = INTNO_MASK(intno);

	if (my_cfgint_table[id] == 0){
		return(false);
	}

    if ((0 <= id) && (id < 32)) {
    } else if ((32 <= id) && (id < 64)) {
		sil_wrw_mem((void *)(ENABLE_IRQ_B), (0x1 << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {
		sil_wrw_mem((void *)(ENABLE_IRQ_1), (0x1 << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {
		sil_wrw_mem((void *)(ENABLE_IRQ_2), (0x1 << (id - 96)));
    }
	return(true);
}

#define t_enable_int(intno) x_enable_int(intno)
#define i_enable_int(intno) x_enable_int(intno)

/*
 * 割込み要求のクリア
 */
Inline void
x_clear_int(INTNO intno)
{
//	gicd_clear_pending(INTNO_MASK(intno));
}

#define t_clear_int(intno) x_clear_int(intno)
#define i_clear_int(intno) x_clear_int(intno)

/*
 *  割込み要求のチェック
 */
Inline bool_t
x_probe_int(INTNO intno)
{
    uint32_t id = INTNO_MASK(intno);
	uintptr_t prc_id = x_prc_index();

	if (0 <= id && id < 32) {
		return(sil_rew_mem((void *)(CORE0_IRQ_SRC + 4 * prc_id)) & (1U << (id - 0)));
    } else if ((32 <= id) && (id < 64)) {
		return(sil_rew_mem((void *)(IRQ_PEND_B)) & (1U << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {
		return(sil_rew_mem((void *)(IRQ_PEND_1)) & (1U << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {
		return(sil_rew_mem((void *)(IRQ_PEND_2)) & (1U << (id - 96)));
	}

	return 0;
}

#define t_probe_int(intno) x_probe_int(intno)
#define i_probe_int(intno) x_probe_int(intno)

/*
 *  割込み要求ラインの属性の設定
 *
 */
extern void x_config_int(INTNO intno, ATR intatr, PRI intpri, uint_t affinity_mask);

/*
 * 割込みハンドラの入り口で必要なIRC操作
 *
 */
Inline void
i_begin_int(INTNO intno)
{
}

/*
 * 割込みハンドラの出口で必要なIRC操作
 */
Inline void
i_end_int(INTNO intno)
{
}

/*
 *  str_ker() の前でマスタプロセッサで行う初期化
 */
extern void chip_mprc_initialize(void);

/*
 *  チップ依存の初期化
 */
extern void chip_initialize(void);

/*
 *  チップ依存部の終了
 *
 *  システムを終了する時に使う．
 */
extern void chip_exit(void);

#endif /* TOPPERS_MACRO_ONLY */

/*
 *  ARM64依存モジュール
 */
#include "arm64_gcc/common/core_config.h"

/*
 *  チップ依存モジュールのツール依存部
 */
#include <chip_config_tool.h>

#endif /* TOPPERS_CHIP_CONFIG_H */
