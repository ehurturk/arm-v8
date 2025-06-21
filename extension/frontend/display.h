#ifndef DISPLAY
#define DISPLAY

#include "../defs.h"
#include "lcd.h"

#define BOARD_WIDTH 320
#define BOARD_HEIGHT 320
#define SCREEN_WIDTH 480
#define SCREEN_HEIGHT 320


#define BOARD_SQ_SIZE 40

#define PIECE_SIZE 32

#define COLOR_LIGHT 0xDF5E //0xF6DA  // light square (beige)
#define COLOR_DARK  0x32EF //0xB344  // dark square (brown)
#define COLOR_DEF 0x0000   // default color

#define COLOR_WHITE 0xF7DF
#define COLOR_BLACK 0x2167
#define COLOR_HIGHLIGHT 0xF7E5

#define HIGHLIHGT_BORDER_WIDTH 2
#define PIECE_WIDTH 32
#define PIECE_OFFSET 4
/*
  Dimensions: 320 x 320 bits
  Piece Info: 30x30 bits
*/
void draw_board();
void clear_highlight(uint x, uint y);
void draw_highlight(uint x, uint y);
void clear_highlight_pos(u16 x, u16 y);
void draw_highlight_pos(u16 x, u16 y);
void init_board();
void move(u16 from_x, u16 from_y, u16 to_x, u16 to_y);
#endif /* DISPLAY */
