/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2006-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: arm64.c 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

#include "kernel_impl.h"

/*
 *  キャッシュ関連
 */
/*
 *  Iキャッシュの全ラインの無効化
 */
void
icache_invalidate_all(void)
{
    IC_IALLU();
    IC_IALLUIS();
    data_sync_barrier();
}

/*
 *  Dキャッシュの全ラインの無効化
 */
void
dcache_invalidate_all()
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
            inst_sync_barrier();
            CCSIDR_EL1_READ(ccsidr);
            way_max = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK)
                    >> CCSIDR_ASSOCIATIVITY_SHIFT) + 1;
            set_max = ((ccsidr & CCSIDR_NUMSETS_MASK)
                    >> CCSIDR_NUMSETS_SHIFT) + 1;
            log2linesize = ((ccsidr & CCSIDR_LINESIZE_MASK)
                    >> CCSIDR_LINESIZE_SHIFT) + 4;
            data_sync_barrier();
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
    data_sync_barrier();
    inst_sync_barrier();
}

/*
 *  Dキャッシュの全ラインのクリーンと無効化
 */
void
dcache_clean_and_invalidate_all()
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
            inst_sync_barrier();
            CCSIDR_EL1_READ(ccsidr);
            way_max = ((ccsidr & CCSIDR_ASSOCIATIVITY_MASK)
                    >> CCSIDR_ASSOCIATIVITY_SHIFT) + 1;
            set_max = ((ccsidr & CCSIDR_NUMSETS_MASK)
                    >> CCSIDR_NUMSETS_SHIFT) + 1;
            log2linesize = ((ccsidr & CCSIDR_LINESIZE_MASK)
                    >> CCSIDR_LINESIZE_SHIFT) + 4;
            data_sync_barrier();
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
    data_sync_barrier();
    inst_sync_barrier();
}

/*
 *  Iキャッシュのフラッシュ
 */
void
icache_flush(void)
{
    icache_invalidate_all();
}

/*
 *  Dキャッシュのフラッシュ
 */
void
dcache_flush(void)
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
void
cache_flush(void)
{
    icache_flush();
    dcache_flush();
}

/*
 *  Iキャッシュの開始
 */
void
icache_enable(void)
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
    inst_sync_barrier();
}

/*
 *  Iキャッシュの停止
 */
void
icache_disable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);
    bits &= ~SCTLR_I_BIT;
    SCTLR_EL1_WRITE(bits);
    inst_sync_barrier();

    icache_invalidate_all();
}

/*
 *  Dキャッシュの開始
 */
void
dcache_enable(void)
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
    inst_sync_barrier();
}

/*
 *  Dキャッシュの停止
 *  CA9では，Dキャッシュが無効な状態でClean and Invalidate()を実行すると，
 *  暴走するため，Dキャッシュの状態を判断して，無効な場合は，Invalidate
 *  のみを行う． 
 *  ARM 64bitでもこの論理を残しておく．
 */
void
dcache_disable(void)
{
    uint64_t bits;

    SCTLR_EL1_READ(bits);
    if (bits & SCTLR_C_BIT) {
        bits &= ~SCTLR_C_BIT;
        SCTLR_EL1_WRITE(bits);
        inst_sync_barrier();
        dcache_clean_and_invalidate_all();
    } else {
        dcache_invalidate_all();
    }
}

/*
 *  I/Dキャッシュを両方を有効に
 */
void
cache_enable(void)
{
    icache_enable();
    dcache_enable();
}

/*
 *  I/Dキャッシュを両方を無効に
 */
void
cache_disable(void)
{
    icache_disable();
    dcache_disable();
}

/*
 *  TLB関連
 */
/*
 *  TLBの全エントリの無効化
 */
void
tlb_invalidate_all(void)
{
    TLBI_VMALLE1();
    data_sync_barrier();
}

/*
 *  MMU関連
 */
/*
 *  初段の変換テーブル
 */
static uintptr_t tt_pri[TNUM_PRCID][TT_FIRST_ENTRY_NUM] __attribute__ ((aligned(TT_FIRST_SIZE)));
#define my_tt_pri (tt_pri[x_prc_index()])

/*
 *  2段目以降の変換テーブル
 */
#define TT_NUM_MAX              10
static uintptr_t tt[TNUM_PRCID][TT_NUM_MAX][TT_ENTRY_NUM] __attribute__ ((aligned(TT_SIZE)));
#define my_tt (tt[x_prc_index()])
static int tt_num[TNUM_PRCID];
#define my_tt_num (tt_num[x_prc_index()])

/*
 *  メモリマップの設定を保存しておく配列
 */
#define MMAP_NUM_MAX            10
static mmap_t mmap[TNUM_PRCID][MMAP_NUM_MAX];
#define my_mmap (mmap[x_prc_index()])
static int mmap_num[TNUM_PRCID];
#define my_mmap_num (mmap_num[x_prc_index()])

/*
 *  メモリマップの初期化
 */
void
mmu_mmap_init(void)
{
    int i;

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
void
mmu_mmap_add(const mmap_t *pmm)
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
 *  変換テーブルの初期化
 */
int
mmu_tt_init(int mmap_index, uintptr_t va, uintptr_t *ptt, int level)
{
    int lvl_sft;
    uintptr_t lvl_entspc_sz;    /* 現レベルの1エントリ分アドレス空間サイズ */
    uintptr_t lvl_allspc_sz;    /* 現レベルの全エントリ分アドレス空間サイズ */
    uintptr_t va_base;
    mmap_t *pmmap;
    uintptr_t desc;

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
            desc = TT_DESC_TABLE | (uintptr_t)my_tt[my_tt_num];
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

/*
 *  MMUの初期化
 */
void
mmu_init(void)
{
    uint64_t bits;

    /* パイプラインをフラッシュ */
    inst_sync_barrier();

    /* メモリマップ設定用変数を初期化 */
    my_mmap_num = 0;
    mmu_mmap_init();

    /* ターゲット依存部での変換テーブルの初期化 */
    target_mmu_init();

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
    data_sync_barrier();
    inst_sync_barrier();

    /* MMUを有効にする*/
    bits = 0;
    SCTLR_EL1_READ(bits);
    bits |= SCTLR_M_BIT;
    SCTLR_EL1_WRITE(bits);

    /* 確実に設定を反映させる */
    inst_sync_barrier();
}
