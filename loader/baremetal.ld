#include "baremetal.h"
#define MACRO_ONLY
#include "bcm283x.h"

OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(_entry)

MEMORY {
    RAM (rwx): ORIGIN = SDRAM_ADDR, LENGTH = SDRAM_SIZE
}

SECTIONS
{
    . = MAIN_ADDR;

    .startup . : {
        __STARTUP_START__ = .;
        startup.o(.startup)
        __STARTUP_END__ = .;
    } > RAM

    .text . : {
        __TEXT_START__ = .;
        *(EXCLUDE_FILE (loader.o) .text*)
        __TEXT_END__ = .;
    } > RAM

    .rodata . : {
        __RODATA_START__ = .;
        *(EXCLUDE_FILE (loader.o) .rodata*)
        __RODATA_END__ = .;
    } > RAM

    .data : ALIGN(16) {
        __DATA_START__ = .;
        *(EXCLUDE_FILE (loader.o) .data*)
        __DATA_END__ = .;
    } > RAM

    .bss : ALIGN(16) {
        __BSS_START__ = .;
        *(EXCLUDE_FILE (loader.o) .bss*)
        *(COMMON) /* TODO */
        __BSS_END__ = .;
    } > RAM

    .stack (NOLOAD) : ALIGN(16) {
        __STACK_START__ = .;
        *(.stack)
        __STACK_END__ = .;
    } > RAM

    . = LOADER_ADDR;

    .loader_text . : {
        __LOADER_TEXT_START__ = .;
        loader.o(.text*)
        __LOADER_TEXT_END__ = .;
    } > RAM

    .loader_rodata . : {
        __LOADER_RODATA_START__ = .;
        loader.o(.rodata*)
        __LOADER_RODATA_END__ = .;
    } > RAM

    .loader_data : ALIGN(16) {
        __LOADER_DATA_START__ = .;
        loader.o(.data*)
        __LOADER_DATA_END__ = .;
    } > RAM

    .loader_bss : ALIGN(16) {
        __LOADER_BSS_START__ = .;
        *(.bss*)
        loader.o(COMMON) /* TODO */
        __LOADER_BSS_END__ = .;
    } > RAM

    .loader_stack (NOLOAD) : ALIGN(16) {
        __LOADER_STACK_START__ = .;
        *(.loader_stack)
        __LOADER_STACK_END__ = .;
    } > RAM
}

