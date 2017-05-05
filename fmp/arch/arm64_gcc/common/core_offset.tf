$
$       オフセットファイル生成用テンプレートファイル（ARM64用）
$

$
$  標準テンプレートファイルのインクルード
$
$INCLUDE "kernel/genoffset.tf"$

$
$  オフセット値のマクロ定義の生成
$
$DEFINE("TCB_p_tinib", offsetof_TCB_p_tinib)$
$DEFINE("TCB_texptn", offsetof_TCB_texptn)$
$DEFINE("TCB_sp", offsetof_TCB_sp)$
$DEFINE("TCB_pc", offsetof_TCB_pc)$

$DEFINE("TINIB_exinf", offsetof_TINIB_exinf)$
$DEFINE("TINIB_task", offsetof_TINIB_task)$

$DEFINE("PCB_p_runtsk", offsetof_PCB_p_runtsk)$
$DEFINE("PCB_p_schedtsk", offsetof_PCB_p_schedtsk)$
$DEFINE("PCB_reqflg", offsetof_PCB_reqflg)$
$DEFINE("PCB_ipmflg", offsetof_PCB_ipmflg)$
$DEFINE("PCB_disdsp", offsetof_PCB_disdsp)$
$DEFINE("PCB_dspflg", offsetof_PCB_dspflg)$

$DEFINE("EXC_FRAME_cpsr", offsetof_EXC_FRAME_cpsr)$
$DEFINE("EXC_FRAME", sizeof_EXC_FRAME)$

$
$  ビットオフセット値等のマクロ定義の生成
$
$DEFINE_BIT("TCB_enatex", sizeof_TCB, "B")$
