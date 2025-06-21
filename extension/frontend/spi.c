#include "spi.h"
#include "gpio.h"
#include "utils.h"

inline void lcd_burst_start(void) {
  gpio_clr(SPI0_CE0_PIN);                                
  gpio_set(SPI0_DC_PIN);                                 
  SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | CS_TA_BIT;                             
}

static void spi_transfer_byte(u8 byte) {
  SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | CS_TA_BIT;
  while (!(SPI0_REGS->CS & CS_TXD_BIT));
  SPI0_REGS->FIFO = byte;
  while (!(SPI0_REGS->CS & CS_DONE_BIT));
  SPI0_REGS->CS &= ~CS_TA_BIT;
}

inline void lcd_burst_byte(u8 byte) {
  while (!(SPI0_REGS->CS & CS_TXD_BIT));
  SPI0_REGS->FIFO = byte;
}

inline void lcd_burst_end(void) {
  while (!(SPI0_REGS->CS & CS_DONE_BIT));
  SPI0_REGS->CS &= ~CS_TA_BIT;                           
  gpio_set(SPI0_CE0_PIN);                                
}

inline void lcd_cmd(u8 cmd) {
  gpio_clr(SPI0_CE0_PIN);
  gpio_clr(SPI0_DC_PIN);
  spi_transfer_byte(cmd);
  gpio_set(SPI0_CE0_PIN);
}

inline void lcd_data(u8 data) {
  gpio_clr(SPI0_CE0_PIN);
  gpio_set(SPI0_DC_PIN);
  spi_transfer_byte(data);
  gpio_set(SPI0_CE0_PIN);
}

void spi_init(void) {
  /* Configure GPIO for SPI0 */
  /*
   * MISO = GPIO9
   * MOSI = GPIO10
   * SCLK = GPIO11
   * CS = GPIO8
   * DC, RESET = custom GPIOs
   */

  gpio_set_fsel(SPI0_MISO_PIN, GPIO_ALT0);
  gpio_set_fsel(SPI0_MOSI_PIN, GPIO_ALT0);
  gpio_set_fsel(SPI0_SCLK_PIN, GPIO_ALT0);
  
  gpio_set_fsel(SPI0_DC_PIN, GPIO_OUTPUT);
  gpio_set_fsel(SPI0_RESET_PIN, GPIO_OUTPUT);

  gpio_set_fsel(SPI0_CE0_PIN, GPIO_OUTPUT); /* ILI9486 */
  gpio_set_fsel(SPI0_CE1_PIN, GPIO_OUTPUT); /* XPT2046 */

  gpio_set_fsel(LCD_BL_PIN, GPIO_OUTPUT);  // INPUT********

  /* Set TP_IRQ pin as touch panel interrupt */
  gpio_set_fsel(TIP_IRQ_PIN, GPIO_INPUT);   // INPUT********
  gpio_enable_pullup(TIP_IRQ_PIN);   // INPUT********

  delay_ms(10);

  gpio_set(SPI0_DC_PIN);
  gpio_set(SPI0_RESET_PIN);

  /* clear both cs pins */
  gpio_set(SPI0_CE0_PIN);
  gpio_set(SPI0_CE1_PIN);

  gpio_set(LCD_BL_PIN);

  SPI0_REGS->CS = 0;
  SPI0_REGS->CLK = 32; /* divide clock by 32 */
  /*
   * Enable CS0
   * CPHA = 0, CPOL = 0, Clear both TX & RX FIFOs
   */
  SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | 3;

}

// void pen_detect_tick(void) {
//     // assert CE1
//     gpio_set(SPI0_CE0_PIN);
//     gpio_clr(SPI0_CE1_PIN);
//       spi_transfer_byte(0xB1); //0x94
//       spi_transfer_byte(0);
//       spi_transfer_byte(0);
//     // de-assert CE1
//     gpio_set(SPI0_CE1_PIN);
// }


void spi_transfer_receive(u8* tx, u8 *rx, int len, int deviceid) {
  if (deviceid == 0) {
    gpio_set(SPI0_CE1_PIN);
    gpio_clr(SPI0_CE0_PIN);
    /* TODO: don't forget to set DC pin for ILI9486 here v */
  } else if (deviceid == 1) {
    gpio_set(SPI0_CE0_PIN);
    gpio_clr(SPI0_CE1_PIN);
  }
  /* Clear RX and TX FIFOs & set Transfer Active */
  SPI0_REGS->DLEN = len;
  SPI0_REGS->CS = CS_CLEAR_RX_BIT | CS_CLEAR_TX_BIT | CS_TA_BIT | (3u << 0);
  
  /* 1) transfer data */
  for (volatile int i = 0; i < len; i++) {
    while (!(SPI0_REGS->CS & CS_TXD_BIT));
    SPI0_REGS->FIFO = tx[i];
    while (!(SPI0_REGS->CS & CS_RXD_BIT));
    rx[i] = SPI0_REGS->FIFO;
  }
  
  /* Deactivate TA at the *very* end of the transfer */
  while (!(SPI0_REGS->CS & CS_DONE_BIT));
  SPI0_REGS->CS &= ~CS_TA_BIT;

  gpio_set(SPI0_CE0_PIN);
  gpio_set(SPI0_CE1_PIN);
}