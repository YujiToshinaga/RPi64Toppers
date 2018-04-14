#include "loader.h"
#include <stdarg.h>

#define OVF_CHECK
#define IMP_DOUBLE
#define IMP_MULT
#define IMP_DIV

#define SYNC_MAGIC      0x87654321

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

int sync = 0;
int exit_sync = 0;

//------------------------------------------------------------------------------
// Prototype declaration
//------------------------------------------------------------------------------
void cache_enable(void);
void cache_disable(void);
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
void mmu_mmap_init(void);
void mmu_mmap_add(const mmap_t *pmm);
void mmu_init(void);
void mmu_term(void);
void tlb_invalidate_all(void);
int mmu_tt_init(int mmap_index, uint64_t va, uint64_t *ptt, int level);
void gpio_init();
void gpio_fsel(int n, int sel);
void gpio_set(int n, int set);
int gpio_get(int n);
void serial_init();
char serial_getready();
void serial_put(char c);
char serial_get();
static inline void _putchar(char c);
static inline uint_t __alt_mult_uint(
		uint_t multiplicand, uint_t multiplier);
static inline uint_t __alt_div_uint(
		uint_t dividend, uint_t divisor);
static inline uint_t __alt_rem_uint(
		uint_t dividend, uint_t divisor);
static inline uint_t __pow_uint(
		uint_t base, uint_t exponent);
static inline uint_t __log_uint(
		uint_t base, uint_t antilogarithm);
#ifdef IMP_DOUBLE
static inline uint64_t __alt_mult_uint64(
		uint64_t multiplicand, uint64_t multiplier);
static inline uint64_t __alt_div_uint64(
		uint64_t dividend, uint64_t divisor);
static inline uint64_t __alt_rem_uint64(
		uint64_t dividend, uint64_t divisor);
static inline uint64_t __pow_uint64(
		uint64_t base, uint64_t exponent);
static inline uint64_t __log_uint64(
		uint64_t base, uint64_t antilogarithm);
#endif
void _print_dec(
		uint_t val, uint_t zpad_flag, uint_t length, uint_t sign_flag);
void _print_hex(
		uint_t uval, uint_t zpad_flag, uint_t length);
#ifdef IMP_DOUBLE
void _print_dec64(
		uint64_t val, uint_t zpad_flag, uint_t length, uint_t sign_flag);
void _print_hex64(
		uint64_t uval, uint_t zpad_flag, uint_t length);
#endif
void _print_char(char c);
void _print_str(char *str);

//------------------------------------------------------------------------------
// Local function
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

/*
 *  プロセッサID（0オリジン）の取得
 */
int prc_id(void)
{
    uint64_t tmp;
    int id;

    MPIDR_EL1_READ(tmp);
    id = (int)(tmp & MPIDR_AFF0_MASK);

    return id;
}

void gpio_init()
{
    wr_word(GPIO_GPPUD, 0);
}

void gpio_fsel(int n, int fsel)
{
    uint32_t tmp;
    uint32_t sel;
    int i;

    switch (fsel) {
        case GPIO_FSEL_IN:
            sel = 0;
            break;
        case GPIO_FSEL_OUT:
            sel = 1;
            break;
        case GPIO_FSEL_0:
            sel = 4;
            break;
        case GPIO_FSEL_1:
            sel = 5;
            break;
        case GPIO_FSEL_2:
            sel = 6;
            break;
        case GPIO_FSEL_3:
            sel = 7;
            break;
        case GPIO_FSEL_4:
            sel = 3;
            break;
        case GPIO_FSEL_5:
            sel = 2;
            break;
        default:
            sel = 0;
            break;
    }

    if ((n >= 0) && (n < 10)) {
        tmp = rd_word(GPIO_GPFSEL0);
        tmp &= ~(0x7 << (3 * (n - 0)));
        tmp |= sel << (3 * (n - 0));
        wr_word(GPIO_GPFSEL0, tmp);
    } else if ((n >= 10) && (n < 20)) {
        tmp = rd_word(GPIO_GPFSEL1);
        tmp &= ~(0x7 << (3 * (n - 10)));
        tmp |= sel << (3 * (n - 10));
        wr_word(GPIO_GPFSEL1, tmp);
    } else if ((n >= 20) && (n < 30)) {
        tmp = rd_word(GPIO_GPFSEL2);
        tmp &= ~(0x7 << (3 * (n - 20)));
        tmp |= sel << (3 * (n - 20));
        wr_word(GPIO_GPFSEL2, tmp);
    } else if ((n >= 30) && (n < 40)) {
        tmp = rd_word(GPIO_GPFSEL3);
        tmp &= ~(0x7 << (3 * (n - 30)));
        tmp |= sel << (3 * (n - 30));
        wr_word(GPIO_GPFSEL3, tmp);
    } else if ((n >= 40) && (n < 50)) {
        tmp = rd_word(GPIO_GPFSEL4);
        tmp &= ~(0x7 << (3 * (n - 40)));
        tmp |= sel << (3 * (n - 40));
        wr_word(GPIO_GPFSEL4, tmp);
    } else if ((n >= 50) && (n <= 53)) {
        tmp = rd_word(GPIO_GPFSEL5);
        tmp &= ~(0x7 << (3 * (n - 50)));
        tmp |= sel << (3 * (n - 50));
        wr_word(GPIO_GPFSEL5, tmp);
    }

    for (i = 0; i < 150; i++) {
        asm volatile("nop");
    }

    if ((n >= 0) && (n < 32)) {
        wr_word(GPIO_GPPUDCLK0, (0x1 << (n - 0)));
    } else if ((n >= 32) && (n <= 53)) {
        wr_word(GPIO_GPPUDCLK1, (0x1 << (n - 32)));
    }

    for (i = 0; i < 150; i++) {
        asm volatile("nop");
    }

    if ((n >= 0) && (n < 32)) {
        wr_word(GPIO_GPPUDCLK0, 0);
    } else if ((n >= 32) && (n <= 53)) {
        wr_word(GPIO_GPPUDCLK1, 0);
    }
}

void gpio_set(int n, int set)
{
    if ((n >= 0) && (n < 32)) {
        if (set == GPIO_HIGH) {
            wr_word(GPIO_GPSET0, (0x1 << (n - 0)));
        } else {
            wr_word(GPIO_GPCLR0, (0x1 << (n - 0)));
        }
    } else if ((n >= 32) && (n <= 53)) {
        if (set == GPIO_HIGH) {
            wr_word(GPIO_GPSET1, (0x1 << (n - 32)));
        } else {
            wr_word(GPIO_GPCLR1, (0x1 << (n - 32)));
        }
    } 
}

int gpio_get(int n)
{
    uint32_t get;
    int ret;

    if ((n >= 0) && (n < 32)) {
        get = rd_word(GPIO_GPLEV0);
        if (get & (0x1 << (n - 0))) {
            ret = GPIO_HIGH;
        } else {
            ret = GPIO_LOW;
        }
    } else if ((n >= 32) && (n <= 53)) {
        get = rd_word(GPIO_GPLEV1);
        if (get & (0x1 << (n - 32))) {
            ret = GPIO_HIGH;
        } else {
            ret = GPIO_LOW;
        }
    } 

    return ret;
}

void serial_init()
{
    uint32_t baud;
    uint32_t tmp;

    // GPIOをUARTに切り替え
    tmp = rd_word(GPIO_GPFSEL1);
    tmp &= ~(7 << 12);      // GPIO14 ALT5
    tmp |= 2 << 12;
    tmp &= ~(7 << 15);      // GPIO15 ALT5
    tmp |= 2 << 15;
    wr_word(GPIO_GPFSEL1, tmp);
    wr_word(GPIO_GPPUD, 0);
    for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
    wr_word(GPIO_GPPUDCLK0, (1 << 14) | (1 << 15));
    for (tmp = 0; tmp < 150; tmp++) {
        asm volatile("nop");
    }
    wr_word(GPIO_GPPUDCLK0, 0);

    baud = (250000000 / (UART_BAUDRATE * 8)) - 1;

    wr_word(AUX_ENABLES, rd_word(AUX_ENABLES) | AUX_ENABLES_MU_EN_BIT);
    wr_word(MU_CNTL, 0);
    wr_word(MU_IER, 0);
    wr_word(MU_IIR, MU_IIR_FIFO_CLR);
    wr_word(MU_LCR, MU_LCR_DS_8BIT);
    wr_word(MU_MCR, 0);
    wr_word(MU_BAUD, baud);
    wr_word(MU_CNTL, MU_CNTL_TX_EN_BIT | MU_CNTL_RX_EN_BIT);
}

char serial_getready()
{
    return ((rd_word(MU_LSR) & MU_LSR_DATA_RDY_BIT) == MU_LSR_DATA_RDY_BIT);
}

void serial_put(char c)
{
    while ((rd_word(MU_LSR) & MU_LSR_TX_EMPTY_BIT) != MU_LSR_TX_EMPTY_BIT);
    wr_word(MU_IO, c);
}

char serial_get()
{
    while ((rd_word(MU_LSR) & MU_LSR_DATA_RDY_BIT) != MU_LSR_DATA_RDY_BIT);
    return ((char)(rd_word(MU_IO) & MU_IO_DATA_MASK));
}

static inline void _putchar(char c)
{
	if (c == '\n') {
		serial_put('\n');
		serial_put('\r');
	} else {
		serial_put(c);
	}
}

/******** Multiply ********/
static inline uint_t __alt_mult_uint(
		uint_t multiplicand, uint_t multiplier)
{
	uint_t result;

#ifdef IMP_MULT

	result = multiplicand * multiplier;
#ifdef OVF_CHECK
	if (multiplicand != __alt_div_uint(result, multiplier)) {
		result = (uint_t)(-1);
	}
#endif

#else

	uint_t temp;
	int i;

	result = 0;
	temp = 0;
	for (i = 0; i < (int)multiplier; i++) {
		temp = result + multiplicand;
#ifdef OVF_CHECK
		if (temp < result) {
			result = (uint_t)(-1);
			break;
		}
#endif
		result = temp;
	}

#endif

	return result;
}

/******** Divide ********/
static inline uint_t __alt_div_uint(
		uint_t dividend, uint_t divisor)
{
	uint_t result;

#ifdef IMP_DIV

	result = dividend / divisor;

#else

	uint_t tmp;

	result = 0;
	tmp = dividend;
	while (tmp >= divisor) {
		result++;
		tmp = tmp - divisor;
	}

#endif

	return result;
}

/******** Remind ********/
static inline uint_t __alt_rem_uint(
		uint_t dividend, uint_t divisor)
{
	uint_t result;

#ifdef IMP_DIV

	result = dividend % divisor;

#else

	result = dividend;
	while (result >= divisor) {
		result = result - divisor;
	}

#endif

	return result;
}

/******** Power ********/
static inline uint_t __pow_uint(
		uint_t base, uint_t exponent)
{
	uint_t result;
	int i;

	result = 1;
	for (i = 0; i < (int)exponent; i++) {
		result = __alt_mult_uint(result, base);
#ifdef OVF_CHECK
		if (result == (uint_t)(-1)) {
			break;
		}
#endif
	}

	return result;
}

/******** Logalism ********/
static inline uint_t __log_uint(
		uint_t base, uint_t antilogarithm)
{
	uint_t result;
	uint_t pow;
	
	result = 0;
	pow = __pow_uint(base, result);
	while (pow <= antilogarithm) {
		result++;
		pow = __pow_uint(base, result);
#ifdef OVF_CHECK
		if (pow == (uint_t)(-1)) {
			break;
		}
#endif
	}
	result--;

	return result;
}

#ifdef IMP_DOUBLE

/******** Multiply ********/
static inline uint64_t __alt_mult_uint64(
		uint64_t multiplicand, uint64_t multiplier)
{
	uint64_t result;

#ifdef IMP_MULT

	result = multiplicand * multiplier;
#ifdef OVF_CHECK
	if (multiplicand != __alt_div_uint64(result, multiplier)) {
		result = (uint64_t)(-1);
	}
#endif

#else

	uint64_t temp;
	int i;

	result = 0;
	temp = 0;
	for (i = 0; i < (int)multiplier; i++) {
		temp = result + multiplicand;
#ifdef OVF_CHECK
		if (temp < result) {
			result = (uint64_t)(-1);
			break;
		}
#endif
		result = temp;
	}

#endif

	return result;

}

/******** Divide ********/
static inline uint64_t __alt_div_uint64(
		uint64_t dividend, uint64_t divisor)
{
	uint64_t result;

#ifdef IMP_DIV

	result = dividend / divisor;

#else

	uint64_t tmp;

	result = 0;
	tmp = dividend;
	while (tmp >= divisor) {
		result++;
		tmp = tmp - divisor;
	}

#endif

	return result;
}

/******** Remind ********/
static inline uint64_t __alt_rem_uint64(
		uint64_t dividend, uint64_t divisor)
{
	uint64_t result;

#ifdef IMP_DIV

	result = dividend % divisor;

#else

	result = dividend;
	while (result >= divisor) {
		result = result - divisor;
	}

#endif

	return result;
}

/******** Power ********/
static inline uint64_t __pow_uint64(
		uint64_t base, uint64_t exponent)
{
	uint64_t result;
	int i;

	result = 1;
	for (i = 0; i < (int)exponent; i++) {
		result = __alt_mult_uint64(result, base);
#ifdef OVF_CHECK
		if (result == (uint64_t)(-1)) {
			break;
		}
#endif
	}

	return result;
}

/******** Logalism ********/
static inline uint64_t __log_uint64(
		uint64_t base, uint64_t antilogarithm)
{
	uint64_t result;
	uint64_t pow;
	
	result = 0;
	pow = __pow_uint64(base, result);
	while (pow <= antilogarithm) {
		result++;
		pow = __pow_uint64(base, result);
#ifdef OVF_CHECK
		if (pow == (uint64_t)(-1)) {
			break;
		}
#endif
	}
	result--;

	return result;
}

#endif

/******** Singed Decimal Integer ********/
void _print_dec(
		uint_t val, uint_t zpad_flag, uint_t length,
		uint_t sign_flag)
{
	uint_t minus_flag;		// Plus : FALSE, Minus : TRUE
	uint_t uval;			// unsigned val
	uint_t digit_length;	// Digit length
	int_t pad_length;		// Padding length
	uint_t digit;			// Digit i
	int i;

	// Check Sign
	if ((sign_flag == TRUE) && ((int_t)val < 0)) {
		minus_flag = TRUE;
		uval = (uint_t)(-((int_t)val));
	} else {
		minus_flag = FALSE;
		uval = val;
	}

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
		digit_length = (uint_t)(__log_uint(10, uval) + 1);
	}

	// Print Pad and Sign
	if (zpad_flag == TRUE) {
		if (minus_flag == TRUE) {
			_putchar('-');
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		}
	} else {
		if (minus_flag == TRUE) {
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
			_putchar('-');
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
#ifdef IMP_DIV
		digit = (uint_t)((uval / __pow_uint(10, (uint_t)i)) % 10);
#else
		digit = (uint_t)__alt_rem_uint(
				__alt_div_uint(uval, __pow_uint(10, (uint_t)i)), 10);
#endif
		_putchar((char)((uint_t)'0' + digit));
	}
}

/******** Unsinged Hexadecimal Integer ********/
void _print_hex(
		uint_t uval, uint_t zpad_flag, uint_t length)
{
	uint_t digit_length;	// Digit length
	uint_t digit;			// Digit i
	int_t pad_length;		// Zero padding length
	uint_t temp_sa;			// shift amount
	int i;

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
//		digit_length = (uint_t)(__log_uint(16, uval) + 1);

		digit_length = 1;
		temp_sa = digit_length << 2;
		while ((temp_sa < (sizeof(uint_t) << 3)) &&
				((uval >> temp_sa) != 0)) {
			digit_length++;
			temp_sa = digit_length << 2;
		}
	}

	// Print Pad
	if (zpad_flag == TRUE) {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar('0');
		}
	} else {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar(' ');
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
//		digit = __alt_rem_uint(
//				__alt_div_uint(uval, __pow_uint(16, (uint_t)i)), 16);
		temp_sa = (uint_t)i << 2;
		digit = (uint_t)((uval >> temp_sa) & 0xf);

		if (digit >= 10) {
			_putchar((char)((uint_t)'a' + (digit - 10)));
		} else {
			_putchar((char)((uint_t)'0' + digit));
		}
	}
}

#ifdef IMP_DOUBLE

/******** Singed Decimal Integer ********/
void _print_dec64(
		uint64_t val, uint_t zpad_flag, uint_t length,
		uint_t sign_flag)
{
	uint_t minus_flag;		// Plus : FALSE, Minus : TRUE
	uint64_t uval;			// unsigned val
	uint_t digit_length;	// Digit length
	int_t pad_length;		// Padding length
	uint_t digit;			// Digit i
	int i;

	// Check Sign
	if ((sign_flag == TRUE) && ((int64_t)val < 0)) {
		minus_flag = TRUE;
		uval = (uint64_t)(-((int64_t)val));
	} else {
		minus_flag = FALSE;
		uval = val;
	}

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
		digit_length = (uint_t)(__log_uint64(10, uval) + 1);
	}

	// Print Pad and Sign
	if (zpad_flag == TRUE) {
		if (minus_flag == TRUE) {
			_putchar('-');
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar('0');
			}
		}
	} else {
		if (minus_flag == TRUE) {
			pad_length = (int_t)length - (int_t)digit_length - 1;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
			_putchar('-');
		} else {
			pad_length = (int_t)length - (int_t)digit_length;
			for (i = 0; i < (int)pad_length; i++) {
				_putchar(' ');
			}
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
#ifdef IMP_DIV
		digit = (uint_t)((uval / __pow_uint64(10, (uint64_t)i)) % 10);
#else
		digit = (uint_t)__alt_rem_uint64(
				__alt_div_uint64(uval, __pow_uint64(10, (uint64_t)i)), 10);
#endif
		_putchar((char)((uint_t)'0' + digit));
	}
}

/******** Unsinged Hexadecimal Integer ********/
void _print_hex64(
		uint64_t uval, uint_t zpad_flag, uint_t length)
{
	uint_t digit_length;	// Digit length
	uint_t digit;			// Digit i
	int_t pad_length;		// Zero padding length
	uint_t temp_sa;			// shift amount
	int i;

	// Check Digit Length
	if (uval == 0) {
		digit_length = 1;
	} else {
//		digit_length = (uint_t)(__log_uint64(16, uval) + 1);

		digit_length = 1;
		temp_sa = digit_length << 2;
		while ((temp_sa < (sizeof(uint64_t) << 3)) &&
				((uval >> temp_sa) != 0)) {
			digit_length++;
			temp_sa = digit_length << 2;
		}
	}

	// Print Pad
	if (zpad_flag == TRUE) {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar('0');
		}
	} else {
		pad_length = (int_t)length - (int_t)digit_length;
		for (i = 0; i < (int)pad_length; i++) {
			_putchar(' ');
		}
	}

	// Print Digit
	for (i = (int)digit_length - 1; i >= 0; i--) {
//		digit = __alt_rem_uint64(
//				__alt_div_uint64(uval, __pow_uint64(16, (uint_t)i)), 16);
		temp_sa = (uint_t)i << 2;
		digit = (uint_t)((uval >> temp_sa) & 0xf);

		if (digit >= 10) {
			_putchar((char)((uint_t)'a' + (digit - 10)));
		} else {
			_putchar((char)((uint_t)'0' + digit));
		}
	}
}

#endif

/******** Character ********/
void _print_char(char c)
{
	_putchar(c);
}

/******** String of Characters ********/
void _print_str(char *str)
{
	while (*str != '\0') {
		_putchar(*str);
		str++;
	}
}

//------------------------------------------------------------------------------
// Global function
//------------------------------------------------------------------------------
/******** Simple Print ********/
void prints(const char *str)
{
	while (*str != '\0') {
		_putchar(*str);
		str++;
	}
}

/******** Kernel Print ********/
void printk(const char *fmt, ...)
{
	uint_t zpad_flag;	// ZERO Padding Flag
	uint_t length;		// Padding Length

	va_list ap;

	va_start(ap, fmt);

	while (*fmt != '\0') {
		if (*fmt == '%') {

			// Initialize Variable
			zpad_flag = FALSE;
			length = 0;

			fmt++;

			// Check Zero Padding
			if (*fmt == '0') {
				zpad_flag = TRUE;
				fmt++;
			}

			// Check Padding Length
			while ((*fmt >= '0') && (*fmt <= '9')) {
				length = length * 10 + (uint_t)(*fmt - '0');
				fmt++;
			}

			// Check Format Specifiers
			switch (*fmt) {
				case 'd':
					_print_dec((uint_t)va_arg(ap, int_t),
							zpad_flag, length, TRUE);
					fmt++;
					break;
				case 'u':
					_print_dec((uint_t)va_arg(ap, int_t),
							zpad_flag, length, FALSE);
					fmt++;
					break;
				case 'x':
					_print_hex((uint_t)va_arg(ap, int_t),
							zpad_flag, length);
					fmt++;
					break;
				case 'c':
					_print_char((char)va_arg(ap, int_t));
					fmt++;
					break;
				case 's':
					_print_str((char *)va_arg(ap, char *));
					fmt++;
					break;
#ifdef IMP_DOUBLE
				case 'l':
					fmt++;
					switch (*fmt) {
						case 'd':
							_print_dec64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length, TRUE);
							fmt++;
							break;
						case 'u':
							_print_dec64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length, FALSE);
							fmt++;
							break;
						case 'x':
							_print_hex64((uint64_t)va_arg(ap, uint64_t),
									zpad_flag, length);
							fmt++;
							break;
						default:
							fmt++;
							break;
					}
					break;
#endif
				default:
					fmt++;
					break;
			}
		} else {
			_putchar(*fmt);
			fmt++;
		}
	}

	va_end(ap);
}

int loader(void)
{
    mmap_t mm;
    uint64_t tmp64;

    // シリアルを有効化する
    if (prc_id() == 0) {
        serial_init();
    }

    if (prc_id() == 0) {
        prints("Binary loader with XMODEM protocol\n");
        printk("    Load point : 0x%08x\n", LOAD_ADDR);
        printk("    Max size   : %d bytes\n", LOAD_SIZE);
        prints("\n");
    }

    // キャッシュ、MMUを有効化する
    cache_disable();

    SCTLR_EL1_READ(tmp64);
    tmp64 |= (SCTLR_SA_BIT | SCTLR_A_BIT);
    SCTLR_EL1_WRITE(tmp64);
    ISB();

    CPUECTLR_EL1_READ(tmp64);
    tmp64 |= CPUECTLR_SMPEN_BIT;
    CPUECTLR_EL1_WRITE(tmp64);
    ISB();

    mmu_mmap_init();

    mm.pa   = LOAD_ADDR;
    mm.va   = mm.pa;
    mm.size = LOAD_SIZE;
    mm.attr = MEM_ATTR_SO;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOADER_ADDR;
    mm.va   = mm.pa;
    mm.size = LOADER_SIZE;
    mm.attr = MEM_ATTR_SO;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOADER_CACHED_ADDR;
    mm.va   = mm.pa;
    mm.size = LOADER_CACHED_SIZE;
    mm.attr = MEM_ATTR_NML_C;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = LOAD_TMP_ADDR;
    mm.va   = mm.pa;
    mm.size = LOAD_SIZE;
    mm.attr = MEM_ATTR_NML_C;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = IO0_ADDR;
    mm.va   = mm.pa;
    mm.size = IO0_SIZE;
    mm.attr = MEM_ATTR_DEV;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);
    mm.pa   = IO1_ADDR;
    mm.va   = mm.pa;
    mm.size = IO1_SIZE;
    mm.attr = MEM_ATTR_DEV;
    mm.ns   = MEM_NS_NONSECURE;
    mmu_mmap_add(&mm);

    mmu_init();
    cache_enable();

    // バリア同期する
    if (prc_id() == 0) {
        sync = SYNC_MAGIC;
    } else {
        while (sync != SYNC_MAGIC);
    }

    return 0;
}

int loader_exit(void)
{
    cache_disable();
    mmu_term();

    // バリア同期する
    if (prc_id() == 0) {
        exit_sync = SYNC_MAGIC;
    } else {
        while (exit_sync != SYNC_MAGIC);
    }

    return 0;
}

