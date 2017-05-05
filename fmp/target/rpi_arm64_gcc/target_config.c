/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2007-2016 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: target_config.c 1148 2016-01-08 08:15:59Z ertl-toshinaga $  
 */

/*
 *  ターゲット依存モジュール（RPI-ARM64用）
 */
#include "kernel_impl.h"
#include <sil.h>
#include "target_serial.h"

//#if defined(TOPPERS_SAFEG_SECURE) | defined(TOPPERS_SAFEG_NONSECURE)
//#include "syscalls_api.h"
//#endif /* defined(TOPPERS_SAFEG_SECURE) | defined(TOPPERS_SAFEG_NONSECURE) */

/*
 *  entry point (start.S)
 */
extern void start(void);

#ifdef INIT_MONITOR
/*
 *  EL3で行う初期化処理
 */
void
target_mon_initialize(void)
{
	uint32_t freqency;
	uint64_t tmp;

	/*
	 *  CPUECTLR_EL1のアクセス許可
	 */
    ACTLR_EL3_READ(tmp);
    tmp |= ACTLR_CPUEC_AC_BIT;
    ACTLR_EL3_WRITE(tmp);

	/*
	 *  Generic Timerの初期化
	 */
//	freqency = sil_rew_mem((void *)(CNTFID0));
	freqency = 1000000;
	CNTFRQ_EL0_WRITE(freqency);

	inst_sync_barrier();
}
#endif /* INIT_MONITOR */

#ifdef INIT_HYPERVISOR
/*
 *  EL2で行う初期化処理
 */
void
target_hyp_initialize(void)
{
    uint64_t tmp;

	/*
	 *  CPUECTLR_EL1のアクセス許可
	 */
    ACTLR_EL2_READ(tmp);
    tmp |= ACTLR_CPUEC_AC_BIT;
    ACTLR_EL2_WRITE(tmp);

	/*
	 *  Generic Timerの初期化
	 */
	/* Physical Counter, Physical TimerをEL1NSとEL0NSからアクセス可能に */
	CNTHCTL_EL2_WRITE(CNTHCTL_EL1PCEN_BIT | CNTHCTL_EL1PCTEN_BIT);

	/* Virtual Counterのオフセットを0に */
	CNTVOFF_EL2_WRITE(0);

	inst_sync_barrier();
}
#endif /* INIT_HYPERVISOR */

/*
 *  str_ker() の前でマスタプロセッサで行う初期化
 */
void
target_mprc_initialize(void)
{
    uint32_t tmp;

    /*
     *  GPIOをUARTに切り替え
     */
	tmp = sil_rew_mem((void *)GPIO_GPFSEL1);
	tmp &= ~(7 << 12);	// GPIO14 ALT5
	tmp |= 2 << 12;
	tmp &= ~(7 << 15);	// GPIO15 ALT5
	tmp |= 2 << 15;
	sil_wrw_mem((void *)GPIO_GPFSEL1, tmp);

	sil_wrw_mem((void *)GPIO_GPPUD, 0);
	for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
	sil_wrw_mem((void *)GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
	for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
	sil_wrw_mem((void *)GPIO_GPPUDCLK0, 0);

	chip_mprc_initialize();

//#ifdef TOPPERS_SAFEG_SECURE
//#endif /* defined(TOPPERS_SAFEG_SECURE) */
}

/*
 *  メモリマップの初期化（core依存部から呼び出される）
 *  同時にキャッシュも有効にする
 */
void
target_mmu_init(void)
{
	mmap_t mm;

	/*
	 *  メモリマップの設定
	 *  全領域は物理アドレス = 仮想アドレス
	 */
//#if defined(TOPPERS_SAFEG_SECURE)
//#elif defined(TOPPERS_SAFEG_NONSECURE)
//#else /* defined(TOPPERS_SAFEG_SECURE) */
	mm.pa   = SDRAM_ADDR;
	mm.va   = mm.pa;
	mm.size = SDRAM_SIZE;
	mm.attr	= MEM_ATTR_NML_C;
	mm.ns	= MEM_NS_NONSECURE;
	mmu_mmap_add(&mm);

	mm.pa   = IO0_ADDR;
	mm.va   = mm.pa;
	mm.size = IO0_SIZE;
	mm.attr	= MEM_ATTR_DEV;
	mm.ns	= MEM_NS_NONSECURE;
	mmu_mmap_add(&mm);

	mm.pa   = IO1_ADDR;
	mm.va   = mm.pa;
	mm.size = IO1_SIZE;
	mm.attr	= MEM_ATTR_DEV;
	mm.ns	= MEM_NS_NONSECURE;
	mmu_mmap_add(&mm);
//#endif /* defined(TOPPERS_SAFEG_SECURE) */

//#ifdef TOPPERS_SAFEG_COM
//#endif /* TOPPERS_SAFEG_COM */
}

/*
 *  ターゲット依存の初期化
 */
void
target_initialize(void)
{
    uint64_t tmp;

	/*
	 *  SMPモードを有効化
     *  これを行わないとコア間でデータキャッシュのコヒーレンシが保てない
	 */
    CPUECTLR_EL1_READ(tmp);
    tmp |= CPUECTLR_SMPEN_BIT;
    CPUECTLR_EL1_WRITE(tmp);

	/*
	 *  バナー表示，低レベル出力用にUARTを初期化
	 */
#ifdef G_SYSLOG
	if (x_sense_mprc()) {
		mu_reset(1);
	}
#else /* G_SYSLOG */
	mu_reset(x_prc_index() + 1);
#endif /* G_SYSLOG */

	/*
	 *  チップ依存の初期化
	 */
	chip_initialize();
}

/*
 *  ターゲット依存の終了処理
 */
void
target_exit(void)
{
	/*
	 *  チップ依存の終了処理
	 */
	chip_exit();

	while(1);
}

//#ifdef TOPPERS_SAFEG_SECURE
//uint64_t
//target_get_nt_start_address(void) {
//}
//#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  システムログの低レベル出力のための文字出力
 */
void
target_fput_log(char c)
{
	if (c == '\n') {
		mu_pol_putc(1, '\r');
	}
	mu_pol_putc(1, c);
}
