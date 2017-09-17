/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2006-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: arm64_tool.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  ARM64のハードウェア資源の定義
 */

#ifndef TOPPERS_ARM64_TOOL_H
#define TOPPERS_ARM64_TOOL_H

#define INST_SYNC_BARRIER()         Asm("isb":::"memory")
#define DATA_MEMORY_BARRIER()       Asm("dmb sy":::"memory")
#define DATA_SYNC_BARRIER()         Asm("dsb sy":::"memory")

#define IC_IALLU()                  Asm("ic iallu");
#define IC_IALLUIS()                Asm("ic ialluis");
#define DC_CISW(x)                  Asm("dc cisw, %0"::"r"(x));
#define DC_CSW(x)                   Asm("dc isw, %0"::"r"(x));
#define DC_ISW(x)                   Asm("dc isw, %0"::"r"(x));

#define TLBI_VMALLE1(x)             Asm("tlbi vmalle1");

#define CCSIDR_EL1_WRITE(x)         Asm("msr ccsidr_el1, %0"::"r"(x));
#define CCSIDR_EL1_READ(x)          Asm("mrs %0, ccsidr_el1":"=r"(x));
#define CLIDR_EL1_WRITE(x)          Asm("msr clidr_el1, %0"::"r"(x));
#define CLIDR_EL1_READ(x)           Asm("mrs %0, clidr_el1":"=r"(x));
#define CSSELR_EL1_WRITE(x)         Asm("msr csselr_el1, %0"::"r"(x));
#define CSSELR_EL1_READ(x)          Asm("mrs %0, csselr_el1":"=r"(x));
#define CTR_EL0_WRITE(x)            Asm("msr ctr_el0, %0"::"r"(x));
#define CTR_EL0_READ(x)             Asm("mrs %0, ctr_el0":"=r"(x));
#define ID_AA64MMFR0_EL1_WRITE(x)   Asm("msr id_aa64mmfr0_el1, %0"::"r"(x));
#define ID_AA64MMFR0_EL1_READ(x)    Asm("mrs %0, id_aa64mmfr0_el1":"=r"(x));
#define MPIDR_EL1_WRITE(x)          Asm("msr mpidr_el1, %0"::"r"(x));
#define MPIDR_EL1_READ(x)           Asm("mrs %0, mpidr_el1":"=r"(x));
#define TPIDR_EL1_WRITE(x)          Asm("msr tpidr_el1, %0"::"r"(x));
#define TPIDR_EL1_READ(x)           Asm("mrs %0, tpidr_el1":"=r"(x));

#define TCR_EL1_WRITE(x)            Asm("msr tcr_el1, %0"::"r"(x));
#define TCR_EL1_READ(x)             Asm("mrs %0, tcr_el1":"=r"(x));
#define TTBR0_EL1_WRITE(x)          Asm("msr ttbr0_el1, %0"::"r"(x));
#define TTBR0_EL1_READ(x)           Asm("mrs %0, ttbr0_el1":"=r"(x));
#define MAIR_EL1_WRITE(x)           Asm("msr mair_el1, %0"::"r"(x));
#define MAIR_EL1_READ(x)            Asm("mrs %0, mair_el1":"=r"(x));

#define SCTLR_EL3_WRITE(x)          Asm("msr sctlr_el3, %0"::"r"(x));
#define SCTLR_EL3_READ(x)           Asm("mrs %0, sctlr_el3":"=r"(x));
#define SCTLR_EL2_WRITE(x)          Asm("msr sctlr_el2, %0"::"r"(x));
#define SCTLR_EL2_READ(x)           Asm("mrs %0, sctlr_el2":"=r"(x));
#define SCTLR_EL1_WRITE(x)          Asm("msr sctlr_el1, %0"::"r"(x));
#define SCTLR_EL1_READ(x)           Asm("mrs %0, sctlr_el1":"=r"(x));

#define ACTLR_EL3_WRITE(x)          Asm("msr actlr_el3, %0"::"r"(x));
#define ACTLR_EL3_READ(x)           Asm("mrs %0, actlr_el3":"=r"(x));
#define ACTLR_EL2_WRITE(x)          Asm("msr actlr_el2, %0"::"r"(x));
#define ACTLR_EL2_READ(x)           Asm("mrs %0, actlr_el2":"=r"(x));

#define VBAR_EL3_WRITE(x)           Asm("msr vbar_el3, %0"::"r"(x));
#define VBAR_EL3_READ(x)            Asm("mrs %0, vbar_el3":"=r"(x));
#define VBAR_EL2_WRITE(x)           Asm("msr vbar_el2, %0"::"r"(x));
#define VBAR_EL2_READ(x)            Asm("mrs %0, vbar_el2":"=r"(x));
#define VBAR_EL1_WRITE(x)           Asm("msr vbar_el1, %0"::"r"(x));
#define VBAR_EL1_READ(x)            Asm("mrs %0, vbar_el1":"=r"(x));

#define HCR_EL2_WRITE(x)            Asm("msr hcr_el2, %0"::"r"(x));
#define HCR_EL2_READ(x)             Asm("mrs %0, hcr_el2":"=r"(x));

#define SCR_EL3_WRITE(x)            Asm("msr scr_el3, %0"::"r"(x));
#define SCR_EL3_READ(x)             Asm("mrs %0, scr_el3":"=r"(x));

#define CPUECTLR_EL1_WRITE(x)       Asm("msr s3_1_c15_c2_1, %0"::"r"(x));
#define CPUECTLR_EL1_READ(x)        Asm("mrs %0, s3_1_c15_c2_1":"=r"(x));

#define CNTFRQ_EL0_WRITE(x)         Asm("msr cntfrq_el0, %0"::"r"(x));
#define CNTFRQ_EL0_READ(x)          Asm("mrs %0, cntfrq_el0":"=r"(x));
#define CNTKCTL_EL1_WRITE(x)        Asm("msr cntkctl_el1, %0"::"r"(x));
#define CNTKCTL_EL1_READ(x)         Asm("mrs %0, cntkctl_el1":"=r"(x));
#define CNTHCTL_EL2_WRITE(x)        Asm("msr cnthctl_el2, %0"::"r"(x));
#define CNTHCTL_EL2_READ(x)         Asm("mrs %0, cnthctl_el2":"=r"(x));
#define CNTPCT_EL0_WRITE(x)         Asm("msr cntpct_el0, %0"::"r"(x));
#define CNTPCT_EL0_READ(x)          Asm("mrs %0, cntpct_el0":"=r"(x));
#define CNTVOFF_EL2_WRITE(x)        Asm("msr cntvoff_el2, %0"::"r"(x));
#define CNTVOFF_EL2_READ(x)         Asm("mrs %0, cntvoff_el2":"=r"(x));
#define CNTVCT_EL0_WRITE(x)         Asm("msr cntvct_el0, %0"::"r"(x));
#define CNTVCT_EL0_READ(x)          Asm("mrs %0, cntvct_el0":"=r"(x));
#define CNTP_CTL_EL0_WRITE(x)       Asm("msr cntp_ctl_el0, %0"::"r"(x));
#define CNTP_CTL_EL0_READ(x)        Asm("mrs %0, cntp_ctl_el0":"=r"(x));
#define CNTP_TVAL_EL0_WRITE(x)      Asm("msr cntp_tval_el0, %0"::"r"(x));
#define CNTP_TVAL_EL0_READ(x)       Asm("mrs %0, cntp_tval_el0":"=r"(x));
#define CNTP_CVAL_EL0_WRITE(x)      Asm("msr cntp_cval_el0, %0"::"r"(x));
#define CNTP_CVAL_EL0_READ(x)       Asm("mrs %0, cntp_cval_el0":"=r"(x));
#define CNTPS_CTL_EL1_WRITE(x)      Asm("msr cntps_ctl_el1, %0"::"r"(x));
#define CNTPS_CTL_EL1_READ(x)       Asm("mrs %0, cntps_ctl_el1":"=r"(x));
#define CNTPS_TVAL_EL1_WRITE(x)     Asm("msr cntps_tval_el1, %0"::"r"(x));
#define CNTPS_TVAL_EL1_READ(x)      Asm("mrs %0, cntps_tval_el1":"=r"(x));
#define CNTPS_CVAL_EL1_WRITE(x)     Asm("msr cntps_cval_el1, %0"::"r"(x));
#define CNTPS_CVAL_EL1_READ(x)      Asm("mrs %0, cntps_cval_el1":"=r"(x));
#define CNTV_CTL_EL0_WRITE(x)       Asm("msr cntv_ctl_el0, %0"::"r"(x));
#define CNTV_CTL_EL0_READ(x)        Asm("mrs %0, cntv_ctl_el0":"=r"(x));
#define CNTV_TVAL_EL0_WRITE(x)      Asm("msr cntv_tval_el0, %0"::"r"(x));
#define CNTV_TVAL_EL0_READ(x)       Asm("mrs %0, cntv_tval_el0":"=r"(x));
#define CNTV_CVAL_EL0_WRITE(x)      Asm("msr cntv_cval_el0, %0"::"r"(x));
#define CNTV_CVAL_EL0_READ(x)       Asm("mrs %0, cntv_cval_el0":"=r"(x));

#define PMCCNTR_EL0_WRITE(x)        Asm("msr pmccntr_el0, %0"::"r"(x));
#define PMCCNTR_EL0_READ(x)         Asm("mrs %0, pmccntr_el0":"=r"(x));
#define PMCR_EL0_WRITE(x)           Asm("msr pmcr_el0, %0"::"r"(x));
#define PMCR_EL0_READ(x)            Asm("mrs %0, pmcr_el0":"=r"(x));
#define PMCNTENSET_EL0_WRITE(x)     Asm("msr pmcntenset_el0, %0"::"r"(x));
#define PMCNTENSET_EL0_READ(x)      Asm("mrs %0, pmcntenset_el0":"=r"(x));

#endif /* TOPPERS_ARM64_TOOL_H */
