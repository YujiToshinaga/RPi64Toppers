#include "baremetal.h"

    .global	_loader_stack
    .align STACK_ALIGN

    .section .loader_stack, "aw", %nobits

_loader_stack:
    .space STACK_SIZE, 0

