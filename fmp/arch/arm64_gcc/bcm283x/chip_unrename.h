/* This file is generated from chip_rename.def by genrename. */

/* This file is included only when chip_rename.h has been included. */
#ifdef TOPPERS_CHIP_RENAME_H
#undef TOPPERS_CHIP_RENAME_H

/*
 *  chip_config.c
 */
#undef chip_mprc_initialize
#undef chip_initialize
#undef chip_exit

/*
 *  kernel_cfg.c
 */
#undef p_inh_table
#undef p_cfgint_table
#undef p_inh_iipm_table
#undef p_iipm_mask_table


#ifdef TOPPERS_LABEL_ASM

/*
 *  chip_config.c
 */
#undef _chip_mprc_initialize
#undef _chip_initialize
#undef _chip_exit

/*
 *  kernel_cfg.c
 */
#undef _p_inh_table
#undef _p_cfgint_table
#undef _p_inh_iipm_table
#undef _p_iipm_mask_table


#endif /* TOPPERS_LABEL_ASM */

#include "arm64_gcc/common/core_unrename.h"

#endif /* TOPPERS_CHIP_RENAME_H */
