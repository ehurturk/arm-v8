#include "gpio.h"
#include "utils.h"

void gpio_set_fsel(unsigned int pinno, GPIO_IO_MODE mode) {
  u8 bit_start = (pinno * 3) % 30;
  u8 reg = pinno / 10;

  u32 selector = GPIO_REGS->func_select[reg];
  selector &= ~(7 << bit_start);
  selector |= (mode << bit_start);

  GPIO_REGS->func_select[reg] = selector;
}

void gpio_set(unsigned int pinno) {
  if (pinno < 32) {
    GPIO_REGS->output_set[0] = (1 << pinno);
  } else {
    GPIO_REGS->output_set[1] = (1 << (pinno - 32));
  }
}

void gpio_clr(unsigned int pinno) {
  if (pinno < 32) {
    GPIO_REGS->output_clear[0] = (1 << pinno);
  } else {
    GPIO_REGS->output_clear[1] = (1 << (pinno - 32));
  }
}

void gpio_write(unsigned int pinno, GPIO_VALUE value) {
  if (value == HIGH) {
    gpio_set(pinno);
  } else { // LOW
    gpio_clr(pinno);
  }
}

void gpio_toggle(uint pinno) {
  uint idx = pinno / 32;
  uint gpioaddr = idx == 0 ? 0 : 1;
  u32 level = GPIO_REGS->level[gpioaddr] & (1 << (pinno % 32));

  if (level) {
    gpio_clr(pinno);
  } else {
    gpio_set(pinno);
  }
}

u8 gpio_read(unsigned int pinno) {
   uint gpioaddr = (pinno / 32) == 0 ? 0 : 1;
   return (GPIO_REGS->level[gpioaddr] & (1 << (pinno % 32))) != 0;
}

void gpio_enable_pullup(unsigned int pinNumber) {
    GPIO_REGS->pupd_enable = 2; //0
    delay_cycles(150);
    GPIO_REGS->pupd_enable_clocks[pinNumber / 32] = 1 << (pinNumber % 32);
    delay_cycles(150);
    GPIO_REGS->pupd_enable = 0;
    GPIO_REGS->pupd_enable_clocks[pinNumber / 32] = 0;
}