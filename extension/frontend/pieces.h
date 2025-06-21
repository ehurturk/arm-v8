#ifndef PIECES_H
#define PIECES_H

#include "../defs.h"

#define PIECE_IMAGE_SIZE 1024

typedef enum {
  PIECE_EMPTY = 0,
  PIECE_PAWN = 1,
  PIECE_KNIGHT = 2,
  PIECE_BISHOP = 3,
  PIECE_ROOK = 4,
  PIECE_QUEEN = 5,
  PIECE_KING = 6,
  PIECE_NR = 7
} PIECES;

extern u8 *pieces[PIECE_NR];
extern u8 piece_Pawn[PIECE_IMAGE_SIZE];

typedef struct {
  int col;
  PIECES piece_type;
} piece;

extern piece board[8][8];


#endif //PIECES_H