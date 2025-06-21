#ifndef UART
#define UART

#include "../defs.h"
#include "peripherals.h"

typedef struct {
  reg32 DR;             // 0x0
  reg32 RSRECR;         // 0x4
  reg32 _reserved0[4];  // 0x8-0xc-0x10-0x14
  reg32 FR;             // 0x18
  reg32 _reserved1;     // 0x1c
  reg32 ILPR;           // 0x20
  reg32 IBRD;           // 0x24
  reg32 FBRD;           // 0x28
  reg32 LCRH;           // 0x2c
  reg32 CR;             // 0x30
  reg32 IFLS;           // 0x34
  reg32 IMSC;           // 0x38
  reg32 RIS;            // 0x3c
  reg32 MIS;            // 0x40
  reg32 ICR;            // 0x44
  reg32 DMACR;          // 0x48
  reg32 _reserved2[13]; // 0x4c-0x7c
  reg32 ITCR;           // 0x80
  reg32 ITIP;           // 0x84
  reg32 ITOP;           // 0x88
  reg32 TDR;            // 0x8c
} uart_regs_t;

#define UART_REGS ((uart_regs_t *)(UART0_BASE))

extern void uart_init(void);
extern void uart_putc(char c);
extern char uart_getc();
extern void uart_puts(const char str[]);
extern void print_hex(unsigned int value);

#endif /* UART */

