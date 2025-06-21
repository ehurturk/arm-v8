#ifndef SPI
#define SPI

#include "../defs.h"
#include "peripherals.h"
#include <stddef.h>

/* ================== SPIO0 Register Bits ================== */
/* SPI control state register bits: */
/* ---- CLEAR bits: ---- */
/* Bits 5:4:
 *   0b00 - No action
 *   0bx1 = Clear TX FIFO
 *   0b1x = Clear RX FIFO
 *   0b11 = Clear TX & RX FIFO
 */
#define CS_CLEAR_RX_BIT (1 << 4)
#define CS_CLEAR_TX_BIT (1 << 5)
/* Chip Select Polarity
 * 0 = CS lines are active low
 * 1 = CS lines are active high
 */
#define CS_CSPOL_BIT (1 << 6)
/* --------------------- */
/* Transfer Active Bit
 * 0 = Transfer not active (CS lines are all high, assuming CSPOL = 0)
 * 1 = Transfer active (CS lines are set according to CS bits and CSPOL)
 */
#define CS_TA_BIT (1 << 7) /* Set to 1 before writing to FIFO */
/*
 * REN Read Enable
 * 0 = Write to SPI Peripheral
 * 1 = Read from SPI Peripheral
 */
#define CS_REN_BIT (1 << 12)
/*
 * 0 = Transfer in progress (or not active TA = 0)
 * 1 = Transfer is complete (cleared by setting TA = 0)
 */
#define CS_DONE_BIT (1 << 16) /* wait for 1 after write */
/* 0 = RX FIFO is empty */
/* 1 = RX FIFO has at least 1 byte */
#define CS_RXD_BIT (1 << 17) /* wait for 1 before read */
/* 0 = TX FIFO is full */
/* 1 = TX FIFO has space for at least 1 byte */
#define CS_TXD_BIT (1 << 18) /* wait for 1 before write */
/*
 * 0 = RX FIFO is not full
 * 1 = RX FIFO is full (no further serial data will be sent/received
 *     until data is read from FIFO)
 */
#define CS_RXF_BIT (1 << 20) /* RXF; full if 1 */

#define SPI0_MISO_PIN 9
#define SPI0_MOSI_PIN 10
#define SPI0_SCLK_PIN 11
#define SPI0_CE0_PIN 8 /* For the ILI9486 */
#define LCD_BL_PIN    18

/* For ILI9486 Display */
#define SPI0_DC_PIN 24
#define SPI0_RESET_PIN 25

/* For the XPT2046 Touchscreen */
#define SPI0_CE1_PIN 7
#define TIP_IRQ_PIN 17

typedef enum { MOSI, MISO, SCLK, CE0 } SPI_SIGNAL;

typedef struct {
  reg32 CS;   // 0x00: Control and Status
  reg32 FIFO; // 0x04: Data FIFO
  reg32 CLK;  // 0x08: Clock Divider
  reg32 DLEN; // 0x0c: Data length
  reg32 LTOH; // 0x10: LOSSI mode TOH
  reg32 DC;   // 0x14: DMA DREQ
} spi0_regs_t;

#define SPI0_REGS ((spi0_regs_t *)(SPI0_BASE))

// we need to set SPI state to gpio as well

/*
 * SPI Notes from https://learn.sparkfun.com/tutorials/serial-peripheral-interface-spi/all
 *
 * PICO := MOSI
 * MISO := POCI
 * Controller (Master) -> Peripheral (Slave) [PICO/MOSI]
 * Peripheral (Slave) -> Controller (Master) [POCI/MISO]
 * So, data is sent as:
 *    [Controller]              [Peripheral]
 *      SCK         ----->         SCK
 *      PICO        ----->         PICO
 *      POCI        <-----         POCI
 *
 * CS - Chip Select
 * Used for telling the peripheral (slave) that it should wake up and receive/send data,
 * also used when multiple peripherals are present to seelct the one we need.
 *  - CS Line is normally held high, which disconnects the peripheral from SPI bus (active low).
 *    Just before the data is sent to the peripheral, line is brought to low, which activates it.
 *
 */

// void spi_begin();
// void spi_end();
void spi_send_command(u8 cmd);
void lcd_cmd(u8 cmd);
void lcd_data(u8 data);
// void spi_transfer_byte(u8 byte);
void spi_init(void);
void lcd_burst_end(void);
void lcd_burst_byte(u8 b);
void lcd_burst_start(void);
// void pen_detect_tick(void);

/* deviceid 0 = ili9486, deviceid 1 = xpt2046 */
// void spi_transfer_receive(u8* tx, u8* rx, int len, int deviceid);
#endif /* SPI */