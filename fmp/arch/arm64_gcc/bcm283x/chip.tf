$
$	 パス2のチップ依存テンプレート（BCM283X用）
$

$
$  有効な割込み番号，割込みハンドラ番号
$
$INTNO_VALID = {
	RANGE(TMIN_INTNO, TMAX_INTNO);
	RANGE(0x10000 | TMIN_INTNO, 0x10000 | TMAX_INTNO);
	RANGE(0x20000 | TMIN_INTNO, 0x20000 | TMAX_INTNO);
	RANGE(0x30000 | TMIN_INTNO, 0x30000 | TMAX_INTNO);
	RANGE(0x40000 | TMIN_INTNO, 0x40000 | TMAX_INTNO);
	RANGE(0x50000 | TMIN_INTNO, 0x50000 | TMAX_INTNO);
	RANGE(0x60000 | TMIN_INTNO, 0x60000 | TMAX_INTNO)
}$
$INHNO_VALID = {
	RANGE(TMIN_INTNO, TMAX_INTNO);
	RANGE(0x10000 | TMIN_INTNO, 0x10000 | TMAX_INTNO);
	RANGE(0x20000 | TMIN_INTNO, 0x20000 | TMAX_INTNO);
	RANGE(0x30000 | TMIN_INTNO, 0x30000 | TMAX_INTNO);
	RANGE(0x40000 | TMIN_INTNO, 0x40000 | TMAX_INTNO);
	RANGE(0x50000 | TMIN_INTNO, 0x50000 | TMAX_INTNO);
	RANGE(0x60000 | TMIN_INTNO, 0x60000 | TMAX_INTNO)
}$

$
$  ATT_ISRで使用できる割込み番号とそれに対応する割込みハンドラ番号
$
$INTNO_ATTISR_VALID = INTNO_VALID$
$INHNO_ATTISR_VALID = INHNO_VALID$

$
$  DEF_INTで使用できる割込みハンドラ番号
$
$INHNO_DEFINH_VALID = INHNO_VALID$

$
$  CFG_INTで使用できる割込み番号と割込み優先度
$
$INTNO_CFGINT_VALID  = INTNO_VALID$
$INTPRI_CFGINT_VALID = RANGE(TMIN_INTPRI, -1)$

$
$ ATT_ISRのターゲット依存のチェック
$ どのプロセッサでも受け付け可能な割込み番号は，初期割付け可能プロセッサにのみ
$ 割付け可能である．
$
$FUNCTION TARGET_CHECK_ATT_ISR$
	$IF ((ISR.INTNO[ARGV[1]] & 0xffff0000) == 0)
		&& ((1 << (CLASS_AFFINITY_INI[ISR.CLASS[ARGV[1]]] - 1)) != CLASS_AFFINITY_MASK[ISR.CLASS[ARGV[1]]])$
		$ERROR ISR.TEXT_LINE[order]$E_RSATR: $FORMAT(_("processor affinity of `0x%1$x\' in %2% is not match with class %3% processor affinity mask `0x%4$x\'."), +ISR.INTNO[ARGV[1]], "ATT_ISR", ISR.CLASS[ARGV[1]], +CLASS_AFFINITY_MASK[ISR.CLASS[ARGV[1]]])$$END$
	$END$
$END$

$
$  ARM64依存テンプレートのインクルード
$
$INCLUDE"../common/core.tf"$

$
$  割込みハンドラテーブル
$
$FOREACH prcid RANGE(1, TNUM_PRCID)$

const FP _kernel_prc$prcid$_inh_table[TNUM_INH] = {$NL$
$FOREACH inhno RANGE(TMIN_INTNO, TMAX_INTNO)$
	$minhno = inhno | (prcid << intno_cpu_shift)$
	$IF LENGTH(INH.INHNO[inhno]) && (CLASS_AFFINITY_INI[INH.CLASS[inhno]] == prcid)$
		$TAB$(FP)($INH.INTHDR[inhno]$),
	$ELSE$
		$IF LENGTH(INH.INHNO[minhno]) && (CLASS_AFFINITY_INI[INH.CLASS[minhno]] == prcid)$
			$TAB$(FP)($INH.INTHDR[minhno]$),
		$ELSE$
			$TAB$(FP)(_kernel_default_int_handler),
		$END$
	$END$
	$SPC$$FORMAT("/* 0x%x */", +minhno)$$NL$
$END$
};$NL$
$NL$

$END$$NL$

$
$  割込みハンドラアクセステーブル
$
const FP* const _kernel_p_inh_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$_kernel_prc$prcid$_inh_table
$END$
$NL$};$NL$
$NL$

$
$  割込み属性テーブル
$
$NL$
$FOREACH prcid RANGE(1, TNUM_PRCID)$

const uint8_t _kernel_prc$prcid$_cfgint_table[TNUM_INH] = {$NL$
$FOREACH inhno RANGE(TMIN_INTNO, TMAX_INTNO)$
	$minhno = inhno | (prcid << intno_cpu_shift)$
	$IF LENGTH(INH.INHNO[inhno]) && (CLASS_AFFINITY_INI[INH.CLASS[inhno]] == prcid)$
		$TAB$1U,
	$ELSE$
		$IF LENGTH(INH.INHNO[minhno]) && (CLASS_AFFINITY_INI[INH.CLASS[minhno]] == prcid)$
			$TAB$1U,
		$ELSE$
			$TAB$0U,
		$END$
	$END$
	$SPC$$FORMAT("/* 0x%x */", +minhno)$$NL$
$END$
};$NL$
$NL$

$END$$NL$

$
$  割込み属性アクセステーブル
$
const uint8_t* const _kernel_p_cfgint_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$_kernel_prc$prcid$_cfgint_table
$END$
$NL$};$NL$
$NL$

$
$  割込み優先度テーブル
$
$FOREACH prcid RANGE(1, TNUM_PRCID)$

const PRI _kernel_prc$prcid$_inh_iipm_table[TNUM_INH] = {$NL$
$FOREACH inhno RANGE(TMIN_INTNO, TMAX_INTNO)$
	$minhno = inhno | (prcid << intno_cpu_shift)$
	$IF LENGTH(INH.INHNO[inhno]) && (CLASS_AFFINITY_INI[INH.CLASS[inhno]] == prcid)$
		$TAB$$INT.INTPRI[inhno]$,
	$ELSE$
		$IF LENGTH(INH.INHNO[minhno]) && (CLASS_AFFINITY_INI[INH.CLASS[minhno]] == prcid)$
			$TAB$$INT.INTPRI[minhno]$,
		$ELSE$
			$TAB$0,
		$END$
	$END$
	$SPC$$FORMAT("/* 0x%x */", +minhno)$$NL$
$END$
};$NL$
$NL$

$END$$NL$

$
$  割込み優先度アクセステーブル
$
const PRI* const _kernel_p_inh_iipm_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$_kernel_prc$prcid$_inh_iipm_table
$END$
$NL$};$NL$
$NL$

$
$  割込みマスクテーブル
$
$FOREACH prcid RANGE(1, TNUM_PRCID)$

const uint32_t _kernel_prc$prcid$_iipm_mask_table[(TNUM_INTPRI + 1) * 4] = {$NL$
$FOREACH intpri RANGE(TMIN_INTPRI, 0)$
	$intmaskc = 0$
	$intmaskb = 0$
	$intmask1 = 0$
	$intmask2 = 0$
	$FOREACH inhno RANGE(TMIN_INTNO, TMAX_INTNO)$
		$minhno = inhno | (prcid << intno_cpu_shift)$
		$IF LENGTH(INH.INHNO[inhno]) && (CLASS_AFFINITY_INI[INH.CLASS[inhno]] == prcid)$
			$IF INT.INTPRI[inhno] >= intpri$
				$IF inhno < 32$
					$intmaskc = intmaskc | (0x1 << (inhno - 0))$
				$ELSE$
					$IF inhno < 64$
						$intmaskb = intmaskb | (0x1 << (inhno - 32))$
					$ELSE$
						$IF inhno < 96$
							$intmask1 = intmask1 | (0x1 << (inhno - 64))$
						$ELSE$
							$intmask2 = intmask2 | (0x1 << (inhno - 96))$
						$END$
					$END$
				$END$
			$END$
		$END$
		$IF LENGTH(INH.INHNO[minhno]) && (CLASS_AFFINITY_INI[INH.CLASS[minhno]] == prcid)$
			$IF INT.INTPRI[minhno] >= intpri$
				$IF inhno < 32$
					$intmaskc = intmaskc | (0x1 << (inhno - 0))$
				$ELSE$
					$IF inhno < 64$
						$intmaskb = intmaskb | (0x1 << (inhno - 32))$
					$ELSE$
						$IF inhno < 96$
							$intmask1 = intmask1 | (0x1 << (inhno - 64))$
						$ELSE$
							$intmask2 = intmask2 | (0x1 << (inhno - 96))$
						$END$
					$END$
				$END$
			$END$
		$END$
	$END$
	$TAB$$FORMAT("0x%08x", intmaskc)$, $FORMAT("0x%08x", intmaskb)$, $FORMAT("0x%08x", intmask1)$, $FORMAT("0x%08x", intmask2)$, /* Priority $+intpri$ */$NL$
$END$
};$NL$
$NL$

$END$$NL$

$
$  割込みマスクアクセステーブル
$
const uint32_t* const _kernel_p_iipm_mask_table[TNUM_PRCID] = {$NL$
$JOINEACH prcid RANGE(1, TNUM_PRCID) ",\n"$
	$TAB$_kernel_prc$prcid$_iipm_mask_table
$END$
$NL$};$NL$
$NL$

