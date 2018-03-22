#include "stddef.h"
#include "bcm283x.h"
#include "aarch64.h"

void icache_invalidate_all(void);
void dcache_invalidate_line(uint32_t bits);
void dcache_clean_and_invalidate_line(uint64_t bits);
void dcache_invalidate_all(void);
void dcache_clean_and_invalidate_all(void);
void icache_flush(void);
void dcache_flush(void);
void cache_flush(void);
void icache_enable(void);
void icache_disable(void);
void dcache_enable(void);
void dcache_disable(void);
void tlb_invalidate_all(void);
int mmu_tt_init(int mmap_index, uint64_t va, uint64_t *ptt, int level);

/*
 *  初段の変換テーブル
 */
static uint64_t tt_pri[PRC_NUM][TT_FIRST_ENTRY_NUM] __attribute__ ((aligned(TT_FIRST_SIZE)));
#define my_tt_pri (tt_pri[prc_id()])

/*
 *  2段目以降の変換テーブル
 */
#define TT_NUM_MAX              10
static uint64_t tt[PRC_NUM][TT_NUM_MAX][TT_ENTRY_NUM] __attribute__ ((aligned(TT_SIZE)));
#define my_tt (tt[prc_id()])
static int tt_num[PRC_NUM];
#define my_tt_num (tt_num[prc_id()])

/*
 *  メモリマップの設定を保存しておく配列
 */
#define MMAP_NUM_MAX            10
static mmap_t mmap[PRC_NUM][MMAP_NUM_MAX];
#define my_mmap (mmap[prc_id()])
static int mmap_num[PRC_NUM];
#define my_mmap_num (mmap_num[prc_id()])

//------------------------------------------------------------------------------
// Global function
//------------------------------------------------------------------------------
/*
 *  I/Dキャッシュを両方を有効に
 */
void cache_enable(void)
{
    icache_enable();
    dcache_enable();
}

/*
 *  I/Dキャッシュを両方を無効に
 */
void cache_disable(void)
{
    icache_disable();
    dcache_disable();
}

/*
 *  メモリマップの初期化
 */
void mmu_mmap_init(void)
{
    int i;

    /* メモリマップ設定用変数を初期化 */
    my_mmap_num = 0;

    for (i = 0; i < MMAP_NUM_MAX; i++) {
        my_mmap[i].pa = 0;
        my_mmap[i].va = 0;
        my_mmap[i].size = 0;
        my_mmap[i].attr = MEM_ATTR_SO;
        my_mmap[i].ns = MEM_NS_NONSECURE;
    }
}

/*
 *  メモリマップの設定の追加
 */
void mmu_mmap_add(const mmap_t *pmm)
{
    int insert_index;
    int i;

    assert((0 <= my_mmap_num) && (my_mmap_num < MMAP_NUM_MAX));
    assert((pmm->pa & TT_OFFSET_MASK) == 0x0);
    assert((pmm->va & TT_OFFSET_MASK) == 0x0);
    assert((pmm->size & TT_OFFSET_MASK) == 0x0);

    if (pmm->size == 0)
        return;

    /*
     *  メモリマップの設定の挿入場所を探索
     */
    insert_index = 0;
    while ((my_mmap[insert_index].va < pmm->va) && (insert_index < my_mmap_num)) {
        insert_index++;
    }

    assert((insert_index == 0) ||
            ((my_mmap[insert_index - 1].va + my_mmap[insert_index - 1].size)
             <= pmm->va));

    /*
     *  メモリマップの設定の挿入
     */
    for (i = my_mmap_num - 1; i >= insert_index; i--) {
        my_mmap[i + 1] = my_mmap[i];
    }
    my_mmap[insert_index] = *pmm;
    my_mmap_num++;

    assert(my_mmap_num < MMAP_NUM_MAX);
}

/*
 *  MMUの初期化
 */
void mmu_init(void)
{
    uint64_t bits;

    ISB();

    /* 変換テーブルの作成 */
    my_tt_num = 0;
    mmu_tt_init(0, 0x0, my_tt_pri, TT_FIRST_LEVEL);

    /* メモリの属性設定 */
    bits = 0;
    bits = MAIR_ATTR_SO     << (MAIR_ATTR_SHIFT * MEM_ATTR_SO_ATTRINDX);
    bits = MAIR_ATTR_DEV    << (MAIR_ATTR_SHIFT * MEM_ATTR_DEV_ATTRINDX);
    bits = MAIR_ATTR_NML_NC << (MAIR_ATTR_SHIFT * MEM_ATTR_NML_NC_ATTRINDX);
    bits = MAIR_ATTR_NML_C  << (MAIR_ATTR_SHIFT * MEM_ATTR_NML_C_ATTRINDX);
    MAIR_EL1_WRITE(bits);

    /* TLBのインバリデート */
    tlb_invalidate_all();

    /* アドレス変換の制御設定 */
    bits = 0;
    bits |= TCR_IPS_4GB;
    bits |= TCR_TG0_4KB;
    bits |= TCR_SH0_INNER_SHAREABLE;
    bits |= TCR_ORGN0_WBWAC;
    bits |= TCR_IRGN0_WBWAC;
    bits |= (64 - ADDR_SPACE_WIDTH) << TCR_T0SZ_SHIFT;
    TCR_EL1_WRITE(bits);

    /* 変換テーブルをセットする */
    TTBR0_EL1_WRITE((uint64_t)my_tt_pri);

    /* 確実に変換テーブルをメモリに書き込み，設定を反映させる */
    DSB();
    ISB();

    /* MMUを有効にする*/
    bits = 0;
    SCTLR_EL1_READ(bits);
    bits |= SCTLR_M_BIT;
    SCTLR_EL1_WRITE(bits);

    /* 確実に設定を反映させる */
    ISB();
}

/*
 *  MMUの終了
 */
void mmu_term(void)
{
    uint64_t bits;

    /* MMUを無効にする*/
    bits = 0;
    SCTLR_EL1_READ(bits);
    bits &= ~SCTLR_M_BIT;
    SCTLR_EL1_WRITE(bits);
    ISB();
}

//------------------------------------------------------------------------------
// Local function
//------------------------------------------------------------------------------
/*
 *  キャッシュ関連
 */
/*
 *  Iキャッシュの全ラインの無効化
 */
void icache_invalidate_all(void)
{
    IC_IALLU();
    IC_IALLUIS();
    DSB();
}

/*
 *  Dキャッシュのラインの無効化
 */
void dcache_invalidate_line(uint32_t bits)
{
    DC_ISW(bits);
}

/*
 *  Dキャッシュのラインのクリーンと無効化
 */
void dcache_clean_and_invalidate_line(uint64_t bits)
{
    DC_CISW(bits);
}

/*
 *  Dキャッシュの全ラインの無効化
 */
void dcache_invalidate_all()
{
    uint64_t clidr;
    uint64_t ccsidr;
    uint64_t ctype;
    uint64_t level_max, level;
    uint64_t way_max, way;
    uint64_t set_max, set;
    uint64_t log2linesize;
    uint64_t bits;

    CLIDR_EL1_READ(clidr);
    level_max = (clidr & CLIDR_LOC_MASK) >> CLIDR_LOC_SHIFT;
    for (level = 0; level < level_max; level++) {
        ctype = (clidr >> (CLIDR_CTYPE_SHIFT * level)) & CLIDR_CTYPE_MASK;
        if ((ctype == CLIDR_CTYPE_DATA) || (ctype == CLIDR_CTYPE_SEPARATE)
                || (ctype == CLIDR_CTYPE_UNIFIED)) {
            CSSELR_EL1_WRITE(level << CSSELR_LEVEL_SHIFT);
            ISB();
            CCSIDR_EL1_READ(ccsidr);
            way_max = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK)
                    >> CCSIDR_ASSOCIATIVITY_SHIFT) + 1;
            set_max = ((ccsidr & CCSIDR_NUMSETS_MASK)
                    >> CCSIDR_NUMSETS_SHIFT) + 1;
            log2linesize = ((ccsidr & CCSIDR_LINESIZE_MASK)
                    >> CCSIDR_LINESIZE_SHIFT) + 4;
            DSB();
            for (way = 0; way < way_max; way++) {
                for (set = 0; set < set_max; set++) {
                    bits = 0;
                    bits |= way << __builtin_clz((uint32_t)way_max);
                    bits |= set << log2linesize;
                    bits |= level << DC_LEVEL_SHIFT;
                    dcache_invalidate_line(bits);
                }
            }
        }
    }
    CSSELR_EL1_WRITE(0);
    DSB();
    ISB();
}

/*
 *  Dキャッシュの全ラインのクリーンと無効化
 */
void dcache_clean_and_invalidate_all()
{
    uint64_t clidr;
    uint64_t ccsidr;
    uint64_t ctype;
    uint64_t level_max, level;
    uint64_t way_max, way;
    uint64_t set_max, set;
    uint64_t log2linesize;
    uint64_t bits;

    CLIDR_EL1_READ(clidr);
    level_max = (clidr & CLIDR_LOC_MASK) >> CLIDR_LOC_SHIFT;
    for (level = 0; level < level_max; level++) {
        ctype = (clidr >> (CLIDR_CTYPE_SHIFT * level)) & CLIDR_CTYPE_MASK;
        if ((ctype == CLIDR_CTYPE_DATA) || (ctype == CLIDR_CTYPE_SEPARATE)
                || (ctype == CLIDR_CTYPE_UNIFIED)) {
            CSSELR_EL1_WRITE(level << CSSELR_LEVEL_SHIFT);
            ISB();
            CCSIDR_EL1_READ(ccsidr);
            way_max = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK)
                    >> CCSIDR_ASSOCIATIVITY_SHIFT) + 1;
            set_max = ((ccsidr & CCSIDR_NUMSETS_MASK)
                    >> CCSIDR_NUMSETS_SHIFT) + 1;
            log2linesize = ((ccsidr & CCSIDR_LINESIZE_MASK)
                    >> CCSIDR_LINESIZE_SHIFT) + 4;
            DSB();
            for (way = 0; way < way_max; way++) {
                for (set = 0; set < set_max; set++) {
                    bits = 0;
                    bits |= way << __builtin_clz((uint32_t)way_max);
                    bits |= set << log2linesize;
                    bits |= level << DC_LEVEL_SHIFT;
                    dcache_clean_and_invalidate_line(bits);
                }
            }
        }
    }
    CSSELR_EL1_WRITE(0);
    DSB();
    ISB();
}

/*
 *  Iキャッシュのフラッシュ
 */
void icache_flush(void)
{
    icache_invalidate_all();
}

/*
 *  Dキャッシュのフラッシュ
 */
void dcache_flush(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);
    if (bits & SCTLR_C_BIT) {
        dcache_clean_and_invalidate_all();
    } else {
        dcache_invalidate_all();
    }
}

/*
 *  I/Dキャッシュを両方をフラッシュ
 */
void cache_flush(void)
{
    icache_flush();
    dcache_flush();
}

/*
 *  Iキャッシュの開始
 */
void icache_enable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);

    /* すでにONならリターン */
    if (bits & SCTLR_I_BIT) {
        return;
    }

    icache_invalidate_all();

    bits |= SCTLR_I_BIT;
    SCTLR_EL1_WRITE(bits);
    ISB();
}

/*
 *  Iキャッシュの停止
 */
void icache_disable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);
    bits &= ~SCTLR_I_BIT;
    SCTLR_EL1_WRITE(bits);
    ISB();

    icache_invalidate_all();
}

/*
 *  Dキャッシュの開始
 */
void dcache_enable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);

    /* すでにONならリターン */
    if (bits & SCTLR_C_BIT) {
        return;
    }

    dcache_invalidate_all();

    bits |= SCTLR_C_BIT;
    SCTLR_EL1_WRITE(bits);
    ISB();
}

/*
 *  Dキャッシュの停止
 */
void dcache_disable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);
    if (bits & SCTLR_C_BIT) {
        bits &= ~SCTLR_C_BIT;
        SCTLR_EL1_WRITE(bits);
        ISB();
        dcache_clean_and_invalidate_all();
    } else {
        dcache_invalidate_all();
    }
}

/*
 *  MMU関連
 */
/*
 *  TLBの全エントリの無効化
 */
void tlb_invalidate_all(void)
{
    TLBI_VMALLE1();
    DSB();
}

/*
 *  変換テーブルの初期化
 */
int mmu_tt_init(int mmap_index, uint64_t va, uint64_t *ptt, int level)
{
    int lvl_sft;
    uint64_t lvl_entspc_sz;    /* 現レベルの1エントリ分アドレス空間サイズ */
    uint64_t lvl_allspc_sz;    /* 現レベルの全エントリ分アドレス空間サイズ */
    uint64_t va_base;
    mmap_t *pmmap;
    uint64_t desc;

    assert(level <= TT_LAST_LEVEL);

    /*
     *  現レベルで設定するアドレス空間サイズの計算
     */
    lvl_sft = TT_GRANULE_WIDTH + (TT_LAST_LEVEL - level) * TT_LEVEL_WIDTH;
    lvl_entspc_sz = 1 << lvl_sft;
    if (level == TT_FIRST_LEVEL) {
        lvl_allspc_sz = ADDR_SPACE_SIZE;
    } else {
        lvl_allspc_sz = 1 << (lvl_sft + TT_LEVEL_WIDTH);
    }

    /*
     *  変換テーブルの各エントリを設定
     */
    va_base = va;
    pmmap = &my_mmap[mmap_index];
    while ((va_base <= va) && (va < va_base + lvl_allspc_sz)) {
        /*
         *  次のメモリマップの設定へ
         */
        if (((pmmap->va + pmmap->size) <= va) && (mmap_index < my_mmap_num)) {
            mmap_index++;
            pmmap = &my_mmap[mmap_index];
            continue;
        }

        /*
        *  メモリマップの設定がないので，
        *  変換テーブルのエントリをINVALIDで埋める
        */
        if ((mmap_index >= my_mmap_num) || (pmmap->va >= (va + lvl_entspc_sz))) {
            desc = TT_DESC_INVALID;

        /*
        *  メモリマップの設定通りに
        *  変換テーブルのエントリをBLOCK or PAGEで埋める
        */
        } else if ((pmmap->va <= va) &&
                ((pmmap->va + pmmap->size) >= (va + lvl_entspc_sz))) {
            if (level == 3) {
                desc = TT_DESC_PAGE;
            } else {
                desc = TT_DESC_BLOCK;
            }
            desc = desc | TT_AF_BIT;
            switch (pmmap->attr) {
                case (MEM_ATTR_SO) :
                    desc = desc | TT_SH_OSH;
                    desc |= (MEM_ATTR_SO_ATTRINDX << TT_ATTRINDX_SHIFT);
                    break;
                case (MEM_ATTR_DEV) :
                    desc = desc | TT_SH_OSH;
                    desc |= (MEM_ATTR_DEV_ATTRINDX << TT_ATTRINDX_SHIFT);
                    break;
                case (MEM_ATTR_NML_NC) :
                    desc = desc | TT_SH_ISH;
                    desc |= (MEM_ATTR_NML_NC_ATTRINDX << TT_ATTRINDX_SHIFT);
                    break;
                case (MEM_ATTR_NML_C) :
                    desc = desc | TT_SH_ISH;
                    desc |= (MEM_ATTR_NML_C_ATTRINDX << TT_ATTRINDX_SHIFT);
                    break;
                default :
                    desc = desc | TT_SH_OSH;
                    desc |= (MEM_ATTR_SO_ATTRINDX << TT_ATTRINDX_SHIFT);
                    break;
            }
            if (pmmap->ns == MEM_NS_NONSECURE) {
                desc = desc | TT_NS_BIT;
            }
            desc = desc | (va - pmmap->va + pmmap->pa);

        /*
        *  メモリマップの設定が細かいので，
        *  変換テーブルのエントリをTABLEにして次段へ
        */
        } else {
            assert(my_tt_num <= TT_NUM_MAX);
            desc = TT_DESC_TABLE | (uint64_t)my_tt[my_tt_num];
            mmap_index = mmu_tt_init(mmap_index, va, my_tt[my_tt_num++], level + 1);
            pmmap = &my_mmap[mmap_index];
        }

        /*
         *  変換テーブルのエントリの書き込み
         */
        *ptt++ = desc;
        va += lvl_entspc_sz;
    }

    return mmap_index;
}

