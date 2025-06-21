# Current FIXME's:
1) We are operating on 32-bit mode for RPI3B. 
2) `boot.S`: 
```S
_start:
    ldr sp, =0x8000 
    bl _cstartup
    b .
```
    Stack pointer is set to 0x8000 (not 0x80000). Maybe change this?
3) `linker.ld`:
   - Look at alignment, and check whether rodata is placed correctly after text.
   - *IMPORTANT*: Program is loaded at address:  ```. = 0x10000;```. MAYBE Change this to 0x80000 for actual execution on RPI3B. For QEMU, use 0x10000.
