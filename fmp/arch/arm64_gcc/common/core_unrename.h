/* This file is generated from core_rename.def by genrename. */

/* This file is included only when core_rename.h has been included. */
#ifdef TOPPERS_CORE_RENAME_H
#undef TOPPERS_CORE_RENAME_H

/*
 *  core_config.c
 */
#undef core_mprc_initialize
#undef core_initialize
#undef core_exit
#undef default_exc_handler
#undef default_int_handler
#undef ipi_ext_handler

/*
 *  kernel_cfg.c
 */
#undef p_exch_table

#ifdef TOPPERS_LABEL_ASM

/*
 *  core_config.c
 */
#undef _core_mprc_initialize
#undef _core_initialize
#undef _core_exit
#undef _default_exc_handler
#undef _default_int_handler
#undef _ipi_ext_handler

/*
 *  kernel_cfg.c
 */
#undef _p_exch_table

#endif /* TOPPERS_LABEL_ASM */


#endif /* TOPPERS_CORE_RENAME_H */
