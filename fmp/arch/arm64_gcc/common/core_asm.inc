/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2011 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: core_asm.inc 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

#ifndef TOPPERS_CORE_ASM_INC
#define TOPPERS_CORE_ASM_INC

/*
 *  ディレクティブのマクロ定義
 */
#define ASECTION(name) .section name,"a"
#define ATEXT          .text
#define AGLOBAL(name)  .global name
#define AALIGN(x)      .align x
#define ABALIGN(x)     .balign x
//#define ALONG          .long /* 現在のコンパイラでは.long = .intになる */
#define ALONG          .dword /* 8byte値を配置したいなら.dwordを使用する */
#define ALABEL(label)  label:
#define AOR            |
#define AWEAK(name)    .weak name

/*
 *  セクション情報
 */
#define DATA_START  __data_start
#define IDATA_START __idata_start
#define IDATA_END   __idata_end
#define BSS_START   __bss_start
#define BSS_END     __bss_end

#ifndef TOPPRS_NONUSE_CORE_DEFAULT_CUSTOM_IDLE
.macro toppers_asm_custom_idle
//#ifndef USE_GIC_CPULOCK
#ifndef TOPPERS_ENABLE_QEMU
    wfi
#endif /* TOPPERS_ENABLE_QEMU */
    mov   x0, #(DAIF_ALWAYS_SET) /* 割り込み許可 */
    msr   daif, x0
    isb                         /* シミュレータ環境ではISBを入れないと */
                                /* 割込み許可区間が生まれない */
    mov   x0, #(DAIF_CPULOCK AOR DAIF_ALWAYS_SET) /* CPUロック状態へ */
    msr   daif, x0
//#else /* USE_GIC_CPULOCK */
//    gic_unlock_cpu x23, x24, w25
//    gic_lock_cpu x23, x24, w25
//#endif /* USE_GIC_CPULOCK */
.endm
#endif /* TOPPRS_NONUSE_CORE_DEFAULT_CUSTOM_IDLE */

#endif /* TOPPERS_CORE_ASM_INC */
