#ifndef MOVEGEN_H
#define MOVEGEN_H

#include <stdint.h>
#include <stdbool.h>
#include "position.h"

#define PIECE_TYPE_COUNT       6 //per colour
#define WHITE                  0 
#define BLACK                  1
#define NUM_DIRECTIONS         8 
#define DEFAULT_MOVE_LIST_SIZE 256

//Flags
#define FLAG_NONE       0   //no flags
#define FLAG_CASTLE     1
#define FLAG_EN_PASSANT 2
#define FLAG_PROMOTE_Q  3
#define FLAG_PROMOTE_R  4
#define FLAG_PROMOTE_B  5
#define FLAG_PROMOTE_N  6

//Ray directions macros (for rooks, bishops, queens)
//vertical and horizontal ones
#define EAST       +1
#define WEST       -1
#define NORTH      +8
#define SOUTH      -8

//diagonal ones
#define NORTH_EAST +9
#define NORTH_WEST +7
#define SOUTH_WEST -9
#define SOUTH_EAST -7

typedef struct {
    uint8_t from, to;    // the squares we did 0-63
    uint8_t piece;       // enum Piece dt { WP, WN, … }
    uint8_t flags;       // capture, promo, EP, castle, stated above
} Move;

typedef struct {
    Move  all_moves[DEFAULT_MOVE_LIST_SIZE];
    int   count;
} MoveList;

enum {
    CAPTURE = 1 << 0,
    EP      = 1 << 1,
    PROMO   = 1 << 2,
    CASTLE  = 1 << 3
};

MoveList generate_moves(Position *pos);
void make_move(Position *pos, Move move);
bool is_checkmate(Position *pos);
bool is_stalemate(Position *pos);

#endif //MOVEGEN_H
