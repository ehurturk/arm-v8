#include "uart.h"
#include "gpio.h"
#include "peripherals.h"
#include "utils.h"

#define UART_GPIO_PIN0 14
#define UART_GPIO_PIN1 15

void uart_init(void) {
  /* Turn off UART0 */
  UART_REGS->CR = 0x00000000;

  /* Setup the gpio pin 14 and 15 to ALT0 mode for UART */
  gpio_set_fsel(UART_GPIO_PIN0, GPIO_ALT0);
  gpio_set_fsel(UART_GPIO_PIN1, GPIO_ALT0);

  /* Disable pull up and pull down for gpio 14 and 15 pins */
  GPIO_REGS->pupd_enable = 0x00000000;
  delay_us(150);
  GPIO_REGS->pupd_enable_clocks[0] = (1 << 14) | (1 << 15);
  delay_us(150);
  GPIO_REGS->pupd_enable_clocks[0] = 0x00000000; /* for flushing GPIO setup */

  /* Clear pending interrupts */
  UART_REGS->ICR = 0x7FF;

  /* Set baud rate (115200) */
  UART_REGS->IBRD = 1;
  UART_REGS->FBRD = 40;

  /* Enable FIFO & 8 bit data transmission */
  UART_REGS->LCRH = (1 << 4) | (1 << 5) | (1 << 6);

  /* Enable UART0, receive & transfer */
  UART_REGS->CR = (1 << 0) | (1 << 8) | (1 << 9);
}

void uart_putc(char c) {
  /* Wait for UART to become ready to transmit */
  while (UART0_FR & (1 << 5)) {
    asm volatile("nop");
  }
  /* Send character to data register */
  UART_REGS->DR = c;
}

char uart_getc() {
  char r;

  while (UART_REGS->FR & (1 << 4)) {
    asm volatile("nop");
  }

  /* read it from the data register */
  r = (char)(UART_REGS->DR);
  return r == '\r' ? '\n' : r;
}

void uart_puts(const char str[]) {
  if (!str)
    return;

  while (*str) {
    if (*str == '\n') {
      uart_putc('\r');
      uart_putc('\n');
    } else {
      uart_putc(*str);
    }
    str++;
  }
}

void print_hex(unsigned int value) {
  uart_putc('0');
  uart_putc('x');
  for (int i = 28; i >= 0; i -= 4) {
    unsigned int digit = (value >> i) & 0xF;
    uart_putc((digit < 10) ? ('0' + digit) : ('A' + digit - 10));
  }
}