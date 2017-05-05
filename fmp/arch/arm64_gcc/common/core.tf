$ 
$     パス2のコア依存テンプレート（ARM64用）
$ 

$ 
$ 有効なCPU例外ハンドラ番号
$ 
$EXCNO_VALID = { 
	(0x10000 | EXCH_NO_CUR_SPX_SYNC), (0x10000 | EXCH_NO_CUR_SPX_SERR);
	(0x20000 | EXCH_NO_CUR_SPX_SYNC), (0x20000 | EXCH_NO_CUR_SPX_SERR);
	(0x30000 | EXCH_NO_CUR_SPX_SYNC), (0x30000 | EXCH_NO_CUR_SPX_SERR);
	(0x40000 | EXCH_NO_CUR_SPX_SYNC), (0x40000 | EXCH_NO_CUR_SPX_SERR);
	(0x50000 | EXCH_NO_CUR_SPX_SYNC), (0x50000 | EXCH_NO_CUR_SPX_SERR);
	(0x60000 | EXCH_NO_CUR_SPX_SYNC), (0x60000 | EXCH_NO_CUR_SPX_SERR)
}$

$ 
$ DEF_EXCで使用できるCPU例外ハンドラ番号
$ 
$EXCNO_DEFEXC_VALID = EXCNO_VALID$

$ 
$  標準テンプレートファイルのインクルード
$ 
$INCLUDE "kernel/kernel.tf"$

$ 
$  例外ハンドラテーブル
$ 
$FOREACH prcid RANGE(1, TNUM_PRCID)$

const FP _kernel_prc$prcid$_exch_table[TNUM_EXCH] = {$NL$
$FOREACH excno RANGE(TMIN_EXCH, TMAX_EXCH)$
    $number = excno|(prcid<<16)$
	$IF LENGTH(EXC.EXCNO[number])$
		$TAB$(FP)($EXC.EXCHDR[number]$),
	$ELSE$
		$TAB$(FP)(_kernel_default_exc_handler),
	$END$
	$SPC$$FORMAT("/* 0x%x */", +number)$$NL$
$END$
$NL$};$NL$
$NL$

$END$

$ 
$  例外ハンドラアクセステーブル
$ 
const FP* const _kernel_p_exch_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$_kernel_prc$prcid$_exch_table
$END$
$NL$};$NL$
$NL$

