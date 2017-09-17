/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2005-2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_timer.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  タイマドライバ（ARM64用）
 */

#ifndef TOPPERS_CORE_TIMER_H
#define TOPPERS_CORE_TIMER_H

#include <sil.h>

#ifndef INTPRI_TIMER
#define INTPRI_TIMER    -6            /* 割込み優先度 */
#endif /* INTPRI_TIMER */

#ifndef INTATR_TIMER
#define INTATR_TIMER    0U            /* 割込み属性 */
#endif /* INTATR_TIMER */

#ifndef TOPPERS_MACRO_ONLY

/*
 *  タイマ値の内部表現の型
 */
typedef uint32_t CLOCK;

/*
 *  タイマのクロックを保持する変数
 *  単位はkHz
 *  target_initialize() で初期化される
 */
extern uint32_t timer_clock;

/*
 *  タイマの設定値
 */
extern uint64_t timer_cval;

/*
 *  タイマ値の内部表現とミリ秒・μ秒単位との変換
 */
#define TO_CLOCK(nume, deno) (timer_clock * (nume) / (deno))
#define TO_USEC(clock)       (((SYSUTM) clock) * 1000U / timer_clock)

/*
 *  タイマの現在値を割込み発生前の値とみなすかの判断
 */
#define GET_TOLERANCE    100U    /* 処理遅れの見積り値（単位は内部表現）*/
#define BEFORE_IREQ(clock) \
            ((clock) >= TO_CLOCK(TIC_NUME, TIC_DENO) - GET_TOLERANCE)

/*
 *  タイマの起動処理
 *
 *  タイマを初期化し，周期的なタイマ割込み要求を発生させる．
 */
extern void target_timer_initialize(intptr_t exinf);

/*
 *  タイマの停止処理
 *
 *  タイマの動作を停止させる．
 */
extern void target_timer_terminate(intptr_t exinf);

/*
 *  タイマの現在値の読出し
 */
Inline CLOCK
target_timer_get_current(void)
{
    // TODO : tickタイマハンドラ内でget_timを行うとバグるらしい
    uint64_t pct;
    int64_t cyc;
    int64_t cnt;

    CNTPCT_EL0_READ(pct);
    cyc = (int64_t)TO_CLOCK(TIC_NUME, TIC_DENO);

    cnt = (int64_t)(pct - (timer_cval - cyc));

    /*
     *  ティック割込みを強制的に発生させたり，
     *  ティック割込みが抜けたりした場合の補正．
     *  この補正が行われるときは，システムが破綻しているときである．
     */
    while (cnt >= cyc) {
        cnt = cnt - cyc;
    }
    while (cnt < 0) {
        cnt = cnt + cyc;
    }

    return (CLOCK)cnt;
}

/*
 *  タイマ割込み要求のチェック
 */
Inline bool_t
target_timer_probe_int(void)
{
    // TODO : tickタイマハンドラ内でget_timを行うとバグるらしい
//#if 0
    uint32_t ctl;

//#if defined(TOPPERS_SAFEG_SECURE)
//    CNTPS_CTL_EL1_READ(ctl);
//#else
    CNTP_CTL_EL0_READ(ctl);
//#endif

    return ((ctl & CNTP_CTL_ISTATUS_BIT) == CNTP_CTL_ISTATUS_BIT);
//#else
//    /*
//     * Generic Timerはタイムアウト後はCNTP_CVALを更新するまで，CNTP_TVALの値は0のまま
//     * であるため， target_timer_probe_int()がtrueを返すとget_utm()に不整合が発生するため，
//     * 常にfalseを返すこととする． 
//     */
//    return false;
//#endif /* 0 */
}

/*
 *  タイマ割込みハンドラ
 */
extern void target_timer_handler(void);

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CORE_TIMER_H */
