/* This file is generated from chip_rename.def by genrename. */

#ifndef TOPPERS_CHIP_RENAME_H
#define TOPPERS_CHIP_RENAME_H

/*
 *  chip_config.c
 */
#define chip_mprc_initialize		_kernel_chip_mprc_initialize
#define chip_initialize				_kernel_chip_initialize
#define chip_exit					_kernel_chip_exit

/*
 *  kernel_cfg.c
 */
#define p_inh_table					_kernel_p_inh_table
#define p_cfgint_table				_kernel_p_cfgint_table
#define p_inh_iipm_table			_kernel_p_inh_iipm_table
#define p_iipm_mask_table			_kernel_p_iipm_mask_table


#ifdef TOPPERS_LABEL_ASM

/*
 *  chip_config.c
 */
#define _chip_mprc_initialize		__kernel_chip_mprc_initialize
#define _chip_initialize			__kernel_chip_initialize
#define _chip_exit					__kernel_chip_exit

/*
 *  kernel_cfg.c
 */
#define _p_inh_table				__kernel_p_inh_table
#define _p_cfgint_table				__kernel_p_cfgint_table
#define _p_inh_iipm_table			__kernel_p_inh_iipm_table
#define _p_iipm_mask_table			__kernel_p_iipm_mask_table


#endif /* TOPPERS_LABEL_ASM */

#include "arm64_gcc/common/core_rename.h"

#endif /* TOPPERS_CHIP_RENAME_H */
