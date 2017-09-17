/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel    
 * 
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2007-2015 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_serial.c 1147 2016-01-08 08:12:25Z ertl-toshinaga $
 */

/*
 *  シリアルI/Oデバイス（SIO）ドライバ（BCM283X用）
 */

#include <kernel.h>
#include <t_syslog.h>
#include "target_syssvc.h"
#include "target_serial.h"

/*
 *  シリアルI/Oポート初期化ブロックの定義
 */
typedef struct sio_port_initialization_block {
    uint32_t intno;    /* 割込み番号 */
} SIOPINIB;

/*
 *  シリアルI/Oポート管理ブロックの定義
 */
typedef struct sio_port_control_block {
    const SIOPINIB  *siopinib;  /* シリアルI/Oポート初期化ブロック */
    intptr_t  exinf;            /* 拡張情報 */
    bool_t    openflag;         /* オープン済みフラグ */
    bool_t    sendflag;         /* 送信割込みイネーブルフラグ */
    bool_t    getready;         /* 文字を受信した状態 */
    bool_t    putready;         /* 文字を送信できる状態 */
} SIOPCB;

/*
 *  シリアルI/Oポート管理ブロックのエリア
 */
SIOPCB siopcb_table[TNUM_SIOP];

/*
 *  シリアルI/OポートIDから管理ブロックを取り出すためのマクロ
 */
#define INDEX_SIOP(siopid)    ((uint_t)((siopid) - 1))
#define get_siopcb(siopid)    (&(siopcb_table[INDEX_SIOP(siopid)]))

/*
 *  シリアルI/Oポート初期化ブロック
 */
const SIOPINIB siopinib_table[TNUM_SIOP] = {
    {
        INTNO_SIO,
    },
};

/*
 *  Reset the Mini UART hardware to a default status
 */
void
mu_reset(ID siopid)
{
    uint32_t baud = (250000000 / (UART_BAUDRATE * 8)) - 1;

    sil_wrw_mem((uint32_t *)AUX_ENABLES,
            sil_rew_mem((uint32_t *)AUX_ENABLES) | AUX_ENABLES_MU_EN_BIT);
    sil_wrw_mem((uint32_t *)MU_CNTL, 0);
    sil_wrw_mem((uint32_t *)MU_IER, 0);
    sil_wrw_mem((uint32_t *)MU_IIR, MU_IIR_FIFO_CLR);
    sil_wrw_mem((uint32_t *)MU_LCR, MU_LCR_DS_8BIT);
    sil_wrw_mem((uint32_t *)MU_MCR, 0);
    sil_wrw_mem((uint32_t *)MU_BAUD, baud);
    sil_wrw_mem((uint32_t *)MU_CNTL, MU_CNTL_TX_EN_BIT | MU_CNTL_RX_EN_BIT);
}

/*
 *  文字を受信できるか？
 */
Inline bool_t
mu_getready(SIOPCB *p_siopcb)
{
    return((sil_rew_mem((uint32_t *)MU_LSR) & MU_LSR_DATA_RDY_BIT) ==
            MU_LSR_DATA_RDY_BIT);
}

/*
 *  文字を送信できるか？
 */
Inline bool_t
mu_putready(SIOPCB *p_siopcb)
{
    return((sil_rew_mem((uint32_t *)MU_LSR) & MU_LSR_TX_EMPTY_BIT) ==
            MU_LSR_TX_EMPTY_BIT);
}

/*
 *  受信した文字の取出し
 */
Inline char
mu_getchar(SIOPCB *p_siopcb)
{
    return((char)(sil_rew_mem((uint32_t *)MU_IO) & MU_IO_DATA_MASK));
}

/*
 *  送信する文字の書込み
 */
Inline void
mu_putchar(SIOPCB *p_siopcb, char c)
{
    sil_wrw_mem((uint32_t *)MU_IO, c);
}

/*
 *  送信割込み許可
 */
Inline void
mu_enable_send(SIOPCB *p_siopcb)
{
    sil_wrw_mem((uint32_t *)MU_IER,
            sil_rew_mem((uint32_t *)MU_IER) | MU_IER_EN_TX_INT_BIT);
}

/*
 *  送信割込み禁止
 */
Inline void
mu_disable_send(SIOPCB *p_siopcb)
{
    sil_wrw_mem((uint32_t *)MU_IER,
            sil_rew_mem((uint32_t *)MU_IER) & ~MU_IER_EN_TX_INT_BIT);
}

/*
 *  受信割込み許可
 */
Inline void
mu_enable_rcv(SIOPCB *p_siopcb)
{
    sil_wrw_mem((uint32_t *)MU_IER,
            sil_rew_mem((uint32_t *)MU_IER) | MU_IER_EN_RX_INT_BIT);
}

/*
 *  受信割込み禁止
 */
Inline void
mu_disable_rcv(SIOPCB *p_siopcb)
{
    sil_wrw_mem((uint32_t *)MU_IER,
            sil_rew_mem((uint32_t *)MU_IER) & ~MU_IER_EN_RX_INT_BIT);
}

/*
 * Put a character in polling mode
 */
void
mu_pol_putc(ID siopid, char c)
{
    while ((sil_rew_mem((uint32_t *)MU_LSR) & MU_LSR_TX_EMPTY_BIT)
            != MU_LSR_TX_EMPTY_BIT);
    sil_wrw_mem((uint32_t *)MU_IO, c);
}

/*
 *  シリアルI/Oポートからの送信可能コールバック
 */
void
mu_irdy_snd(intptr_t exinf)
{
    sio_irdy_snd(exinf);
}

/*
 *  シリアルI/Oポートからの受信通知コールバック
 */
void
mu_irdy_rcv(intptr_t exinf)
{
    sio_irdy_rcv(exinf);
}

/*
 *  SIOドライバの初期化
 */
void
sio_initialize(intptr_t exinf)
{
    SIOPCB *p_siopcb;
    uint_t i = (uint_t)exinf;

    /*
     *  シリアルI/Oポート管理ブロックの初期化
     */
    p_siopcb = &(siopcb_table[i]);
    p_siopcb->siopinib = &(siopinib_table[i]);
    p_siopcb->openflag = false;
    p_siopcb->sendflag = false;
}

/*
 *  シリアルI/Oポートのオープン
 */
SIOPCB *
sio_opn_por(ID siopid, intptr_t exinf)
{
    SIOPCB    *p_siopcb;
    ER        ercd;

    /*
     *  シリアルI/O割込みをマスクする．
     */
    p_siopcb = get_siopcb(siopid);
    ercd = dis_int(p_siopcb->siopinib->intno);
    assert(ercd == E_OK);

    /*
     *  デバイス依存のオープン処理．
     */
    mu_reset(siopid);
    p_siopcb->exinf = exinf;
    p_siopcb->getready = false;
    p_siopcb->putready = false;
    p_siopcb->openflag = true;

    /*
     *  シリアルI/O割込みのマスクを解除する．
     */
    ercd = ena_int(p_siopcb->siopinib->intno);
    assert(ercd == E_OK);

    return(p_siopcb);
}

/*
 *  シリアルI/Oポートのクローズ
 */
void
sio_cls_por(SIOPCB *p_siopcb)
{
    ER ercd;

    /*
     *  デバイス依存のクローズ処理．
     */
    p_siopcb->openflag = false;

    /*
     *  シリアルI/O割込みをマスクする．
     */
    ercd = dis_int(p_siopcb->siopinib->intno);
    assert(ercd == E_OK);
}

/*
 *  SIOの割込みハンドラ
 */
void
sio_isr(intptr_t exinf)
{
    SIOPCB *p_siopcb = get_siopcb(exinf);

    if (mu_getready(p_siopcb)) {
        /*
         *  受信通知コールバックルーチンを呼び出す．
         */
        mu_irdy_rcv(p_siopcb->exinf);
    }
    if (mu_putready(p_siopcb)) {
        /*
         *  送信可能コールバックルーチンを呼び出す．
         */
        mu_irdy_snd(p_siopcb->exinf);
    }
}

/*
 *  シリアルI/Oポートへの文字送信
 */
bool_t
sio_snd_chr(SIOPCB *p_siopcb, char c)
{
    if (mu_putready(p_siopcb)){
        mu_putchar(p_siopcb, c);
        return(true);
    }
    return(false);
}

/*
 *  シリアルI/Oポートからの文字受信
 */
int_t
sio_rcv_chr(SIOPCB *p_siopcb)
{
    if (mu_getready(p_siopcb)) {
        return((int_t)(uint8_t) mu_getchar(p_siopcb));
    }
    return(-1);
}

/*
 *  シリアルI/Oポートからのコールバックの許可
 */
void
sio_ena_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
      case SIO_RDY_SND:
        mu_enable_send(p_siopcb);
        break;
      case SIO_RDY_RCV:
        mu_enable_rcv(p_siopcb);
        break;
    }
}

/*
 *  シリアルI/Oポートからのコールバックの禁止
 */
void
sio_dis_cbr(SIOPCB *p_siopcb, uint_t cbrtn)
{
    switch (cbrtn) {
      case SIO_RDY_SND:
        mu_disable_send(p_siopcb);
        break;
      case SIO_RDY_RCV:
        mu_disable_rcv(p_siopcb);
        break;
    }
}
