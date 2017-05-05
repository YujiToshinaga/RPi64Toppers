/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2007-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_asm.inc 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

#ifndef TOPPERS_CHIP_ASM_INC
#define TOPPERS_CHIP_ASM_INC

.macro my_core_index re_reg sc_reg1
	mrs   \re_reg, mpidr_el1
	and	  \re_reg, \re_reg, #MPIDR_AFF0_MASK
.endm

#ifdef USE_THREAD_ID_PCB
.macro my_pcb re_reg sc_reg1
	mrs   \re_reg, tpidr_el1
.endm
#else  /* USE_THREAD_ID_PCB */
.macro my_pcb re_reg sc_reg1
	mrs   \re_reg, mpidr_el1
	and	  \sc_reg1, \re_reg, #MPIDR_AFF0_MASK
	ldr   \re_reg, =p_pcb_table
	ldr   \re_reg, [\re_reg, \sc_reg1, lsl #3]
.endm
#endif /* USE_THREAD_ID_PCB */

.macro my_istkpt re_reg sc_reg1
	mrs   \re_reg, mpidr_el1
	and	  \sc_reg1, \re_reg, #MPIDR_AFF0_MASK
	ldr   \re_reg, =_kernel_istkpt_table
	ldr   \re_reg, [\re_reg, \sc_reg1, lsl #3]
.endm

//#ifdef USE_GIC_CPULOCK
//
//.macro gic_lock reg1, reg2 // reg1 : x, reg2 : w
//	ldr   \reg1, =(GICC_PMR)
//	mov   \reg2, #0
//	str   \reg2, [\reg1]
//	dsb sy
//.endm
//
//.macro gic_unlock reg1, reg2 // reg1 : x, reg2 : w
//	ldr   \reg1, =(GICC_PMR)
//	mov   \reg2, #(IIPM_ENAALL << GICC_PMR_OFFSET)
//	str   \reg2, [\reg1]
//	dsb sy
//.endm
//
//.macro gic_lock_cpu reg1, reg2, reg3 // reg1 : x, reg2 : x, reg3 : w
//	gic_lock \reg1, \reg3
//	my_pcb \reg1, \reg2
//	mov   \reg3, #1
//	str   \reg3, [\reg1, #PCB_lock_flag]
//.endm  
//
//.macro gic_unlock_cpu reg1, reg2, reg3 // reg1 : x, reg2 : x, reg3 : w
//	my_pcb \reg1, \reg2
//	mov   \reg3, #0
//	str   \reg3, [\reg1, #PCB_lock_flag]
//	gic_unlock \reg1, \reg3
//.endm  
//
//#endif /* USE_GIC_CPULOCK */

#include "arm64_gcc/common/core_asm.inc"

#endif /* TOPPERS_CHIP_ASM_INC */
