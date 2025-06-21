#ifndef POSITION_H
#define POSITION_H

#include "bitboard.h"

enum Piece {
    WP, WN, WB, WR, WQ, WK,
    BP, BN, BB, BR, BQ, BK,
    PIECE_NONE = 12
};

enum {
    CASTLE_WK = 1 << 0,
    CASTLE_WQ = 1 << 1,
    CASTLE_BK = 1 << 2,
    CASTLE_BQ = 1 << 3
};

typedef struct {
    Bitboard pieces[12];               // piece-specific board for each COLOUR and TYPE
    Bitboard whiteAll, blackAll, all;  //colour-specific board and WHOLE board

    uint8_t  side_to_move;             //0: white    1: black
    uint8_t  castle_rights;            //0001 WK short, 0010 WK long; 0100 BK short, 1000 BK long;
    int8_t  enpass_square;             //the square which enpassant lands on, -1 = none

    uint8_t  king_sq[2];
} Position;


void parse_fen(Position *pos, const char *fen);
Position *make_empty_Position();

#endif //POSITION_H