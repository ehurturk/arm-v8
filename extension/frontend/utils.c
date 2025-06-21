#include "utils.h"
#include "lcd.h"
#include "display.h"

#define SYSTEM_TIMER_CLO (*(volatile uint32_t *)(0x3F003004))

void delay_us(volatile uint us) {
  uint32_t start = SYSTEM_TIMER_CLO;
  while ((SYSTEM_TIMER_CLO - start) < us)
    ;
}

uint get_system_time_ms() {
  return SYSTEM_TIMER_CLO / 1000;
}

void delay_cycles(volatile int count) {
  while (count--) {
    asm volatile("nop");
  }
}

void print_number(uint x) {
  // PRE: 4 digit number
  uint y0 = 10;
  while (x) {
    uint x0 = 10;
    int d = x%10; x/=10;
    for (int i = 0; i < d; i++) {
      lcd_draw_rectangle(x0, y0,
                         x0+15, y0+50,
                         0xFA00);
      x0 += 40;
    }
    y0 += 70;
  }
}