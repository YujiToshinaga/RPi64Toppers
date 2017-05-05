/*
 *  TOPPERS/FMP Kernel
 *      Toyohashi Open Platform for Embedded Real-Time Systems/
 *      Flexible MultiProcessor Kernel
 *
 *  Copyright (C) 2000-2003 by Embedded and Real-Time Systems Laboratory
 *                              Toyohashi Univ. of Technology, JAPAN
 *  Copyright (C) 2006-2012 by Embedded and Real-Time Systems Laboratory
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
 *  @(#) $Id: chip_config_tool.h 1149 2016-01-08 08:19:14Z ertl-toshinaga $
 */

/*
 *  チップ依存モジュールのツール依存部（MPCHIP用）
 *
 *  カーネルのチップ依存部のインクルードファイルのツール依存部．
 *  chip_config.h からインクルードされる．
 */

#ifndef TOPPERS_CHIP_CONFIG_TOOL_H
#define TOPPERS_CHIP_CONFIG_TOOL_H

#ifndef TOPPERS_MACRO_ONLY

//#ifndef USE_GIC_CPULOCK

/*
 *  ロック取得解放関数
 *  x_lock_cpu()/x_unlock_cpu()を使うため，コア依存モジュール（ARM用）を
 *  読み込んでから定義する．
 */

/*
 *  ロック取得失敗時の割込み待ち
 *
 *  MPCoreでは，ロック取得失敗時にスピンを繰り返すのではなく，wfeにより，
 *  イベント待ちとすることが推奨されている．
 *  wfeによるイベント待ちの際，割込み応答性を低下させないため，割込みを許
 *  可した状態て，イベント待ちとするべきである．wfe命令自信は，割込みを禁
 *  止・許可を操作しないため，別途msr命令により割込みを許可する．
 *
 *  その際，msr実行により割込みが許可された直後(wfeを実行する前に)に割込
 *  みが入り，その後割込みハンドラからのリターン後にwfeになり，イベントを
 *  逃す可能性が考えられる．
 *
 *  しかしながら， 割込みハンドラからのリターン(正確には例外一般からのリタ
 *  ーン)を行うと，イベントレジスタがセットされ，wfe実行時にイベントレジス
 *  タがセットされていると，クリアしてwfeから即座にリターンするための問題
 *  ない．
 */

/*
 *  1段目のロック取得（タスク・非タスクコンテキスト共用）
 */
Inline void
x_acquire_lock(LOCK *p_lock)
{
	int		locked;

	while (true) {
		Asm("   mov   w2, #0x01    \n"
			"\t ldxr  w1, [%1]     \n"
			"\t cmp   w1, #0x00    \n"
			"\t b.eq  1f           \n"
			"\t mov   x3, %2       \n" /* 割込みの許可 */
			"\t msr   daif, x3     \n"
			"\t wfe                \n"
			"\t mov   x3, %3       \n" /* 割込みの禁止 */
			"\t msr   daif, x3     \n"
			"\t b     2f           \n"
			"\t 1:                 \n"
			"\t stxr  w1, w2, [%1] \n"
			"\t 2:                 \n"
			"\t mov   %0, x1       \n"
			:"=r"(locked)
			:"r"(p_lock), "I"(DAIF_ALWAYS_SET), "I"(DAIF_CPULOCK|DAIF_ALWAYS_SET)
			:"x1", "x2", "x3", "cc");

		if (locked == 0) {
			/* ロック取得成功 */
			data_memory_barrier();
			Asm("":::"memory");
			return;
		}
	}
}

#define t_acquire_lock(p_lock) x_acquire_lock(p_lock)
#define i_acquire_lock(p_lock) x_acquire_lock(p_lock)

#if TTYPE_KLOCK != G_KLOCK

/*
 *  2段目以上のロック取得（タスク・非タスクコンテキスト共用）
 */
Inline bool_t
x_acquire_nested_lock(LOCK *p_lock)
{
	PCB		*p_pcb;
	int		locked;

	while (true) {
		Asm("   mov   w2, #0x01    \n"
			"\t ldxr  w1, [%1]     \n"
			"\t cmp   w1, #0x00    \n"
			"\t b.eq  1f           \n"
			"\t mov   x3, %2       \n" /* 割込みの許可 */
			"\t msr   daif, x3     \n"
			"\t wfe                \n"
			"\t mov   x3, %3       \n" /* 割込みの禁止 */
			"\t msr   daif, x3     \n"
			"\t b     2f           \n"
			"\t 1:                 \n"
			"\t stxr  w1, w2, [%1] \n"
			"\t 2:                 \n"
			"\t mov   %0, x1       \n"
			:"=r"(locked)
			:"r"(p_lock), "I"(DAIF_ALWAYS_SET), "I"(DAIF_CPULOCK|DAIF_ALWAYS_SET)
			:"x1", "x2", "x3", "cc");

		if (locked == 0) {
			/* ロック取得成功 */
			data_memory_barrier();
			Asm("":::"memory");
			return(false);
		}

		/*
		 * マイグレーションする可能性があるのでここで毎回取得
		 * 非タスクコンテキストの場合，マイグレーションしないため，
		 * while前に実行してもよいが，1回でロックがとれた場合，
		 * 効率が悪いので，ここで取得する．
		 */
		p_pcb = get_my_p_pcb();
		if (p_pcb->p_firstlock == NULL) {
			return(true);
		}
	}
}

#define t_acquire_nested_lock(p_lock) x_acquire_nested_lock(p_lock)
#define i_acquire_nested_lock(p_lock) x_acquire_nested_lock(p_lock)

#endif /* TTYPE_KLOCK != G_KLOCK */

/*
 *  ロックの解放（タスク・非タスクコンテキスト共用）
 */
Inline void
x_release_lock(LOCK *p_lock)
{
	Asm("":::"memory");
	data_memory_barrier();
	*p_lock = 0;
	data_sync_barrier();
	Asm("sev");
}

/*
 *  割込を許可しないロック取得関数
 */
Inline void
x_acquire_lock_without_preemption(LOCK *p_lock)
{
	int		locked;

	while (true) {
		Asm("   mov   w2, #0x01    \n"
			"\t ldxr  w1, [%1]     \n"
			"\t cmp   w1, #0x00    \n"
			"\t b.eq  1f           \n"
			"\t wfe                \n"
			"\t b     2f           \n"
			"\t 1:                 \n"
			"\t stxr  w1, w2, [%1] \n"
			"\t 2:                 \n"
			"\t mov   %0, x1       \n"
			:"=r"(locked)
			:"r"(p_lock)
			:"x1", "x2", "cc");

		if (locked == 0) {
			/* ロック取得成功 */
			data_memory_barrier();
			Asm("":::"memory");
			return;
		}
	}
}

//#else /* USE_GIC_CPULOCK */ 
//
///*
// *  ロック取得解放関数
// *  x_lock_cpu()/x_unlock_cpu()を使うため，コア依存モジュール（ARM用）を
// *  読み込んでから定義する．
// */
//
///*
// *  ロック取得失敗時の割込み待ち
// *
// *  MPCoreでは，ロック取得失敗時にスピンを繰り返すのではなく，wfeにより，
// *  イベント待ちとすることが推奨されている．
// *  wfeによるイベント待ちの際，割込み応答性を低下させないため，割込みを許
// *  可した状態て，イベント待ちとするべきである．wfe命令自信は，割込みを禁
// *  止・許可を操作しないため，別途msr命令により割込みを許可する．
// *
// *  その際，msr実行により割込みが許可された直後(wfeを実行する前に)に割込
// *  みが入り，その後割込みハンドラからのリターン後にwfeになり，イベントを
// *  逃す可能性が考えられる．
// *
// *  しかしながら， 割込みハンドラからのリターン(正確には例外一般からのリタ
// *  ーン)を行うと，イベントレジスタがセットされ，wfe実行時にイベントレジス
// *  タがセットされていると，クリアしてwfeから即座にリターンするための問題
// *  ない．
// */
//
///*
// *  1段目のロック取得（タスク・非タスクコンテキスト共用）
// */
//Inline void
//x_acquire_lock(LOCK *p_lock)
//{
//	int		locked;
//	uint8_t	iipm;
//
//	iipm = gicc_current_priority();
//
//	while (true) {
//		Asm("   mov   w2, #0x01    \n"
//			"\t ldxr  w1, [%1]     \n"
//			"\t cmp   w1, #0x00    \n"
//			"\t b.ne  1f           \n"
//			"\t stxr  w1, w2, [%1] \n"
//			"\t 1:                 \n"
//			"\t mov   %0, x1       \n"
//			:"=r"(locked)
//			:"r"(p_lock)
//			:"x1", "x2", "cc");
//
//		if (locked == 0) {
//			/* ロック取得成功 */
//			data_memory_barrier();
//			Asm("":::"memory");
//			return;
//		} else {
//			/* 一旦割込みを許可にする */
//			gicc_set_priority(GIC_MIN_PRIORITY);
//			gicc_set_priority(iipm);
//		}
//	}
//}
//
//#define t_acquire_lock(p_lock) x_acquire_lock(p_lock)
//#define i_acquire_lock(p_lock) x_acquire_lock(p_lock)
//
//#if TTYPE_KLOCK != G_KLOCK
//
///*
// *  2段目以上のロック取得（タスク・非タスクコンテキスト共用）
// */
//Inline bool_t
//x_acquire_nested_lock(LOCK *p_lock)
//{
//	PCB		*p_pcb;
//	int		locked;
//	uint8_t iipm;
//
//	iipm = gicc_current_priority();
//
//	while (true) {
//		Asm("   mov   w2, #0x01    \n"
//			"\t ldxr  w1, [%1]     \n"
//			"\t cmp   w1, #0x00    \n"
//			"\t b.ne  1f           \n"
//			"\t stxr  w1, w2, [%1] \n"
//			"\t 1:                 \n"
//			"\t mov   %0, x1       \n"
//			:"=r"(locked)
//			:"r"(p_lock)
//			:"x1", "x2", "cc");
//
//		if (locked == 0) {
//			/* ロック取得成功 */
//			data_memory_barrier();
//			Asm("":::"memory");
//			return(false);
//		} else {
//			/* 一旦割込みを許可にする */
//			gicc_set_priority(GIC_MIN_PRIORITY);
//			gicc_set_priority(iipm);
//		}
//
//		/*
//		 * マイグレーションする可能性があるのでここで毎回取得
//		 * 非タスクコンテキストの場合，マイグレーションしないため，
//		 * while前に実行してもよいが，1回でロックがとれた場合，
//		 * 効率が悪いので，ここで取得する．
//		 */
//		p_pcb = get_my_p_pcb();
//		if (p_pcb->p_firstlock == NULL) {
//			return(true);
//		}
//	}
//}
//
//#define t_acquire_nested_lock(p_lock) x_acquire_nested_lock(p_lock)
//#define i_acquire_nested_lock(p_lock) x_acquire_nested_lock(p_lock)
//
//#endif /* TTYPE_KLOCK != G_KLOCK */
//
///*
// *  ロックの解放（タスク・非タスクコンテキスト共用）
// */
//Inline void
//x_release_lock(LOCK *p_lock)
//{
//	Asm("":::"memory");
//	data_memory_barrier();
//	*p_lock = 0;
//	data_sync_barrier();
//	Asm("sev");
//}
//
///*
// *  割込を許可しないロック取得関数
// */
//Inline void
//x_acquire_lock_without_preemption(LOCK *p_lock)
//{
//	int		locked;
//
//	while (true) {
//		Asm("   mov   w2, #0x01    \n"
//			"\t ldxr  w1, [%1]     \n"
//			"\t cmp   w1, #0x00    \n"
//			"\t b.ne  1f           \n"
//			"\t stxr  w1, w2, [%1] \n"
//			"\t 1:                 \n"
//			"\t mov   %0, x1       \n"
//			:"=r"(locked)
//			:"r"(p_lock)
//			:"x1", "x2", "cc");
//
//		if (locked == 0) {
//			/* ロック取得成功 */
//			data_memory_barrier();
//			Asm("":::"memory");
//			return;
//		}
//	}
//}
//
//#endif /* USE_GIC_CPULOCK */ 

/*
 *  スピンロック機能のネイティブロック方式で使用する関数
 */

/*
 *  スピンロックの型
 */
typedef uint32_t SPNLOCK;

/*
 *  スピンロックの初期化
 */
Inline void
x_initialize_spin(ID spnid, SPNLOCK *p_spn_lock)
{
	*p_spn_lock = 0;
}

/*
 *  スピンロックの取得
 */
Inline void
x_lock_spin(SPNLOCK *p_spn_lock)
{
	x_acquire_lock(p_spn_lock);
}

#define t_lock_spin(p_spn_lock)  x_lock_spin(p_spn_lock)
#define i_lock_spin(p_spn_lock)  x_lock_spin(p_spn_lock)

/*
 *  スピンロックの取得の試行
 */
Inline bool_t
x_try_lock_spin(SPNLOCK *p_spn_lock)
{
	int		locked;

	Asm("   mov   w2, #0x01    \n"
		"\t ldxr  w1, [%1]     \n"
		"\t cmp   w1, #0x00    \n"
		"\t b.ne  1f           \n"
		"\t stxr  w1, w2, [%1] \n"
		"\t 1:                 \n"
		"\t mov   %0, x1       \n"
		:"=r"(locked)
		:"r"(p_spn_lock)
		:"x1", "x2", "cc");

	if (locked == 0) {
		/* 成功した場合 */
		data_memory_barrier();
		Asm("":::"memory");
		return(false);
	}

	return(true);
}

/*
 *  スピンロックの返却
 */
Inline void
x_unlock_spin(SPNLOCK *p_spn_lock)
{
	x_release_lock(p_spn_lock);
}

#endif /* TOPPERS_MACRO_ONLY */

#endif /* TOPPERS_CHIP_CONFIG_TOOL_H */
