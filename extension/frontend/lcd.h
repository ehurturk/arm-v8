#ifndef LCD
#define LCD

#include "../defs.h"
#include "gpio.h"
#include "spi.h"
#include <stdbool.h>
#include <stdint.h>

#define ILI9486_WIDTH 320
#define ILI9486_HEIGHT 480

#define ILI9486_BLACK 0x0000
#define ILI9486_WHITE 0xFFFF
#define ILI9486_RED 0xF800
#define ILI9486_GREEN 0x07E0
#define ILI9486_BLUE 0x001F
#define ILI9486_YELLOW 0xFFE0
#define ILI9486_CYAN 0x07FF
#define ILI9486_MAGENTA 0xF81F

/* ILI9486 Commands */
#define ILI9486_INTRMDCTRL 0xB0 // Interface Mode Control
#define ILI9486_SLPOUT 0x11     // Sleep Out
#define ILI9486_DISPON 0x29     // Display On
#define ILI9486_CASET 0x2A      // Column Address Set
#define ILI9486_PASET 0x2B      // Page Address Set
#define ILI9486_RAMWR 0x2C      // Memory Write
#define ILI9486_MADCTL 0x36     // Memory Access Control
#define ILI9486_PIXFMT 0x3A     // Pixel Format Set (16-Bit)
#define ILI9486_FRMCTR1 0xB1    // Frame Control (Normal Mode)
#define ILI9486_DFUNCTR 0xB6    // Display Function Control
#define ILI9486_PWCTR1 0xC0     // Power Control 1
#define ILI9486_PWCTR2 0xC1     // Power Control 2
#define ILI9486_PWCTR3 0xC2     // Power Control 3
#define ILI9486_VMCTR1 0xC5     // VCOM Control 1
#define ILI9486_PGAMCTRL 0xE0   // Positive Gamma Control
#define ILI9486_NGAMCTRL 0xE1   // Negative Gamma Control
#define ILI9486_DGAMCTRL1 0xE2  // Digital Gamma Control 1

/* Rotation values for MADCTL */
#define ILI9486_MADCTL_MH 0x04  // Horizontal refresh order
#define ILI9486_MADCTL_ML 0x10  // Vertical refresh order
#define ILI9486_MADCTL_MV 0x20  // Row/Column exchange
#define ILI9486_MADCTL_MX 0x40  // Column address order
#define ILI9486_MADCTL_MY 0x80  // Row address order
#define ILI9486_MADCTL_BGR 0x08 // BGR color order

#define ILI9486_INVON 0x21
typedef struct {
  u16 width;
  u16 height;
  u8 rotation;
  u16 cursor_x;
  u16 cursor_y;
} ILI9486_t;

void lcd_init();
void lcd_draw_pixel_color(uint x, uint y, uint col);
void lcd_draw_rectangle_color(uint x0, uint x1, uint y0, uint y1, uint col);
void lcd_draw_image(uint x0, uint y0, uint x1, uint y1, uint nr_piece, uint fg_col, uint bg_col);
void lcd_draw_rectangle(uint x0, uint y0, uint x1, uint y1, uint color);
extern ILI9486_t display;

uint16_t read_touch(uint8_t cmd);
u16 read_x();
u16 read_y();

#endif /* LCD */
