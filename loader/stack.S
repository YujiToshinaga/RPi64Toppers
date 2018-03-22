#include "baremetal.h"

    .global	_stack
    .align STACK_ALIGN

    .section .stack, "aw", %nobits

_stack:
    .space STACK_SIZE, 0

