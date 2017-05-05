/* This file is generated from core_rename.def by genrename. */

#ifndef TOPPERS_CORE_RENAME_H
#define TOPPERS_CORE_RENAME_H

/*
 *  core_config.c
 */
#define core_mprc_initialize		_kernel_core_mprc_initialize
#define core_initialize				_kernel_core_initialize
#define core_exit					_kernel_core_exit
#define default_exc_handler			_kernel_default_exc_handler
#define default_int_handler			_kernel_default_int_handler
#define ipi_ext_handler				_kernel_ipi_ext_handler

/*
 *  kernel_cfg.c
 */
#define p_exch_table				_kernel_p_exch_table

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_config.c
 */
#define _core_mprc_initialize		__kernel_core_mprc_initialize
#define _core_initialize			__kernel_core_initialize
#define _core_exit					__kernel_core_exit
#define _default_exc_handler		__kernel_default_exc_handler
#define _default_int_handler		__kernel_default_int_handler
#define _ipi_ext_handler			__kernel_ipi_ext_handler

/*
 *  kernel_cfg.c
 */
#define _p_exch_table				__kernel_p_exch_table

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
