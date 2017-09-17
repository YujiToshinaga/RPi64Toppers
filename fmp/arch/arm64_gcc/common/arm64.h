/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2008 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: arm64.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  ARM64のハードウェア資源の定義
 */

#ifndef TOPPERS_ARM64_H
#define TOPPERS_ARM64_H

/* 
 *  ARM64のハードウェア資源のツール依存部の定義 
 */ 
#include <arm64_tool.h>

/*
 *  例外ベクタ
 */
#define CUR_SP0_SYNC_Vector     ULONG_C(0x80 * 0)
#define CUR_SP0_IRQ_Vector      ULONG_C(0x80 * 1)
#define CUR_SP0_FIQ_Vector      ULONG_C(0x80 * 2)
#define CUR_SP0_SERR_Vector     ULONG_C(0x80 * 3)
#define CUR_SPX_SYNC_Vector     ULONG_C(0x80 * 4)
#define CUR_SPX_IRQ_Vector      ULONG_C(0x80 * 5)
#define CUR_SPX_FIQ_Vector      ULONG_C(0x80 * 6)
#define CUR_SPX_SERR_Vector     ULONG_C(0x80 * 7)
#define L64_SYNC_Vector         ULONG_C(0x80 * 8)
#define L64_IRQ_Vector          ULONG_C(0x80 * 9)
#define L64_FIQ_Vector          ULONG_C(0x80 * 10)
#define L64_SERR_Vector         ULONG_C(0x80 * 11)
#define L32_SYNC_Vector         ULONG_C(0x80 * 12)
#define L32_IRQ_Vector          ULONG_C(0x80 * 13)
#define L32_FIQ_Vector          ULONG_C(0x80 * 14)
#define L32_SERR_Vector         ULONG_C(0x80 * 15)

/*
 *  例外ベクタ番号
 */
#define CUR_SP0_SYNC_Number     UINT_C(0)
#define CUR_SP0_IRQ_Number      UINT_C(1)
#define CUR_SP0_FIQ_Number      UINT_C(2)
#define CUR_SP0_SERR_Number     UINT_C(3)
#define CUR_SPX_SYNC_Number     UINT_C(4)
#define CUR_SPX_IRQ_Number      UINT_C(5)
#define CUR_SPX_FIQ_Number      UINT_C(6)
#define CUR_SPX_SERR_Number     UINT_C(7)
#define L64_SYNC_Number         UINT_C(8)
#define L64_IRQ_Number          UINT_C(9)
#define L64_FIQ_Number          UINT_C(10)
#define L64_SERR_Number         UINT_C(11)
#define L32_SYNC_Number         UINT_C(12)
#define L32_IRQ_Number          UINT_C(13)
#define L32_FIQ_Number          UINT_C(14)
#define L32_SERR_Number         UINT_C(15)

/*
 *  CPSRの構成
 */
#define DAIF_MASK               UINT_C(0x3c0)
#define DAIF_D_BIT              UINT_C(0x200)
#define DAIF_A_BIT              UINT_C(0x100)
#define DAIF_I_BIT              UINT_C(0x080)
#define DAIF_F_BIT              UINT_C(0x040)

#define CURRENTEL_EL_MASK       UINT_C(0x3 << 2)
#define CURRENTEL_EL_EL0        UINT_C(0x0 << 2)
#define CURRENTEL_EL_EL1        UINT_C(0x1 << 2)
#define CURRENTEL_EL_EL2        UINT_C(0x2 << 2)
#define CURRENTEL_EL_EL3        UINT_C(0x3 << 2)

#define SPSEL_SP_MASK           UINT_C(0x1 << 0)
#define SPSEL_SP_EL0            UINT_C(0x0 << 0)
#define SPSEL_SP_ELX            UINT_C(0x1 << 0)

#define MODE_MASK               UINT_C(0x1f << 0)
#define MODE_AARCH64_EL0T       UINT_C(0x00 << 0)
#define MODE_AARCH64_EL1T       UINT_C(0x04 << 0)
#define MODE_AARCH64_EL1H       UINT_C(0x05 << 0)
#define MODE_AARCH64_EL2T       UINT_C(0x08 << 0)
#define MODE_AARCH64_EL2H       UINT_C(0x09 << 0)
#define MODE_AARCH64_EL3T       UINT_C(0x0c << 0)
#define MODE_AARCH64_EL3H       UINT_C(0x0d << 0)

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
#define MPIDR_AFF3_MASK             ULONG_C(0x000000ff00000000)
#define MPIDR_AFF2_MASK             ULONG_C(0x0000000000ff0000)
#define MPIDR_AFF1_MASK             ULONG_C(0x000000000000ff00)
#define MPIDR_AFF0_MASK             ULONG_C(0x00000000000000ff)
#define MPIDR_AFF_ALL_MASK          ULONG_C(0x000000ff00ffffff)
#define MPIDR_AFF_MASK              ULONG_C(0xff)
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

#ifndef TOPPERS_MACRO_ONLY

/*
 *  変換テーブルへの設定内容
 *  va         : 仮想アドレス
 *  pa         : 物理アドレス
 *  size       : サイズ
 *  attr       : 属性
 *  ns         : ノンセキュア
 */
typedef struct{
    uintptr_t   va;
    uintptr_t   pa;
    uintptr_t   size;
    uint8_t     attr;
    uint8_t     ns;
} mmap_t;

/*
 *  Instruction Synchronization Barrier
 */
Inline void
inst_sync_barrier(void)
{
    INST_SYNC_BARRIER();
}

/*
 *  Data Memory Barrier
 */
Inline void
data_memory_barrier(void)
{
    DATA_MEMORY_BARRIER();
}

/*
 *  Data Synchronization Barrier
 */
Inline void
data_sync_barrier(void)
{
    DATA_SYNC_BARRIER();
}

/*
 *  Dキャッシュのラインの無効化
 */
Inline void
dcache_invalidate_line(uint32_t bits)
{
    DC_ISW(bits);
}

/*
 *  Dキャッシュのラインのクリーンと無効化
 */
Inline void
dcache_clean_and_invalidate_line(uint64_t bits)
{
    DC_CISW(bits);
}

/*
 *  Iキャッシュの全ラインの無効化
 */
extern void icache_invalidate_all(void);

/*
 *  Dキャッシュの全ラインの無効化
 */
extern void dcache_invalidate_all(void);

/*
 *  Dキャッシュの全ラインのクリーンと無効化
 */
extern void dcache_clean_and_invalidate_all(void);

/*
 *  Iキャッシュのフラッシュ
 */
extern void icache_flush(void);

/*
 *  Dキャッシュのフラッシュ
 */
extern void dcache_flush(void);

/*
 *  I/Dキャッシュを両方をフラッシュ
 */
extern void cache_flush(void);

/*
 *  Iキャッシュの開始
 */
extern void icache_enable(void);

/*
 *  Iキャッシュの停止
 */
extern void icache_disable(void);

/*
 *  Dキャッシュの開始
 */
extern void dcache_enable(void);

/*
 *  Dキャッシュの停止
 */
extern void dcache_disable(void);

/*
 *  I/Dキャッシュを両方を有効に
 */
extern void cache_enable(void);

/*
 *  I/Dキャッシュを両方を無効に
 */
extern void cache_disable(void);

/*
 *  TLBの全エントリの無効化
 */
extern void tlb_invalidate_all(void);

/*
 *  MMUの初期化
 */
extern void mmu_init(void);

/*
 *  メモリマップ設定の追加
 */
extern void mmu_mmap_add(const mmap_t *pmm);

#endif  /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_ARM64_H */
