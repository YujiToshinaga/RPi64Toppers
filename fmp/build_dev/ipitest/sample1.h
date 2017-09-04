#include "target_test.h"

#define TASK1_PRIORITY	9
#define MAIN_PRIORITY	5

#ifndef TASK_PORTID_PRC1
#define	TASK_PORTID_PRC1		1
#endif /* TASK_PORTID_PRC1 */

#ifndef TASK_PORTID_PRC2
#define	TASK_PORTID_PRC2		2
#endif /* TASK_PORTID_PRC2 */

#ifndef TASK_PORTID_PRC3
#define	TASK_PORTID_PRC3		3
#endif /* TASK_PORTID_PRC3 */

#ifndef TASK_PORTID_PRC4
#define	TASK_PORTID_PRC4		4
#endif /* TASK_PORTID_PRC4 */

#ifndef TASK_PORTID_G_SYSLOG
#define	TASK_PORTID_G_SYSLOG	1
#endif /* TASK_PORTID_G_SYSLOG */

#ifndef STACK_SIZE
#define	STACK_SIZE		4096
#endif /* STACK_SIZE */

#ifndef TOPPERS_MACRO_ONLY

extern void	cpuexc_handler(void *p_excinf);
extern void	cyclic_handler(intptr_t exinf);
extern void	task1(intptr_t exinf);
extern void	main_task(intptr_t exinf);

#endif /* TOPPERS_MACRO_ONLY */
