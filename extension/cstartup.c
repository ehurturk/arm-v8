/* Linker symbols defined in your linker script */
extern unsigned int __data_start__;
extern unsigned int __data_end__;
extern unsigned int __rodata_end__;   // load address for .data in flash
extern unsigned int __bss_start__;
extern unsigned int __bss_end__;

extern void main(void);

void _cstartup(unsigned int r0, unsigned int r1, unsigned int atags) {
    unsigned int *src, *dst;

    // 0) Enable the FPU (CP10/11) via CP15, not SCB:
    asm volatile(
      /* enable CP10/11 in CPACR */
      "MRC p15, #0, r0, c1, c0, #2\n"
      "ORR r0, r0, #(0xF << 20)\n"
      "MCR p15, #0, r0, c1, c0, #2\n"
      /* now enable the FPU itself */
      "LDR   r0, =0xE000EF34   \n"
      "LDR   r1, [r0]          \n"
      "ORR   r1, r1, #(1<<30)  \n"
      "STR   r1, [r0]          \n"
      "ISB                     \n"
      "DSB                     \n"
      ::: "r0","r1"
    );


    // 1) Copy .data from flash into RAM
    extern unsigned int __rodata_end__, __data_start__, __data_end__;
    src = &__rodata_end__;
    dst = &__data_start__;
    while (dst < &__data_end__) {
        *dst++ = *src++;
    }

    // 2) Zero the .bss
    extern unsigned int __bss_start__, __bss_end__;
    dst = &__bss_start__;
    while (dst < &__bss_end__) {
        *dst++ = 0;
    }

    // 3) Jump to your C main
    main();

    // 4) Hang if main ever returns
    for (;;);
}

