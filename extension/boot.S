.global _start
.global _get_stack_pointer

.section .text
_start:
    ldr sp, =0x100000      // Set stack pointer safely above kernel
    bl main                // Call C code
    b .                    // Halt forever

_get_stack_pointer:
    mov r0, sp
    bx lr