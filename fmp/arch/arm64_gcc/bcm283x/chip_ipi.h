/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2009 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_ipi.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  プロセッサ間割込みドライバ（BCM283X用）
 */

#ifndef TOPPERS_CHIP_IPI_H
#define TOPPERS_CHIP_IPI_H

/*
 *  Numbers for software generated interrupts SGI
 */
//#ifdef TOPPERS_SAFEG_SECURE
//#define IPINO_DIS  GIC_IRQNO_IPI14
//#define IPINO_EXT  GIC_IRQNO_IPI15
//#else  /* !TOPPERS_SAFEG_SECURE */
//#define IPINO_DIS  GIC_IRQNO_IPI0
//#define IPINO_EXT  GIC_IRQNO_IPI1
//#endif /* TOPPERS_SAFEG_SECURE */
#define IPINO_DIS_PRC1  IRQNO_MBOX0
#define IPINO_DIS_PRC2  IRQNO_MBOX1
#define IPINO_DIS_PRC3  IRQNO_MBOX2
#define IPINO_DIS_PRC4  IRQNO_MBOX3
#define IPINO_EXT_PRC1  IRQNO_MBOX0
#define IPINO_EXT_PRC2  IRQNO_MBOX1
#define IPINO_EXT_PRC3  IRQNO_MBOX2
#define IPINO_EXT_PRC4  IRQNO_MBOX3

/*
 *  Priority for inter-processor interrupts used for dispatching requests
 */
//#ifdef TOPPERS_SAFEG_SECURE
//#define INTPRI_IPI_DIS_PRC1   (-16)
//#define INTPRI_IPI_DIS_PRC2   (-16)
//#define INTPRI_IPI_DIS_PRC3   (-16)
//#define INTPRI_IPI_DIS_PRC4   (-16)
//#else  /* !TOPPERS_SAFEG_SECURE */
#define INTPRI_IPI_DIS_PRC1   (-2)
#define INTPRI_IPI_DIS_PRC2   (-2)
#define INTPRI_IPI_DIS_PRC3   (-2)
#define INTPRI_IPI_DIS_PRC4   (-2)
//#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  Priority for inter-processor interrupts used when the kernel ends
 */
//#ifdef TOPPERS_SAFEG_SECURE
//#define INTPRI_IPI_EXT_PRC1   (-31)
//#define INTPRI_IPI_EXT_PRC2   (-31)
//#define INTPRI_IPI_EXT_PRC3   (-31)
//#define INTPRI_IPI_EXT_PRC4   (-31)
//#else  /* !TOPPERS_SAFEG_SECURE */
#define INTPRI_IPI_EXT_PRC1   (-15)
#define INTPRI_IPI_EXT_PRC2   (-15)
#define INTPRI_IPI_EXT_PRC3   (-15)
#define INTPRI_IPI_EXT_PRC4   (-15)
//#endif /* TOPPERS_SAFEG_SECURE */

/*
 *  ディスパッチ要求用プロセッサ間割込みの割込み番号
 */
#define INTNO_IPI_DIS_PRC1   (0x00010000 | IPINO_DIS_PRC1)
#define INTNO_IPI_DIS_PRC2   (0x00020000 | IPINO_DIS_PRC2)
#define INTNO_IPI_DIS_PRC3   (0x00030000 | IPINO_DIS_PRC3)
#define INTNO_IPI_DIS_PRC4   (0x00040000 | IPINO_DIS_PRC4)

/*
 *  カーネル終了用のプロセッサ間割込みの割込み番号
 */
#define INTNO_IPI_EXT_PRC1   (0x00010000 | IPINO_EXT_PRC1)
#define INTNO_IPI_EXT_PRC2   (0x00020000 | IPINO_EXT_PRC2)
#define INTNO_IPI_EXT_PRC3   (0x00030000 | IPINO_EXT_PRC3)
#define INTNO_IPI_EXT_PRC4   (0x00040000 | IPINO_EXT_PRC4)

/*
 *  ディスパッチ要求用のプロセッサ間割込みの割込みハンドラ番号
 */
#define INHNO_IPI_DIS_PRC1   (0x00010000 | IPINO_DIS_PRC1)
#define INHNO_IPI_DIS_PRC2   (0x00020000 | IPINO_DIS_PRC2)
#define INHNO_IPI_DIS_PRC3   (0x00030000 | IPINO_DIS_PRC3)
#define INHNO_IPI_DIS_PRC4   (0x00040000 | IPINO_DIS_PRC4)

/*
 *  カーネル終了用のプロセッサ間割込みの割込みハンドラ番号
 */
#define INHNO_IPI_EXT_PRC1   (0x00010000 | IPINO_EXT_PRC1)
#define INHNO_IPI_EXT_PRC2   (0x00020000 | IPINO_EXT_PRC2)
#define INHNO_IPI_EXT_PRC3   (0x00030000 | IPINO_EXT_PRC3)
#define INHNO_IPI_EXT_PRC4   (0x00040000 | IPINO_EXT_PRC4)

#ifndef TOPPERS_MACRO_ONLY

/*
 *  ディスパッチ用プロセッサ間割込みの発行
 */
Inline void
target_ipi_raise(uint_t prcid)
{
	switch(prcid) {
	case 1:
		sil_wrw_mem((void *)CORE0_MBOX0_WS, 0x00000001U);
		break;
	case 2:
		sil_wrw_mem((void *)CORE1_MBOX1_WS, 0x00000001U);
		break;
	case 3:
		sil_wrw_mem((void *)CORE2_MBOX2_WS, 0x00000001U);
		break;
	case 4:
		sil_wrw_mem((void *)CORE3_MBOX3_WS, 0x00000001U);
		break;
	}
}

/*
 *  ディスパッチ用プロセッサ間割込みのクリア
 */
Inline void
target_ipi_clear(void)
{
	switch(x_prc_index() + 1) {
	case 1:
		sil_wrw_mem((void *)CORE0_MBOX0_RC, 0xffffffffU);
		break;
	case 2:
		sil_wrw_mem((void *)CORE1_MBOX1_RC, 0xffffffffU);
		break;
	case 3:
		sil_wrw_mem((void *)CORE2_MBOX2_RC, 0xffffffffU);
		break;
	case 4:
		sil_wrw_mem((void *)CORE3_MBOX3_RC, 0xffffffffU);
		break;
	}
}

#ifdef USE_IPI_DIS_HANDER_BYPASS
/*
 *  カーネル終了処理用のプロセッサ間割込みハンドラ
 */
extern void _kernel_ipi_ext_handler(void);
#endif /* USE_IPI_DIS_HANDER_BYPASS */

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CHIP_IPI_H */
