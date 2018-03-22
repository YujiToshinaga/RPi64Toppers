#ifndef AARCH64_H
#define AARCH64_H

/*
 *  CPSRの構成
 */
#define DAIF_MASK               (0x3c0)
#define DAIF_D_BIT              (0x200)
#define DAIF_A_BIT              (0x100)
#define DAIF_I_BIT              (0x080)
#define DAIF_F_BIT              (0x040)

#define CURRENTEL_EL_MASK       (0x3 << 2)
#define CURRENTEL_EL_EL0        (0x0 << 2)
#define CURRENTEL_EL_EL1        (0x1 << 2)
#define CURRENTEL_EL_EL2        (0x2 << 2)
#define CURRENTEL_EL_EL3        (0x3 << 2)

#define SPSEL_SP_MASK           (0x1 << 0)
#define SPSEL_SP_EL0            (0x0 << 0)
#define SPSEL_SP_ELX            (0x1 << 0)

#define MODE_MASK               (0x1f << 0)
#define MODE_AARCH64_EL0T       (0x00 << 0)
#define MODE_AARCH64_EL1T       (0x04 << 0)
#define MODE_AARCH64_EL1H       (0x05 << 0)
#define MODE_AARCH64_EL2T       (0x08 << 0)
#define MODE_AARCH64_EL2H       (0x09 << 0)
#define MODE_AARCH64_EL3T       (0x0c << 0)
#define MODE_AARCH64_EL3H       (0x0d << 0)

/*
 *  System instruction
 */
#define DC_SETWAY_SHIFT         4
#define DC_SETWAY_MASK          (0x7ffffff << 4)
#define DC_LEVEL_SHIFT          1
#define DC_LEVEL_MASK           (0x7 << 1)

/*
 *  General System Control Register
 */
#define MPIDR_AFF3_MASK             0x000000ff00000000
#define MPIDR_AFF2_MASK             0x0000000000ff0000
#define MPIDR_AFF1_MASK             0x000000000000ff00
#define MPIDR_AFF0_MASK             0x00000000000000ff
#define MPIDR_AFF_ALL_MASK          0x000000ff00ffffff
#define MPIDR_AFF_MASK              0xff
#define MPIDR_AFF_SHIFT             8

#define SCTLR_EE_BIT                (1 << 25)
#define SCTLR_WXN_BIT               (1 << 19)
#define SCTLR_I_BIT                 (1 << 12)
#define SCTLR_SA_BIT                (1 << 3)
#define SCTLR_C_BIT                 (1 << 2)
#define SCTLR_A_BIT                 (1 << 1)
#define SCTLR_M_BIT                 (1 << 0)

#define ACTLR_CPUEC_AC_BIT          (1 << 1)

#define SCR_TWE_BIT                 (1 << 13)
#define SCR_TWI_BIT                 (1 << 12)
#define SCR_ST_BIT                  (1 << 11)
#define SCR_RW_BIT                  (1 << 10)
#define SCR_SIF_BIT                 (1 << 9)
#define SCR_HCE_BIT                 (1 << 8)
#define SCR_SMD_BIT                 (1 << 7)
#define SCR_EA_BIT                  (1 << 3)
#define SCR_FIQ_BIT                 (1 << 2)
#define SCR_IRQ_BIT                 (1 << 1)
#define SCR_NS_BIT                  (1 << 0)

#define HCR_RW_BIT                  (1 << 31)
#define HCR_AMO_BIT                 (1 << 5)
#define HCR_IMO_BIT                 (1 << 4)
#define HCR_FMO_BIT                 (1 << 3)

#define TCR_IPS_SHIFT               32
#define TCR_IPS_MASK                (0x7ull << 32)
#define TCR_IPS_4GB                 (0x0ull << 32)
#define TCR_IPS_64GB                (0x1ull << 32)
#define TCR_IPS_1TB                 (0x2ull << 32)
#define TCR_IPS_4TB                 (0x3ull << 32)
#define TCR_IPS_16TB                (0x4ull << 32)
#define TCR_IPS_256TB               (0x5ull << 32)
#define TCR_EPD1_BIT                (1 << 23)
#define TCR_TG0_SHIFT               14
#define TCR_TG0_MASK                (0x3 << 14)
#define TCR_TG0_4KB                 (0x0 << 14)
#define TCR_TG0_64KB                (0x1 << 14)
#define TCR_TG0_16KB                (0x2 << 14)
#define TCR_SH0_SHIFT               12
#define TCR_SH0_MASK                (0x3 << 12)
#define TCR_SH0_NON_SHAREABLE       (0x0 << 12)
#define TCR_SH0_OUTER_SHAREABLE     (0x2 << 12)
#define TCR_SH0_INNER_SHAREABLE     (0x3 << 12)
#define TCR_ORGN0_SHIFT             10
#define TCR_ORGN0_MASK              (0x3 << 10)
#define TCR_ORGN0_NC                (0x0 << 10)
#define TCR_ORGN0_WBWAC             (0x1 << 10)
#define TCR_ORGN0_WTC               (0x2 << 10)
#define TCR_ORGN0_WBNWAC            (0x3 << 10)
#define TCR_IRGN0_SHIFT             8
#define TCR_IRGN0_MASK              (0x3 << 8)
#define TCR_IRGN0_NC                (0x0 << 8)
#define TCR_IRGN0_WBWAC             (0x1 << 8)
#define TCR_IRGN0_WTC               (0x2 << 8)
#define TCR_IRGN0_WBNWAC            (0x3 << 8)
#define TCR_EPD0_BIT                (1 << 7)
#define TCR_T0SZ_SHIFT              0
#define TCR_T0SZ_MASK               (0x3f << 0)

#define MAIR_ATTR_SHIFT             8
#define MAIR_ATTR_MASK              0xff
#define MAIR_ATTR_SO                0x00
#define MAIR_ATTR_DEV               0x04
#define MAIR_ATTR_NML_NC            0x44
#define MAIR_ATTR_NML_C             0xff

#define CLIDR_LOC_SHIFT             24
#define CLIDR_LOC_MASK              (0x7 << 24)
#define CLIDR_CTYPE_SHIFT           0x3
#define CLIDR_CTYPE_MASK            0x7
#define CLIDR_CTYPE_NO              0x0
#define CLIDR_CTYPE_INST            0x1
#define CLIDR_CTYPE_DATA            0x2
#define CLIDR_CTYPE_SEPARATE        0x3
#define CLIDR_CTYPE_UNIFIED         0x4

#define CSSELR_LEVEL_SHIFT          1
#define CSSELR_LEVEL_MASK           (0x7 << 1)
#define CSSELR_IND_BIT              (1 << 0)

#define CCSIDR_NUMSETS_SHIFT        13
#define CCSIDR_NUMSETS_MASK         (0x7fff << 13)
#define CCSIDR_ASSOCIATIVITY_SHIFT  3
#define CCSIDR_ASSOCIATIVITY_MASK   (0x3ff << 3)
#define CCSIDR_LINESIZE_SHIFT       0
#define CCSIDR_LINESIZE_MASK        (0x7 << 0)

#define CPUECTLR_SMPEN_BIT          (1 << 6)

/*
 *  Generic Timer Register
 */
#define CNTHCTL_EL1PCEN_BIT     (1 << 1)
#define CNTHCTL_EL1PCTEN_BIT    (1 << 0)
#define CNTKCTL_EL0PTEN_BIT     (1 << 9)
#define CNTKCTL_EL0VTEN_BIT     (1 << 8)
#define CNTKCTL_EL0PCTEN_BIT    (1 << 0)
#define CNTKCTL_EL0VCTEN_BIT    (1 << 1)
#define CNTP_CTL_ISTATUS_BIT    (1 << 2)
#define CNTP_CTL_IMASK_BIT      (1 << 1)
#define CNTP_CTL_ENABLE_BIT     (1 << 0)
#define CNTPS_CTL_ISTATUS_BIT   (1 << 2)
#define CNTPS_CTL_IMASK_BIT     (1 << 1)
#define CNTPS_CTL_ENABLE_BIT    (1 << 0)
#define CNTV_CTL_ISTATUS_BIT    (1 << 2)
#define CNTV_CTL_IMASK_BIT      (1 << 1)
#define CNTV_CTL_ENABLE_BIT     (1 << 0)

/*
 *  Performance Monitor Register
 */
#define PMCR_LC_BIT             (1 << 6)
#define PMCR_C_BIT              (1 << 2)
#define PMCR_E_BIT              (1 << 0)
#define PMCNTENSET_C_BIT        (1 << 31)

/*
 *  Memory-mapped System Register
 */
#define CNTCR                   (CNTCONTROL_BASE + 0x000)
#define CNTFID0                 (CNTCONTROL_BASE + 0x020)
#define CNTPCT0                 (CNTBASE0 + 0x000)
#define CNTFRQ0                 (CNTBASE0 + 0x010)
#define CNTP_CVAL0              (CNTBASE0 + 0x020)
#define CNTP_TVAL0              (CNTBASE0 + 0x028)
#define CNTP_CTL0               (CNTBASE0 + 0x02C)
#define CNTPCT1                 (CNTBASE1 + 0x000)
#define CNTFRQ1                 (CNTBASE1 + 0x010)
#define CNTP_CVAL1              (CNTBASE1 + 0x020)
#define CNTP_TVAL1              (CNTBASE1 + 0x028)
#define CNTP_CTL1               (CNTBASE1 + 0x02C)
#define CNTFRQ                  (CNTCTLBASE + 0x000)
#define CNTNSAR                 (CNTCTLBASE + 0x004)
#define CNTACR0                 (CNTCTLBASE + 0x040)
#define CNTACR1                 (CNTCTLBASE + 0x044)

#define CNTCR_FCREQ_SHIFT       8
#define CNTCR_EN_BIT            (1 << 0)
#define CNTNSAR_NS1_BIT         (1 << 1)
#define CNTNSAR_NS0_BIT         (1 << 0)
#define CNTACR_RWPT_BIT         (1 << 5)
#define CNTACR_RWVT_BIT         (1 << 4)
#define CNTACR_RVOFF_BIT        (1 << 3)
#define CNTACR_RFRQ_BIT         (1 << 2)
#define CNTACR_RVCT_BIT         (1 << 1)
#define CNTACR_RPCT_BIT         (1 << 0)

/*
 *  Translation table
 */
#define ADDR_SPACE_WIDTH        32
#define ADDR_SPACE_SIZE         (1ull << ADDR_SPACE_WIDTH)
#define ADDR_SPACE_MASK         (ADDR_SPACE_SIZE - 1)

#define TT_GRANULE_WIDTH        12
#define TT_GRANULE_SIZE         (1 << TT_GRANULE_WIDTH)

#define TT_ENTRY_WIDTH          3 /* 1 entry = 8bytes (64bit) */
#define TT_ENTRY_SIZE           (1 << TT_ENTRY_WIDTH)
#define TT_PAGE_SIZE            (1 << TT_GRANULE_WIDTH)
#define TT_OFFSET_MASK          ((1 << TT_GRANULE_WIDTH) - 1)
#define TT_ENTRY_NUM            (1 << TT_LEVEL_WIDTH)
#define TT_SIZE                 (TT_ENTRY_SIZE * TT_ENTRY_NUM)

#define TT_LEVEL_WIDTH          (TT_GRANULE_WIDTH - TT_ENTRY_WIDTH)
#define TT_LEVEL_MASK           ((1 << TT_LEVEL_WIDTH) - 1)

#define TT_L3_SHIFT             TT_GRANULE_WIDTH
#define TT_L2_SHIFT             (TT_L3_SHIFT + TT_LEVEL_WIDTH)
#define TT_L1_SHIFT             (TT_L2_SHIFT + TT_LEVEL_WIDTH)
#define TT_L0_SHIFT             (TT_L1_SHIFT + TT_LEVEL_WIDTH)

#if (ADDR_SPACE_WIDTH > TT_L0_SHIFT)
#define TT_FIRST_ENTRY_NUM      (1 << (ADDR_SPACE_WIDTH - TT_L0_SHIFT))
#define TT_FIRST_LEVEL          0
#elif (ADDR_SPACE_MASK > TT_L1_SHIFT)
#define TT_FIRST_ENTRY_NUM      (1 << (ADDR_SPACE_WIDTH - TT_L1_SHIFT))
#define TT_FIRST_LEVEL          1
#elif (ADDR_SPACE_MASK > TT_L2_SHIFT)
#define TT_FIRST_ENTRY_NUM      (1 << (ADDR_SPACE_WIDTH - TT_L2_SHIFT))
#define TT_FIRST_LEVEL          2
#elif (ADDR_SPACE_MASK > TT_L3_SHIFT)
#define TT_FIRST_ENTRY_NUM      (1 << (ADDR_SPACE_WIDTH - TT_L3_SHIFT))
#define TT_FIRST_LEVEL          3
#endif
#define TT_FIRST_SIZE           (TT_ENTRY_SIZE * TT_FIRST_ENTRY_NUM)
#define TT_LAST_LEVEL           3

#define TT_UXN_BIT              (1 << 54)
#define TT_PXN_BIT              (1 << 53)
#define TT_CONTIGUOUS_BIT       (1 << 52)
#define TT_NG_BIT               (1 << 11)
#define TT_AF_BIT               (1 << 10)
#define TT_SH_SHIFT             8
#define TT_SH_MASK              (0x3 << 8)
#define TT_SH_NSH               (0x0 << 8)
#define TT_SH_OSH               (0x2 << 8)
#define TT_SH_ISH               (0x3 << 8)
#define TT_AP_SHIFT             6
#define TT_AP_MASK              (0x3 << 6)
#define TT_AP_EL1RW_EL0NO       (0x0 << 6)
#define TT_AP_EL1RW_EL0RW       (0x1 << 6)
#define TT_AP_EL1RO_EL0NO       (0x2 << 6)
#define TT_AP_EL1RO_EL0RO       (0x3 << 6)
#define TT_NS_BIT               (1 << 5)
#define TT_ATTRINDX_SHIFT       2
#define TT_ATTRINDX_MASK        (0x7 << 2)

#define TT_DESC_SHIFT           0
#define TT_DESC_MASK            (0x3 << 0)
#define TT_DESC_INVALID         (0x0 << 0)
#define TT_DESC_BLOCK           (0x1 << 0)
#define TT_DESC_TABLE           (0x3 << 0)
#define TT_DESC_PAGE            (0x3 << 0)

#define MEM_ATTR_SO                 0
#define MEM_ATTR_DEV                1
#define MEM_ATTR_NML_NC             2
#define MEM_ATTR_NML_C              3
#define MEM_ATTR_SO_ATTRINDX        0
#define MEM_ATTR_DEV_ATTRINDX       1
#define MEM_ATTR_NML_NC_ATTRINDX    2
#define MEM_ATTR_NML_C_ATTRINDX     3
#define MEM_NS_SECURE               0
#define MEM_NS_NONSECURE            1

#ifndef MACRO_ONLY

#define ISB()                       asm volatile("isb":::"memory")
#define DMB()                       asm volatile("dmb sy":::"memory")
#define DSB()                       asm volatile("dsb sy":::"memory")

#define IC_IALLU()                  asm volatile("ic iallu");
#define IC_IALLUIS()                asm volatile("ic ialluis");
#define DC_CISW(x)                  asm volatile("dc cisw, %0"::"r"(x));
#define DC_CSW(x)                   asm volatile("dc isw, %0"::"r"(x));
#define DC_ISW(x)                   asm volatile("dc isw, %0"::"r"(x));

#define TLBI_VMALLE1(x)             asm volatile("tlbi vmalle1");

#define CCSIDR_EL1_WRITE(x)         asm volatile("msr ccsidr_el1, %0"::"r"(x));
#define CCSIDR_EL1_READ(x)          asm volatile("mrs %0, ccsidr_el1":"=r"(x));
#define CLIDR_EL1_WRITE(x)          asm volatile("msr clidr_el1, %0"::"r"(x));
#define CLIDR_EL1_READ(x)           asm volatile("mrs %0, clidr_el1":"=r"(x));
#define CSSELR_EL1_WRITE(x)         asm volatile("msr csselr_el1, %0"::"r"(x));
#define CSSELR_EL1_READ(x)          asm volatile("mrs %0, csselr_el1":"=r"(x));
#define CTR_EL0_WRITE(x)            asm volatile("msr ctr_el0, %0"::"r"(x));
#define CTR_EL0_READ(x)             asm volatile("mrs %0, ctr_el0":"=r"(x));
#define ID_AA64MMFR0_EL1_WRITE(x)   asm volatile("msr id_aa64mmfr0_el1, %0"::"r"(x));
#define ID_AA64MMFR0_EL1_READ(x)    asm volatile("mrs %0, id_aa64mmfr0_el1":"=r"(x));
#define MPIDR_EL1_WRITE(x)          asm volatile("msr mpidr_el1, %0"::"r"(x));
#define MPIDR_EL1_READ(x)           asm volatile("mrs %0, mpidr_el1":"=r"(x));
#define TPIDR_EL1_WRITE(x)          asm volatile("msr tpidr_el1, %0"::"r"(x));
#define TPIDR_EL1_READ(x)           asm volatile("mrs %0, tpidr_el1":"=r"(x));

#define TCR_EL1_WRITE(x)            asm volatile("msr tcr_el1, %0"::"r"(x));
#define TCR_EL1_READ(x)             asm volatile("mrs %0, tcr_el1":"=r"(x));
#define TTBR0_EL1_WRITE(x)          asm volatile("msr ttbr0_el1, %0"::"r"(x));
#define TTBR0_EL1_READ(x)           asm volatile("mrs %0, ttbr0_el1":"=r"(x));
#define MAIR_EL1_WRITE(x)           asm volatile("msr mair_el1, %0"::"r"(x));
#define MAIR_EL1_READ(x)            asm volatile("mrs %0, mair_el1":"=r"(x));

#define SCTLR_EL3_WRITE(x)          asm volatile("msr sctlr_el3, %0"::"r"(x));
#define SCTLR_EL3_READ(x)           asm volatile("mrs %0, sctlr_el3":"=r"(x));
#define SCTLR_EL2_WRITE(x)          asm volatile("msr sctlr_el2, %0"::"r"(x));
#define SCTLR_EL2_READ(x)           asm volatile("mrs %0, sctlr_el2":"=r"(x));
#define SCTLR_EL1_WRITE(x)          asm volatile("msr sctlr_el1, %0"::"r"(x));
#define SCTLR_EL1_READ(x)           asm volatile("mrs %0, sctlr_el1":"=r"(x));

#define ACTLR_EL3_WRITE(x)          asm volatile("msr actlr_el3, %0"::"r"(x));
#define ACTLR_EL3_READ(x)           asm volatile("mrs %0, actlr_el3":"=r"(x));
#define ACTLR_EL2_WRITE(x)          asm volatile("msr actlr_el2, %0"::"r"(x));
#define ACTLR_EL2_READ(x)           asm volatile("mrs %0, actlr_el2":"=r"(x));

#define VBAR_EL3_WRITE(x)           asm volatile("msr vbar_el3, %0"::"r"(x));
#define VBAR_EL3_READ(x)            asm volatile("mrs %0, vbar_el3":"=r"(x));
#define VBAR_EL2_WRITE(x)           asm volatile("msr vbar_el2, %0"::"r"(x));
#define VBAR_EL2_READ(x)            asm volatile("mrs %0, vbar_el2":"=r"(x));
#define VBAR_EL1_WRITE(x)           asm volatile("msr vbar_el1, %0"::"r"(x));
#define VBAR_EL1_READ(x)            asm volatile("mrs %0, vbar_el1":"=r"(x));

#define HCR_EL2_WRITE(x)            asm volatile("msr hcr_el2, %0"::"r"(x));
#define HCR_EL2_READ(x)             asm volatile("mrs %0, hcr_el2":"=r"(x));

#define SCR_EL3_WRITE(x)            asm volatile("msr scr_el3, %0"::"r"(x));
#define SCR_EL3_READ(x)             asm volatile("mrs %0, scr_el3":"=r"(x));

#define CPUECTLR_EL1_WRITE(x)       asm volatile("msr s3_1_c15_c2_1, %0"::"r"(x));
#define CPUECTLR_EL1_READ(x)        asm volatile("mrs %0, s3_1_c15_c2_1":"=r"(x));

#define CNTFRQ_EL0_WRITE(x)         asm volatile("msr cntfrq_el0, %0"::"r"(x));
#define CNTFRQ_EL0_READ(x)          asm volatile("mrs %0, cntfrq_el0":"=r"(x));
#define CNTKCTL_EL1_WRITE(x)        asm volatile("msr cntkctl_el1, %0"::"r"(x));
#define CNTKCTL_EL1_READ(x)         asm volatile("mrs %0, cntkctl_el1":"=r"(x));
#define CNTHCTL_EL2_WRITE(x)        asm volatile("msr cnthctl_el2, %0"::"r"(x));
#define CNTHCTL_EL2_READ(x)         asm volatile("mrs %0, cnthctl_el2":"=r"(x));
#define CNTPCT_EL0_WRITE(x)         asm volatile("msr cntpct_el0, %0"::"r"(x));
#define CNTPCT_EL0_READ(x)          asm volatile("mrs %0, cntpct_el0":"=r"(x));
#define CNTVOFF_EL2_WRITE(x)        asm volatile("msr cntvoff_el2, %0"::"r"(x));
#define CNTVOFF_EL2_READ(x)         asm volatile("mrs %0, cntvoff_el2":"=r"(x));
#define CNTVCT_EL0_WRITE(x)         asm volatile("msr cntvct_el0, %0"::"r"(x));
#define CNTVCT_EL0_READ(x)          asm volatile("mrs %0, cntvct_el0":"=r"(x));
#define CNTP_CTL_EL0_WRITE(x)       asm volatile("msr cntp_ctl_el0, %0"::"r"(x));
#define CNTP_CTL_EL0_READ(x)        asm volatile("mrs %0, cntp_ctl_el0":"=r"(x));
#define CNTP_TVAL_EL0_WRITE(x)      asm volatile("msr cntp_tval_el0, %0"::"r"(x));
#define CNTP_TVAL_EL0_READ(x)       asm volatile("mrs %0, cntp_tval_el0":"=r"(x));
#define CNTP_CVAL_EL0_WRITE(x)      asm volatile("msr cntp_cval_el0, %0"::"r"(x));
#define CNTP_CVAL_EL0_READ(x)       asm volatile("mrs %0, cntp_cval_el0":"=r"(x));
#define CNTPS_CTL_EL1_WRITE(x)      asm volatile("msr cntps_ctl_el1, %0"::"r"(x));
#define CNTPS_CTL_EL1_READ(x)       asm volatile("mrs %0, cntps_ctl_el1":"=r"(x));
#define CNTPS_TVAL_EL1_WRITE(x)     asm volatile("msr cntps_tval_el1, %0"::"r"(x));
#define CNTPS_TVAL_EL1_READ(x)      asm volatile("mrs %0, cntps_tval_el1":"=r"(x));
#define CNTPS_CVAL_EL1_WRITE(x)     asm volatile("msr cntps_cval_el1, %0"::"r"(x));
#define CNTPS_CVAL_EL1_READ(x)      asm volatile("mrs %0, cntps_cval_el1":"=r"(x));
#define CNTV_CTL_EL0_WRITE(x)       asm volatile("msr cntv_ctl_el0, %0"::"r"(x));
#define CNTV_CTL_EL0_READ(x)        asm volatile("mrs %0, cntv_ctl_el0":"=r"(x));
#define CNTV_TVAL_EL0_WRITE(x)      asm volatile("msr cntv_tval_el0, %0"::"r"(x));
#define CNTV_TVAL_EL0_READ(x)       asm volatile("mrs %0, cntv_tval_el0":"=r"(x));
#define CNTV_CVAL_EL0_WRITE(x)      asm volatile("msr cntv_cval_el0, %0"::"r"(x));
#define CNTV_CVAL_EL0_READ(x)       asm volatile("mrs %0, cntv_cval_el0":"=r"(x));

#define PMCCNTR_EL0_WRITE(x)        asm volatile("msr pmccntr_el0, %0"::"r"(x));
#define PMCCNTR_EL0_READ(x)         asm volatile("mrs %0, pmccntr_el0":"=r"(x));
#define PMCR_EL0_WRITE(x)           asm volatile("msr pmcr_el0, %0"::"r"(x));
#define PMCR_EL0_READ(x)            asm volatile("mrs %0, pmcr_el0":"=r"(x));
#define PMCNTENSET_EL0_WRITE(x)     asm volatile("msr pmcntenset_el0, %0"::"r"(x));
#define PMCNTENSET_EL0_READ(x)      asm volatile("mrs %0, pmcntenset_el0":"=r"(x));

/*
 *  変換テーブルへの設定内容
 *  va         : 仮想アドレス
 *  pa         : 物理アドレス
 *  size       : サイズ
 *  attr       : 属性
 *  ns         : ノンセキュア
 */
typedef struct{
    uint64_t    va;
    uint64_t    pa;
    uint64_t    size;
    uint8_t     attr;
    uint8_t     ns;
} mmap_t;

extern void cache_enable(void);
extern void cache_disable(void);
extern void mmu_mmap_init(void);
extern void mmu_mmap_add(const mmap_t *pmm);
extern void mmu_init(void);
extern void mmu_term(void);

#endif  /* MACRO_ONLY */

#endif /* AARCH64_H */

