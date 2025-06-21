#include "lcd.h"
#include "gpio.h"
#include "spi.h"
#include "utils.h"
#include "display.h"
#include "pieces.h"

static void lcd_write_reg(u8 *buf, size_t len) {
  if (len == 0)
    return; //return 0x00;
  lcd_cmd(buf[0]);
  // lcd_burst_start();
  for (size_t i = 1; i < len; i++) {
    lcd_data(buf[i]);
    // lcd_burst_byte(buf[i]);
  }
  // lcd_burst_end();
}

void lcd_draw_pixel_color(uint x, uint y, uint col) {
  // Set column address
  u8 caset[] = {ILI9486_CASET, (y >> 8) & 0xFF, y & 0xFF, 
                (y >> 8) & 0xFF, y & 0xFF};
  lcd_write_reg(caset, 5);

  // Set page address
  u8 paset[] = {ILI9486_PASET, (x >> 8) & 0xFF, x & 0xFF,
                (x >> 8) & 0xFF, x & 0xFF};
  lcd_write_reg(paset, 5);

  //                                   high byte        low byte
  u8 pixel_data[] = {ILI9486_RAMWR, (col >> 8) & 0xFF, col & 0xFF};
  lcd_write_reg(pixel_data, 3);
}

// extra pixels are ignored
void lcd_draw_rectangle_color(uint x0, uint y0, uint x1, uint y1, uint col) {
  if (x0 > x1) { uint tmp = x0; x0 = x1; x1 = tmp; } // assure x0 <= x1
  if (y0 > y1) { uint tmp = y0; y0 = y1; y1 = tmp; } // assure y0 <= y1
  // set column address
  u8 caset[] = {ILI9486_CASET, 
                (y0 >> 8) & 0xFF, y0 & 0xFF,
                (y1 >> 8) & 0xFF, y1 & 0xFF};
  lcd_write_reg(caset, 5);

  // set page address
  u8 paset[] = {ILI9486_PASET, 
                (x0 >> 8) & 0xFF, x0 & 0xFF, 
                (x1 >> 8) & 0xFF, x1 & 0xFF};
  lcd_write_reg(paset, 5);

  // set pixel values
  uint count = (x1 - x0 + 1) * (y1 - y0 + 1);
  lcd_cmd(ILI9486_RAMWR);
  // lcd_burst_start();
  for (uint i = 0; i < count; i++) {
    // lcd_burst_byte((col >> 8) & 0xFF);  // high byte
    // lcd_burst_byte(col & 0xFF);         // low byte
    lcd_data((col >> 8) & 0xFF);  // high byte
    lcd_data(col & 0xFF);         // low byte
  }
  // do twice:
  // for (uint i = 0; i < count; i++) {
  //   // lcd_burst_byte((col >> 8) & 0xFF);  // high byte
  //   // lcd_burst_byte(col & 0xFF);         // low byte
  //   lcd_data((col >> 8) & 0xFF);  // high byte
  //   lcd_data(col & 0xFF);         // low byte
  // }
  // lcd_burst_end();
}

void lcd_draw_image(uint x0, uint y0, uint x1, uint y1, uint nr_piece, uint fg_col, uint bg_col) {
  uint p_idx = 0;
  for (uint i = x1; i >= x0; i--) {
    for (uint j = y1; j >= y0; j--) {
      if (pieces[nr_piece][p_idx]) {
        lcd_draw_pixel_color(i, j, fg_col);
      } else {
         lcd_draw_pixel_color(i, j, bg_col);
      }
      p_idx++;
    }
  }  
}

void lcd_draw_rectangle(uint x0, uint y0, uint x1, uint y1, uint color) {
  for (uint i = x0; i <= x1; i++) {
    for (uint j = y0; j <= y1; j++) {
      lcd_draw_pixel_color(i, j, color);
    }
  }
}

void lcd_init(void) {
    // ---- Hardware reset ----
    gpio_set(SPI0_RESET_PIN);
    delay_ms(10);
    gpio_clr(SPI0_RESET_PIN);
    delay_ms(100);
    gpio_set(SPI0_RESET_PIN);
    delay_ms(120);

    // ---- Software reset ----
    lcd_cmd(0x01);       // ILI9468 Software Reset
    delay_ms(150);

    // ---- Exit sleep ----
    lcd_cmd(0x11);       // Sleep Out
    delay_ms(150);

    // ---- Pixel format: 16-bit/pixel (RGB565) ----
    u8 pf[] = { 0x3A, 0x55 };
    lcd_write_reg(pf, sizeof(pf));
    delay_ms(10);

    // ---- Memory Access Control: RGB order, mirroring column ----
    u8 mac[] = { 0x36, 0x48 };
    lcd_write_reg(mac, sizeof(mac));
    delay_ms(10);

    // ---- Interface control (porch, gate) ----
    u8 itf[] = { 0xB0, 0x00 };
    lcd_write_reg(itf, sizeof(itf));
    delay_ms(10);

    // ---- Power-control & VCOM ----
    u8 pwr1[] = { 0xC0, 0x0D, 0x0D };
    u8 pwr2[] = { 0xC1, 0x43, 0x00 };
    u8 pwr3[] = { 0xC2, 0x00      };
    u8 vcom[] = { 0xC5, 0x00, 0x48 };
    lcd_write_reg(pwr1, sizeof(pwr1));
    lcd_write_reg(pwr2, sizeof(pwr2));
    lcd_write_reg(pwr3, sizeof(pwr3));
    lcd_write_reg(vcom, sizeof(vcom));

    // ---- Frame-rate & porch control ----
    u8 itf_ctrl[] = { 0xB6, 0x00, 0x01, 0x3B };
    lcd_write_reg(itf_ctrl, sizeof(itf_ctrl));

    // ---- Gamma correction ----
    u8 gamma_pos[] = {
        0xE0,
        0x0F, 0x24, 0x1C, 0x0A, 0x0F, 0x08, 0x43,
        0x88, 0x32, 0x0F, 0x10, 0x06, 0x0F, 0x07, 0x00
      };
    u8 gamma_neg[] = {
        0xE1,
        0x0F, 0x38, 0x30, 0x09, 0x0F, 0x0F, 0x4E,
        0x77, 0x3C, 0x07, 0x10, 0x05, 0x23, 0x1B, 0x00
      };
    lcd_write_reg(gamma_pos, sizeof(gamma_pos));
    lcd_write_reg(gamma_neg, sizeof(gamma_neg));
    delay_ms(10);

    // ---- Display ON ----
    lcd_cmd(0x29);       // Display ON
    delay_ms(1000);
}


// u16 read_x() {
//   u8 tx[3] = {0x90, 0x00, 0x00};  // Command + 2 dummy bytes to clock out data
//   u8 rx[3] = {0};

//   spi_transfer_receive(tx, rx, 3, 1);  // deviceid=1 for touchscreen

//   // XPT2046 sends 12-bit data in bits [15:4], so:
//   u16 val = ((rx[1] << 8) | rx[2]) >> 4;

//   return val;
// }

// u16 read_y() {
//   u8 tx[3] = {0xD0, 0x00, 0x00};  // Command + 2 dummy bytes to clock out data
//   u8 rx[3] = {0};

//   spi_transfer_receive(tx, rx, 3, 1);  // deviceid=1 for touchscreen

//   // XPT2046 sends 12-bit data in bits [15:4], so:
//   u16 val = ((rx[1] << 8) | rx[2]) >> 4;

//   return val;
// }

uint16_t read_touch(uint8_t cmd) {
    uint8_t tx[3] = { cmd, 0x00, 0x00 };
    uint8_t rx[3];
    // CE1 low
    gpio_set(SPI0_CE0_PIN);
    gpio_clr(SPI0_CE1_PIN);

    // clear FIFOs & start transfer of 3 bytes
    SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | CS_TA_BIT;
    SPI0_REGS->DLEN = 3;
    for (int i = 0; i < 3; i++) {
      while (!(SPI0_REGS->CS & CS_TXD_BIT));
      SPI0_REGS->FIFO = tx[i];
      while (!(SPI0_REGS->CS & CS_RXD_BIT));
      rx[i] = SPI0_REGS->FIFO;
    }
    while (!(SPI0_REGS->CS & CS_DONE_BIT));
    SPI0_REGS->CS &= ~CS_TA_BIT;

    // CE1 high
    gpio_set(SPI0_CE1_PIN);

    // Data is 12 bits: rx[1]<<8 | rx[2], then >>3
    return (((uint16_t)rx[1] << 8) | rx[2]) >> 3;
}
