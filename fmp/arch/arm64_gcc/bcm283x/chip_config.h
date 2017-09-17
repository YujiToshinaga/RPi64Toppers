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

#define VALID_INTNO_DISINT(prcid, intno)    VALID_INTNO(prcid, intno)
#define VALID_INTNO_CFGINT(prcid, intno)    VALID_INTNO(prcid, intno)

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
 *  割込み優先度マスク操作ライブラリ
 */

/*
 *  割込み優先度マスクの外部表現と内部表現の変換
 *
 *  アセンブリ言語のソースファイルからインクルードする場合のために，型
 *  キャストしない定義も用意している．
 */
#ifndef TOPPERS_MACRO_ONLY
#define EXT_IPM(iipm)   ((PRI)(iipm))       /* 内部表現を外部表現に */
#define INT_IPM(ipm)    ((uint8_t)(ipm))    /* 外部表現を内部表現に */
#else /* TOPPERS_MACRO_ONLY */
#define EXT_IPM(iipm)   (iipm)              /* 内部表現を外部表現に */
#define INT_IPM(ipm)    (ipm)               /* 外部表現を内部表現に */
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
    TPCB *p_tpcb = get_my_p_tpcb();
    return(p_tpcb->iipm);
}

/*
 *  IPM（ハードウェアの割込み優先度マスク，内部表現）の現在値の設定
 */
Inline void
set_iipm(uint8_t iipm)
{
    TPCB *p_tpcb = get_my_p_tpcb();
    p_tpcb->iipm = iipm;
}

#endif /* TOPPERS_MACRO_ONLY */

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
        // 0-31の割込みはNMIとしている
    } else if ((32 <= id) && (id < 64)) {
        sil_wrw_mem((uint32_t *)(DISABLE_IRQ_B), (0x1 << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {                   
        sil_wrw_mem((uint32_t *)(DISABLE_IRQ_1), (0x1 << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {                  
        sil_wrw_mem((uint32_t *)(DISABLE_IRQ_2), (0x1 << (id - 96)));
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
        // 0-31の割込みはNMIとしている
    } else if ((32 <= id) && (id < 64)) {
        sil_wrw_mem((uint32_t *)(ENABLE_IRQ_B), (0x1 << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {
        sil_wrw_mem((uint32_t *)(ENABLE_IRQ_1), (0x1 << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {
        sil_wrw_mem((uint32_t *)(ENABLE_IRQ_2), (0x1 << (id - 96)));
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
    // MEMO : 共通の割込み要求クリアの仕組みはない
//    gicd_clear_pending(INTNO_MASK(intno));
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
        return(sil_rew_mem((uint32_t *)(CORE0_IRQ_SRC + 4 * prc_id)) &
                (1U << (id - 0)));
    } else if ((32 <= id) && (id < 64)) {
        return(sil_rew_mem((uint32_t *)(IRQ_PEND_B)) & (1U << (id - 32)));
    } else if ((64 <= id) && (id < 96)) {
        return(sil_rew_mem((uint32_t *)(IRQ_PEND_1)) & (1U << (id - 64)));
    } else if ((96 <= id) && (id < 128)) {
        return(sil_rew_mem((uint32_t *)(IRQ_PEND_2)) & (1U << (id - 96)));
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
