#include <kernel.h>
#include <t_syslog.h>
#include <t_stdlib.h>
#include <sil.h>
#include "syssvc/serial.h"
#include "syssvc/syslog.h"
#include "kernel_cfg.h"
#include "sample1.h"

uint_t const serial_port[TNUM_PRCID] = {
#ifndef G_SYSLOG
	TASK_PORTID_PRC1,
#else /* G_SYSLOG */
	TASK_PORTID_G_SYSLOG,
#endif /* G_SYSLOG */
#if TNUM_PRCID >= 2
    TASK_PORTID_PRC2,
#endif /* TNUM_PRCID >= 2 */
#if TNUM_PRCID >= 3
    TASK_PORTID_PRC3,
#endif /* TNUM_PRCID >= 3 */
#if TNUM_PRCID >= 4
    TASK_PORTID_PRC4,
#endif /* TNUM_PRCID >= 4 */
};

uint_t const task1_id[TNUM_PRCID] = {TASK1_1, TASK1_2, TASK1_3, TASK1_4};
uint_t const cychdr_id[TNUM_PRCID] = {CYCHDR_1, CYCHDR_2, CYCHDR_3, CYCHDR_4};

void cpuexc_handler(void *p_excinf)
{
	uint64_t pc;

	syslog(LOG_NOTICE, "CPU exception handler (p_excinf = %08p).", p_excinf);

	pc = x_get_exc_raddr(p_excinf);

	syslog(LOG_NOTICE, "pc= %x", pc);

	for ( ; ; );

	x_set_exc_raddr(p_excinf, pc + 0x04);
}
void cyclic_handler(intptr_t exinf)
{
	iact_tsk(task1_id[(int)exinf - 1]);
}

void task1(intptr_t exinf)
{
	static int cnt[TNUM_PRCID] = {0};

	syslog(LOG_NOTICE, "prc %d : TASK1 %d", (int)exinf, cnt[(int)exinf - 1]);
	cnt[(int)exinf - 1]++;
}

void main_task(intptr_t exinf)
{
	syslog_msk_log(LOG_UPTO(LOG_INFO), LOG_UPTO(LOG_EMERG));
	serial_opn_por((int_t)serial_port[exinf - 1]);
	serial_ctl_por(serial_port[exinf - 1],
			(IOCTL_CRLF | IOCTL_FCSND | IOCTL_FCRCV));

	syslog(LOG_NOTICE, "prc %d : Sample program starts", (int)exinf);

//	RAISE_CPU_EXCEPTION;

	sta_cyc(cychdr_id[(int)exinf - 1]);
	slp_tsk();

	for ( ; ; );

	syslog(LOG_NOTICE, "prc %d : Sample program ends", (int)exinf);
	ext_ker();
	assert(0);
}

