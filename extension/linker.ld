ENTRY(_start)

SECTIONS {
    /* Default Pi3 load address for kernel.img */
    . = 0x00008000;

    .text   : ALIGN(4) { *(.text)    *(.text.*) }
    .rodata : ALIGN(4) {
        *(.rodata)
        *(.rodata.*)
        __rodata_end__ = .;
    }
    .data   : ALIGN(4) {
        __data_start__ = .;
        *(.data)
        *(.data.*)
        __data_end__   = .;
    }

    . = ALIGN(4);
    __bss_start__ = .;
    .bss    : ALIGN(4) { *(.bss) *(.bss.*) *(COMMON) }
    __bss_end__   = .;

    _end = .;
}
