/* This file is generated from kernel_rename.def by genrename. */

/* This file is included only when kernel_rename.h has been included. */
#ifdef TOPPERS_KERNEL_RENAME_H
#undef TOPPERS_KERNEL_RENAME_H

/*
 *  startup.c
 */
#undef exit_kernel

/*
 *  mp.c
 */
#undef giant_lock
#undef ext_ker_reqflg
#undef ipi_handler
#undef dispatch_request
#undef ext_ker_request
#undef initialize_pcb
#undef barrier_sync
#undef t_acquire_tsk_lock_prcid
#undef t_acquire_tsk_lock_self
#undef t_acquire_tsk_lock_self_without_runnable_check
#undef t_acquire_nested_tsk_lock_self
#undef t_acquire_tsk_lock
#undef t_acquire_tsk_lock_alm
#undef t_acquire_tsk_lock_cyc
#undef i_acquire_tsk_lock_prcid
#undef i_acquire_tsk_lock_self
#undef i_acquire_tsk_lock
#undef i_acquire_tsk_lock_alm
#undef t_acquire_obj_lock
#undef i_acquire_obj_lock
#undef release_tsk_lock
#undef release_nested_tsk_lock
#undef release_dual_tsk_lock
#undef release_obj_lock
#undef acquire_tsk_lock_without_preemption_self
#undef acquire_nested_tsk_lock_without_preemption
#undef t_acquire_nested_tsk_lock
#undef t_acquire_dual_tsk_lock
#undef t_acquire_dual_tsk_lock_alm
#undef t_acquire_dual_tsk_lock_cyc
#undef t_acquire_nested_dual_tsk_lock
#undef i_acquire_nested_tsk_lock
#undef i_acquire_dual_tsk_lock
#undef i_acquire_dual_tsk_lock_alm

/*
 *  task.c
 */
#undef initialize_task
#undef search_schedtsk
#undef make_runnable
#undef make_non_runnable
#undef make_dormant
#undef make_active
#undef change_priority
#undef rotate_ready_queue
#undef call_texrtn
#undef dispatch_call_texrtn
#undef calltex
#undef migrate_self
#undef exit_and_migrate_self
#undef set_lowest_precedence

/*
 *  wait.c
 */
#undef make_wait_tmout
#undef wait_complete
#undef wait_tmout
#undef wait_tmout_ok
#undef wait_release
#undef wobj_make_wait
#undef wobj_make_wait_tmout
#undef init_wait_queue

/*
 *  time_event.c
 */
#undef current_time
#undef min_time
#undef next_time
#undef next_subtime
#undef last_index
#undef initialize_tmevt
#undef tmevt_up
#undef tmevt_down
#undef tmevtb_insert
#undef tmevtb_delete
#undef tmevt_lefttim
#undef signal_time
#undef get_my_current_time

/*
 *  semaphore.c
 */
#undef initialize_semaphore

/*
 *  eventflag.c
 */
#undef initialize_eventflag
#undef check_flg_cond

/*
 *  dataqueue.c
 */
#undef initialize_dataqueue
#undef enqueue_data
#undef force_enqueue_data
#undef dequeue_data
#undef send_data
#undef force_send_data
#undef receive_data

/*
 *  pridataq.c
 */
#undef initialize_pridataq
#undef enqueue_pridata
#undef dequeue_pridata
#undef send_pridata
#undef receive_pridata

/*
 *  mailbox.c
 */
#undef initialize_mailbox

/*
 *  mempfix.c
 */
#undef initialize_mempfix
#undef get_mpf_block

/*
 *  cyclic.c
 */
#undef initialize_cyclic
#undef call_cychdr

/*
 *  alarm.c
 */
#undef initialize_alarm
#undef call_almhdr

/*
 *  spin_lock.c
 */
#undef initialize_spin_lock
#undef force_unlock_spin

/*
 *  interrupt.c
 */
#undef initialize_interrupt

/*
 *  exception.c
 */
#undef initialize_exception

/*
 *  kernel_cfg.c
 */
#undef p_pcb_table
#undef initialize_object
#undef call_global_inirtn
#undef call_local_inirtn
#undef call_global_terrtn
#undef call_local_terrtn
#undef tmax_tskid
#undef tinib_table
#undef torder_table
#undef tcb_table
#undef p_tcb_table
#undef tmax_semid
#undef seminib_table
#undef semcb_table
#undef p_semcb_table
#undef tmax_flgid
#undef flginib_table
#undef flgcb_table
#undef p_flgcb_table
#undef tmax_dtqid
#undef dtqcb_table
#undef dtqinib_table
#undef p_dtqcb_table
#undef tmax_pdqid
#undef pdqcb_table
#undef pdqinib_table
#undef p_pdqcb_table
#undef tmax_mbxid
#undef mbxinib_table
#undef p_mbxcb_table
#undef tmax_mpfid
#undef mpfinib_table
#undef mpfcb_table
#undef p_mpfcb_table
#undef tmax_cycid
#undef cycinib_table
#undef cyccb_table
#undef p_cyccb_table
#undef tmax_almid
#undef alminib_table
#undef almcb_table
#undef p_almcb_table
#undef tmax_spnid
#undef spninib_table
#undef spncb_table
#undef p_spncb_table
#undef tnum_inhno
#undef inhinib_table
#undef tnum_intno
#undef intinib_table
#undef tnum_excno
#undef excinib_table
#undef tmevt_heap
#undef p_tevtcb_table
#undef p_tmevt_heap_table
#undef tevtcb
#undef istksz_table
#undef istk_table
#undef istkpt_table

/*
 *  syslog.c
 */
#undef p_syslogcb_table


#ifdef TOPPERS_LABEL_ASM

/*
 *  startup.c
 */
#undef _exit_kernel

/*
 *  mp.c
 */
#undef _giant_lock
#undef _ext_ker_reqflg
#undef _ipi_handler
#undef _dispatch_request
#undef _ext_ker_request
#undef _initialize_pcb
#undef _barrier_sync
#undef _t_acquire_tsk_lock_prcid
#undef _t_acquire_tsk_lock_self
#undef _t_acquire_tsk_lock_self_without_runnable_check
#undef _t_acquire_nested_tsk_lock_self
#undef _t_acquire_tsk_lock
#undef _t_acquire_tsk_lock_alm
#undef _t_acquire_tsk_lock_cyc
#undef _i_acquire_tsk_lock_prcid
#undef _i_acquire_tsk_lock_self
#undef _i_acquire_tsk_lock
#undef _i_acquire_tsk_lock_alm
#undef _t_acquire_obj_lock
#undef _i_acquire_obj_lock
#undef _release_tsk_lock
#undef _release_nested_tsk_lock
#undef _release_dual_tsk_lock
#undef _release_obj_lock
#undef _acquire_tsk_lock_without_preemption_self
#undef _acquire_nested_tsk_lock_without_preemption
#undef _t_acquire_nested_tsk_lock
#undef _t_acquire_dual_tsk_lock
#undef _t_acquire_dual_tsk_lock_alm
#undef _t_acquire_dual_tsk_lock_cyc
#undef _t_acquire_nested_dual_tsk_lock
#undef _i_acquire_nested_tsk_lock
#undef _i_acquire_dual_tsk_lock
#undef _i_acquire_dual_tsk_lock_alm

/*
 *  task.c
 */
#undef _initialize_task
#undef _search_schedtsk
#undef _make_runnable
#undef _make_non_runnable
#undef _make_dormant
#undef _make_active
#undef _change_priority
#undef _rotate_ready_queue
#undef _call_texrtn
#undef _dispatch_call_texrtn
#undef _calltex
#undef _migrate_self
#undef _exit_and_migrate_self
#undef _set_lowest_precedence

/*
 *  wait.c
 */
#undef _make_wait_tmout
#undef _wait_complete
#undef _wait_tmout
#undef _wait_tmout_ok
#undef _wait_release
#undef _wobj_make_wait
#undef _wobj_make_wait_tmout
#undef _init_wait_queue

/*
 *  time_event.c
 */
#undef _current_time
#undef _min_time
#undef _next_time
#undef _next_subtime
#undef _last_index
#undef _initialize_tmevt
#undef _tmevt_up
#undef _tmevt_down
#undef _tmevtb_insert
#undef _tmevtb_delete
#undef _tmevt_lefttim
#undef _signal_time
#undef _get_my_current_time

/*
 *  semaphore.c
 */
#undef _initialize_semaphore

/*
 *  eventflag.c
 */
#undef _initialize_eventflag
#undef _check_flg_cond

/*
 *  dataqueue.c
 */
#undef _initialize_dataqueue
#undef _enqueue_data
#undef _force_enqueue_data
#undef _dequeue_data
#undef _send_data
#undef _force_send_data
#undef _receive_data

/*
 *  pridataq.c
 */
#undef _initialize_pridataq
#undef _enqueue_pridata
#undef _dequeue_pridata
#undef _send_pridata
#undef _receive_pridata

/*
 *  mailbox.c
 */
#undef _initialize_mailbox

/*
 *  mempfix.c
 */
#undef _initialize_mempfix
#undef _get_mpf_block

/*
 *  cyclic.c
 */
#undef _initialize_cyclic
#undef _call_cychdr

/*
 *  alarm.c
 */
#undef _initialize_alarm
#undef _call_almhdr

/*
 *  spin_lock.c
 */
#undef _initialize_spin_lock
#undef _force_unlock_spin

/*
 *  interrupt.c
 */
#undef _initialize_interrupt

/*
 *  exception.c
 */
#undef _initialize_exception

/*
 *  kernel_cfg.c
 */
#undef _p_pcb_table
#undef _initialize_object
#undef _call_global_inirtn
#undef _call_local_inirtn
#undef _call_global_terrtn
#undef _call_local_terrtn
#undef _tmax_tskid
#undef _tinib_table
#undef _torder_table
#undef _tcb_table
#undef _p_tcb_table
#undef _tmax_semid
#undef _seminib_table
#undef _semcb_table
#undef _p_semcb_table
#undef _tmax_flgid
#undef _flginib_table
#undef _flgcb_table
#undef _p_flgcb_table
#undef _tmax_dtqid
#undef _dtqcb_table
#undef _dtqinib_table
#undef _p_dtqcb_table
#undef _tmax_pdqid
#undef _pdqcb_table
#undef _pdqinib_table
#undef _p_pdqcb_table
#undef _tmax_mbxid
#undef _mbxinib_table
#undef _p_mbxcb_table
#undef _tmax_mpfid
#undef _mpfinib_table
#undef _mpfcb_table
#undef _p_mpfcb_table
#undef _tmax_cycid
#undef _cycinib_table
#undef _cyccb_table
#undef _p_cyccb_table
#undef _tmax_almid
#undef _alminib_table
#undef _almcb_table
#undef _p_almcb_table
#undef _tmax_spnid
#undef _spninib_table
#undef _spncb_table
#undef _p_spncb_table
#undef _tnum_inhno
#undef _inhinib_table
#undef _tnum_intno
#undef _intinib_table
#undef _tnum_excno
#undef _excinib_table
#undef _tmevt_heap
#undef _p_tevtcb_table
#undef _p_tmevt_heap_table
#undef _tevtcb
#undef _istksz_table
#undef _istk_table
#undef _istkpt_table

/*
 *  syslog.c
 */
#undef _p_syslogcb_table


#endif /* TOPPERS_LABEL_ASM */

#include "target_unrename.h"

#endif /* TOPPERS_KERNEL_RENAME_H */
