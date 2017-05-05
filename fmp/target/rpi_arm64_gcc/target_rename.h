/* This file is generated from target_rename.def by genrename. */

#ifndef TOPPERS_TARGET_RENAME_H
#define TOPPERS_TARGET_RENAME_H

/*
 *  start.S
 */
#define start_sync					_kernel_start_sync

/*
 *  target_config.c
 */
#define target_mon_initialize		_kernel_target_mon_initialize
#define target_hyp_initialize		_kernel_target_hyp_initialize
#define target_mprc_initialize		_kernel_target_mprc_initialize
#define target_mmu_init				_kernel_target_mmu_init
#define target_initialize			_kernel_target_initialize
#define target_exit					_kernel_target_exit


#ifdef TOPPERS_LABEL_ASM

/*
 *  start.S
 */
#define _start_sync					__kernel_start_sync

/*
 *  target_config.c
 */
#define _target_mon_initialize		__kernel_target_mon_initialize
#define _target_hyp_initialize		__kernel_target_hyp_initialize
#define _target_mprc_initialize		__kernel_target_mprc_initialize
#define _target_mmu_init			__kernel_target_mmu_init
#define _target_initialize			__kernel_target_initialize
#define _target_exit				__kernel_target_exit


#endif /* TOPPERS_LABEL_ASM */

#include "arm64_gcc/bcm283x/chip_rename.h"

#endif /* TOPPERS_TARGET_RENAME_H */
