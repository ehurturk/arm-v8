#ifndef GPIO
#define GPIO

#include "../defs.h"
#include "peripherals.h"
typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1,
    GPIO_ALT0 = 4,
    GPIO_ALT1 = 5,
    GPIO_ALT2 = 6,
    GPIO_ALT3 = 7,
    GPIO_ALT4 = 3,
    GPIO_ALT5 = 2
} GPIO_IO_MODE;

/*
 * Based on the SPEC:
 * GPIO:
 *  0x3F200000 - 0x3F200000: GPFSEL[0-5] Registers - GPIO Function select
 *  0x3F20001C - 0x3F200020: GPSET[0-1] Registers - Pin Output Set
 *  0x3F200028 - 0x3F20002C: GPCLR[0-1] Registers - Pin Output Clear
 *  0x3F200034 - 0x3F200038: GPLEV[0-1] Registers - Pin Pin Level
 *  0x3F200040 - 0x3F200044: GPEDS[0-1] Registers - Pin Event Detect Status
 *  0x3F20004C - 0x3F200050: GPREN[0-1] Registers - Pin Rising Edge Detect Enable
 *  0x3F200058 - 0x3F20005C: GPFEN[0-1] Registers - Pin Falling Edge Detect Enable
 *  0x3F200064 - 0x3F200068: GPHEN[0-1] Registers - Pin High Detect Enable
 *  0x3F200070 - 0x3F200075: GPLEN[0-1] Registers - Pin Low Detect Enable
 *  0x3F200094             : GPPUD Register - Pin Pull-up/down Enable
 *  0x3F200098             : GPPUDCLK0 Register - Pin Pull-up/down Enable Clock 0
 *  0x3F20009C             : GPPUDCLK1 Register - Pin Pull-up/down Enable Clock 1
 */
typedef struct {
  reg32 func_select[6];         // 0x00 - 0x14
    reg32 _reserved0;             // 0x18
  reg32 output_set[2];          // 0x1C - 0x20
    reg32 _reserved1[1];          // 0x24
  reg32 output_clear[2];        // 0x28 - 0x2C
    reg32 _reserved2[1];          // 0x30
  reg32 level[2];               // 0x34 - 0x38
    reg32 _reserved3[1];          // 0x3C
  reg32 event_detect_status[2]; // 0x40 - 0x44
    reg32 _reserved4[1];          // 0x48
  reg32 re_detect_enable[2];    // 0x4C - 0x50
    reg32 _reserved5[1];          // 0x54
  reg32 fe_detect_enable[2];    // 0x58 - 0x5C
    reg32 _reserved6[1];          // 0x60
  reg32 hi_detect_enable[2];    // 0x64 - 0x68
    reg32 _reserved7[1];          // 0x6C
  reg32 low_detect_enable[2];   // 0x70 - 0x74
    reg32 _reserved8[1];          // 0x78
  reg32 async_re_detect[2];     // 0x7C - 0x80
    reg32 _reserved9[1];          // 0x84
  reg32 async_fe_detect[2];     // 0x88 - 0x8C
    reg32 _reserved10[4];         // 0x90 - 0x9C
  reg32 pupd_enable;            // 0x94
  reg32 pupd_enable_clocks[2];  // 0x98 - 0x9C
} gpio_regs_t;

#define GPIO_REGS ((gpio_regs_t *)(GPIO_BASE))

typedef enum {
    LOW = 0,
    HIGH = 1
} GPIO_VALUE;

extern void gpio_set_fsel(unsigned int pinno, GPIO_IO_MODE mode);
extern void gpio_set(unsigned int pinno);
extern void gpio_clr(unsigned int pinno);
extern void gpio_toggle(unsigned int pinno);
extern void gpio_write(unsigned int pinno, GPIO_VALUE value);
void gpio_enable_pullup(unsigned int pin);
extern u8 gpio_read(unsigned int pinno);

#endif /* GPIO */
