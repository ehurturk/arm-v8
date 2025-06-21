.global _start
.global _get_stack_pointer

.section .text

_start:
    ldr sp, =0x8000 
    bl _cstartup
    b .

_get_stack_pointer:
    mov r0, sp
    bx lr