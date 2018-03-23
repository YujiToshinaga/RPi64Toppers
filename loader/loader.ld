#define MACRO_ONLY
#include "loader.h"

OUTPUT_FORMAT(elf64-littleaarch64)
OUTPUT_ARCH(aarch64)
ENTRY(_entry)

MEMORY {
    RAM (rwx): ORIGIN = SDRAM_ADDR, LENGTH = SDRAM_SIZE
}

SECTIONS
{
    . = START_ADDR;

    .startup . : {
        __STARTUP_START__ = .;
        startup.o(.startup)
        __STARTUP_END__ = .;
    } > RAM

    . = LOADER_ADDR;

    .loader_startup . : {
        __LOADER_STARTUP_START__ = .;
        startup.o(.loader_startup)
        __LOADER_STARTUP_END__ = .;
    } > RAM

    .loader_text . : {
        __LOADER_TEXT_START__ = .;
        *(EXCLUDE_FILE (loader_cached.o) .text*)
        __LOADER_TEXT_END__ = .;
    } > RAM

    .loader_rodata . : {
        __LOADER_RODATA_START__ = .;
        *(EXCLUDE_FILE (loader_cached.o) .rodata*)
        __LOADER_RODATA_END__ = .;
    } > RAM

    .loader_data : ALIGN(16) {
        __LOADER_DATA_START__ = .;
        *(EXCLUDE_FILE (loader_cached.o) .data*)
        __LOADER_DATA_END__ = .;
    } > RAM

    .loader_bss : ALIGN(16) {
        __LOADER_BSS_START__ = .;
        *(EXCLUDE_FILE (loader_cached.o) .bss*)
        *(COMMON) /* TODO */
        __LOADER_BSS_END__ = .;
    } > RAM

    .loader_stack (NOLOAD) : ALIGN(16) {
        __LOADER_STACK_START__ = .;
        *(.loader_stack)
        __LOADER_STACK_END__ = .;
    } > RAM

    . = LOADER_CACHED_ADDR;

    .loader_cached_text . : {
        __LOADER_CACHED_TEXT_START__ = .;
        loader_cached.o(.text*)
        __LOADER_CACHED_TEXT_END__ = .;
    } > RAM

    .loader_cached_rodata . : {
        __LOADER_CACHED_RODATA_START__ = .;
        loader_cached.o(.rodata*)
        __LOADER_CACHED_RODATA_END__ = .;
    } > RAM

    .loader_cached_data : ALIGN(16) {
        __LOADER_CACHED_DATA_START__ = .;
        loader_cached.o(.data*)
        __LOADER_CACHED_DATA_END__ = .;
    } > RAM

    .loader_cached_bss : ALIGN(16) {
        __LOADER_CACHED_BSS_START__ = .;
        *(.bss*)
        loader_cached.o(COMMON) /* TODO */
        __LOADER_CACHED_BSS_END__ = .;
    } > RAM

    .loader_cached_stack (NOLOAD) : ALIGN(16) {
        __LOADER_CACHED_STACK_START__ = .;
        *(.loader_cached_stack)
        __LOADER_CACHED_STACK_END__ = .;
    } > RAM
}

