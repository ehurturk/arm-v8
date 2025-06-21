#include "display.h"
#include "peripherals.h"
#include "pieces.h"
#include "lcd.h"
#include "../defs.h"
#include <stdio.h>
#include <stdbool.h>

#define SQUARE_COLOR(X, Y) (((X + Y) % 2 != 0) ? COLOR_LIGHT : COLOR_DARK)


void draw_square(uint x, uint y) {
  uint bg_color = SQUARE_COLOR(x, y);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, y*BOARD_SQ_SIZE,
                    (x+1)*BOARD_SQ_SIZE-1, (y+1)*BOARD_SQ_SIZE-1,
                    bg_color);
              
}

void draw_piece(uint x, uint y, bool is_white, uint nr_piece) {
  uint bg_color = SQUARE_COLOR(x, y);
  uint fg_color = is_white ? COLOR_WHITE : COLOR_BLACK;
  lcd_draw_image(x*BOARD_SQ_SIZE + PIECE_OFFSET, y*BOARD_SQ_SIZE + PIECE_OFFSET,
                  (x+1)*BOARD_SQ_SIZE-1-PIECE_OFFSET, (y+1)*BOARD_SQ_SIZE-1-PIECE_OFFSET,
                  nr_piece, fg_color, bg_color
                );
}

void draw_highlight(uint x, uint y) {
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, y*BOARD_SQ_SIZE,
                     (x+1)*BOARD_SQ_SIZE-1, y*BOARD_SQ_SIZE+1,
                     COLOR_HIGHLIGHT);
  lcd_draw_rectangle((x+1)*BOARD_SQ_SIZE-2, y*BOARD_SQ_SIZE+2,
                     (x+1)*BOARD_SQ_SIZE-1, (y+1)*BOARD_SQ_SIZE-1,
                     COLOR_HIGHLIGHT);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, (y+1)*BOARD_SQ_SIZE-2,
                     (x+1)*BOARD_SQ_SIZE-3, (y+1)*BOARD_SQ_SIZE-1,
                     COLOR_HIGHLIGHT);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, y*BOARD_SQ_SIZE+2,
                     x*BOARD_SQ_SIZE+1, (y+1)*BOARD_SQ_SIZE-3,
                     COLOR_HIGHLIGHT);
}

void clear_highlight(uint x, uint y) {
  uint bg_color = SQUARE_COLOR(x, y);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, y*BOARD_SQ_SIZE,
                     (x+1)*BOARD_SQ_SIZE-1, y*BOARD_SQ_SIZE+1,
                     bg_color);
  lcd_draw_rectangle((x+1)*BOARD_SQ_SIZE-2, y*BOARD_SQ_SIZE+2,
                     (x+1)*BOARD_SQ_SIZE-1, (y+1)*BOARD_SQ_SIZE-1,
                     bg_color);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, (y+1)*BOARD_SQ_SIZE-2,
                     (x+1)*BOARD_SQ_SIZE-3, (y+1)*BOARD_SQ_SIZE-1,
                     bg_color);
  lcd_draw_rectangle(x*BOARD_SQ_SIZE, y*BOARD_SQ_SIZE+2,
                     x*BOARD_SQ_SIZE+1, (y+1)*BOARD_SQ_SIZE-3,
                     bg_color);
}

void draw_highlight_pos(u16 x, u16 y) {
  draw_highlight(x/BOARD_SQ_SIZE, y/BOARD_SQ_SIZE);
}

void clear_highlight_pos(u16 x, u16 y) {
  clear_highlight(x/BOARD_SQ_SIZE, y/BOARD_SQ_SIZE);
}

/*
  Dimensions: 320 x 320 bits
  Piece Info: 32x32 bits
*/

void draw_board() {
  for (int x = 0; x < 8; x++) {
    for (int y = 0; y < 8; y++) {
      draw_square(x, y);
    }
  }
}

void draw_pieces() {
    for (int i = 0; i < 8; i++) 
    draw_piece(i, 1, true, PIECE_PAWN); // white pawns
  
  draw_piece(0, 0, true, PIECE_ROOK); // white rooks
  draw_piece(7, 0, true, PIECE_ROOK);

  draw_piece(1, 0, true, PIECE_KNIGHT); // white knights
  draw_piece(6, 0, true, PIECE_KNIGHT);

  draw_piece(2, 0, true, PIECE_BISHOP); // white bishops
  draw_piece(5, 0, true, PIECE_BISHOP);

  draw_piece(3, 0, true, PIECE_QUEEN); // white queen
  draw_piece(4, 0, true, PIECE_KING);  // white king

  for (int i = 0; i < 8; i++) 
    draw_piece(i, 6, false, PIECE_PAWN); // black pawns
  
  draw_piece(0, 7, false, PIECE_ROOK); // black rooks
  draw_piece(7, 7, false, PIECE_ROOK);

  draw_piece(1, 7, false, PIECE_KNIGHT); // black knights
  draw_piece(6, 7, false, PIECE_KNIGHT);

  draw_piece(2, 7, false, PIECE_BISHOP); // black bishops
  draw_piece(5, 7, false, PIECE_BISHOP);

  draw_piece(3, 7, false, PIECE_QUEEN); // black queen
  draw_piece(4, 7, false, PIECE_KING);  // black king
}

void init_board() {
  draw_board();
  draw_pieces();
  lcd_draw_rectangle(BOARD_WIDTH, 0, SCREEN_WIDTH, SCREEN_HEIGHT, COLOR_DEF);
}

void move(u16 from_x, u16 from_y, u16 to_x, u16 to_y) {
  piece p = board[from_x][from_y];
  if (p.piece_type == PIECE_EMPTY) {
    return;
  }

  board[from_x][from_y] = (piece){ 0, PIECE_EMPTY};
  draw_square(from_x, from_y);
  board[to_x][to_y] = p;
  draw_piece(to_x, to_y, p.col == COLOR_WHITE, p.piece_type);
  // update board here
}