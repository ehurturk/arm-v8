ENTRY(_start)

SECTIONS {
    . = 0x10000;

    .text : ALIGN(4) { 
        *(.text)
        *(.text.*)
        
        . = ALIGN(4);
        __rodata_start = .;
        *(.rodata*)
        *(.rodata)
        __rodata_end = .;
        
        /* And data */
        . = ALIGN(4);
        __data_start = .;
        *(.data*)
        *(.data)
        __data_end = .;
    }
    
    .bss : ALIGN(8)
    {
        __bss_start__ = .;
        *(.bss*)
        *(.bss)
        *(COMMON)
        __bss_end__ = .;
    }
    _end = .; 
}