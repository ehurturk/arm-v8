/*
 * touchscreen.c
 * XPT2046 touch routines using Z1 threshold instead of PENIRQ
 */

#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "touchscreen.h"
#include "utils.h"
#include "display.h"
#include "gpio.h"        // gpio_set_fsel, gpio_enable_pullup, gpio_set, gpio_clr, gpio_read
#include "spi.h"         // spi_transfer_receive, SPI0_CE1_PIN

// XPT2046 commands
#define XPT_CMD_Z1         0xB1  // Z1 pressure, PD=01
#define CMD_COL 0x90
#define CMD_ROW 0xD0

// Threshold for detecting touch from Z1 reading
#define Z1_THRESHOLD       30

#define RAW_X_MAX 1350
#define RAW_X_MIN 209
#define RAW_Y_MAX 1856 
#define RAW_Y_MIN 255

// screenX = A*rawX + B*rawY + C
static const float A = -0.01316804f;
static const float B = -0.30336286f;
static const float C = 597.063676f;

// screenY = D*rawX + E*rawY + F
static const float D = -0.20938256f;
static const float E =  0.00956594f;
static const float F = 425.468920f;

#define FP_SCALE  (1<<16)
#define A_i (int32_t)(A * FP_SCALE)
#define B_i (int32_t)(B * FP_SCALE)
#define C_i (int32_t)(C * FP_SCALE)
#define D_i (int32_t)(D * FP_SCALE)
#define E_i (int32_t)(E * FP_SCALE)
#define F_i (int32_t)(F * FP_SCALE)

void spi_send_receive(int cmd, u8* rx1, u8* rx2) {
  gpio_set(SPI0_CE0_PIN);
  gpio_clr(SPI0_CE1_PIN);

  SPI0_REGS->DLEN = 3;
  SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | CS_TA_BIT | (3u << 0); 

  while (!(SPI0_REGS->CS & CS_TXD_BIT));
  SPI0_REGS->FIFO = cmd;
  while (!(SPI0_REGS->CS & CS_RXD_BIT));
  (void)SPI0_REGS->FIFO;

  while (!(SPI0_REGS->CS & CS_TXD_BIT));
  SPI0_REGS->FIFO = 0x00;
  while (!(SPI0_REGS->CS & CS_RXD_BIT));
  *rx1 = SPI0_REGS->FIFO;

  while (!(SPI0_REGS->CS & CS_TXD_BIT));
  SPI0_REGS->FIFO = 0x00;
  while (!(SPI0_REGS->CS & CS_RXD_BIT));
  *rx2 = SPI0_REGS->FIFO;

  while (!(SPI0_REGS->CS & CS_DONE_BIT));
  SPI0_REGS->CS &= ~CS_TA_BIT;
  gpio_set(SPI0_CE1_PIN);
}

u16 touchscreen_read(u8 cmd) {
    u8 rx1 = 0, rx2 = 0;
    spi_send_receive(cmd, &rx1, &rx2);
    
    // spi_transfer_receive(tx, rx, 3, 1);
    // Combine bytes and shift: upper 12 bits >>3
    return (((u16)rx1 << 8) | rx2) >> 4;
}

static inline void map_affine_i(uint16_t rawX, uint16_t rawY,
                                uint16_t *pX, uint16_t *pY) {
    // fixed‐point multiply/add in 32×32→64:
    int64_t fx = (int64_t)A_i*rawX + (int64_t)B_i*rawY + (int64_t)C_i;
    int64_t fy = (int64_t)D_i*rawX + (int64_t)E_i*rawY + (int64_t)F_i;

    // back to integer: fx>>16
    int32_t ix = fx >> 16, iy = fy >> 16;
    if (ix < 0) ix = 0;    else if (ix > 479) ix = 479;
    if (iy < 0) iy = 0;    else if (iy > 319) iy = 319;

    *pX = ix;
    *pY = iy;
}

static int cmp_u16(const void *a, const void *b) {
    uint16_t va = *(uint16_t*)a;
    uint16_t vb = *(uint16_t*)b;
    return (va < vb) ? -1 : (va > vb) ? 1 : 0;
}

bool touchscreen_get_point(u16 *x, u16 *y) {
  static uint32_t last_tap_ms = 0;
  uint32_t now = get_system_time_ms();
  if (now - last_tap_ms < 50) return false;
  delay_ms(2);
  // check for time of last touch maybes
  // for (int i = 0; i < 5; i++) {
  //   if (!touchscreen_pen_down()) return false;
  //   delay_ms(10);
  // }

  uint32_t t0 = now;
  while (get_system_time_ms() - t0 < 70) {
    if (!touchscreen_pen_down()) 
      return false;          // finger lifted too soon
    delay_ms(5);
  }

  u16 x_raw[20], y_raw[20];
  // u32 x_sum = 0, y_sum = 0;
  int valid_readings = 0;  
  for (int i = 0; i < 20; i++) {
      if (!touchscreen_pen_down()) break; //return false;  // Touch released
      
      x_raw[valid_readings] = touchscreen_read(CMD_ROW);
      y_raw[valid_readings] = touchscreen_read(CMD_COL);
      valid_readings++;
      // if (x_raw >= 200 && x_raw <= 2500 && y_raw >= 200 && y_raw <= 2500) {
          // x_sum += x_r aw;
          // y_sum += y_raw;
    //     valid_readings++;
      // }
      delay_ms(20);
  }
  if(valid_readings < 14) return false;

  qsort(x_raw, valid_readings, sizeof(u16), cmp_u16);
  qsort(y_raw, valid_readings, sizeof(u16), cmp_u16);  // delay_ms(1);
  u16 x_avg = (x_raw[valid_readings/2-1] + x_raw[valid_readings/2] + x_raw[valid_readings/2+1]) / 3;
  u16 y_avg = (y_raw[valid_readings/2-1] + y_raw[valid_readings/2] + y_raw[valid_readings/2+1]) / 3;

  // print_number(x_avg);
  // delay_ms(4000);
  // draw_board();
  // print_number(y_avg);
  // delay_ms(4000);
  // draw_board();

  map_affine_i(x_avg, y_avg, x, y);  
  last_tap_ms = now;
  return true;
}

bool touchscreen_pen_down(void) {
  u16 z = touchscreen_read(XPT_CMD_Z1); // pressure
  return (z >= Z1_THRESHOLD);
}
