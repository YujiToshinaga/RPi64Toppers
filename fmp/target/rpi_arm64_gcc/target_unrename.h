/* This file is generated from target_rename.def by genrename. */

/* This file is included only when target_rename.h has been included. */
#ifdef TOPPERS_TARGET_RENAME_H
#undef TOPPERS_TARGET_RENAME_H

/*
 *  start.S
 */
#undef start_sync

/*
 *  target_config.c
 */
#undef target_mon_initialize
#undef target_hyp_initialize
#undef target_mprc_initialize
#undef target_mmu_init
#undef target_initialize
#undef target_exit


#ifdef TOPPERS_LABEL_ASM

/*
 *  start.S
 */
#undef _start_sync

/*
 *  target_config.c
 */
#undef _target_mon_initialize
#undef _target_hyp_initialize
#undef _target_mprc_initialize
#undef _target_mmu_init
#undef _target_initialize
#undef _target_exit


#endif /* TOPPERS_LABEL_ASM */

#include "arm64_gcc/bcm283x/chip_unrename.h"

#endif /* TOPPERS_TARGET_RENAME_H */
