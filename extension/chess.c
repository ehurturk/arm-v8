#include "backend/src/alpha_beta_search.h"
#include "backend/src/bitboard.h"
#include "backend/src/movegen.h"
#include "backend/src/position.h"
#include "backend/src/zobrist_hash.h"
#include "frontend/display.h"
#include "frontend/gpio.h"
#include "frontend/lcd.h"
#include "frontend/pieces.h"
#include "frontend/spi.h"
#include "frontend/touchscreen.h"
#include "frontend/utils.h"
// #include "backend/src/bytefish.h"
#include "defs.h"
#include <stdbool.h>

int main(void) {
  spi_init();
  delay_ms(100);
  lcd_init();
  delay_ms(100);
  init_board();
  init_zobrist();
  // backend
  char fen[] = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";
  Position *pos = make_empty_Position();
  parse_fen(pos, fen);

  u16 from_x = 0, from_y = 0;
  bool picking = false;

  while (!is_checkmate(pos) && !is_stalemate(pos)) {
    if (pos->side_to_move == WHITE) {
      u16 to_x, to_y;
      if (!touchscreen_get_point(&to_x, &to_y))
        continue;
      to_x /= BOARD_SQ_SIZE;
      to_y /= BOARD_SQ_SIZE;

      if (!picking) {
        // pick up
        if (board[to_x][to_y].piece_type != PIECE_EMPTY && from_x < 8 && from_x >= 0 &&
            from_y < 8 && from_y >= 0) {
          from_x = to_x;
          from_y = to_y;
          draw_highlight(to_x, to_y);
          picking = true;
        }
      } else { // attempt move
        clear_highlight(from_x, from_y);

        int from_sq = from_y * 8 + from_x;
        int to_sq = to_y * 8 + to_x;

        MoveList ml = generate_moves(pos); // only generates legal moves
        for (int i = 0; i < ml.count; i++) {
          Move m = ml.all_moves[i];
          if (m.from == from_sq && m.to == to_sq) {
            make_move(pos, m); // only move legal moves
            move(from_x, from_y, to_x, to_y);
            break;
          }
        }

        picking = false;
      }
    } else {
      // black
      u8 fx, fy, tx, ty;

      Move best_move = get_best_move(*pos);
      make_move(pos, best_move); // make the best move calculated

      fx = best_move.from % 8;
      fy = best_move.from / 8;
      tx = best_move.to % 8;
      ty = best_move.to / 8;

      move(fx, fy, tx, ty);
    }
    delay_ms(10); // small pause between turns
  }
  while (1)
    ;
}
