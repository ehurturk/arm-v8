#ifndef BITBOARD_H
#define BITBOARD_H

#include <stdint.h>
#include <stdio.h>

typedef uint64_t Bitboard;

enum Square {
    A1, B1, C1, D1, E1, F1, G1, H1,
    A2, B2, C2, D2, E2, F2, G2, H2,
    A3, B3, C3, D3, E3, F3, G3, H3,
    A4, B4, C4, D4, E4, F4, G4, H4,
    A5, B5, C5, D5, E5, F5, G5, H5,
    A6, B6, C6, D6, E6, F6, G6, H6,
    A7, B7, C7, D7, E7, F7, G7, H7,
    A8, B8, C8, D8, E8, F8, G8, H8,
    SQ_NONE = 64
};

#define BB_EMPTY 0ULL
#define BIT(sq)  (1ULL << (sq))

#define NUM_ROWS 8
#define NUM_COLUMNS 8


#define bb_pos(file, row) ((row) * 8 + (file))

static inline int pop_lsb(Bitboard *bb) {
    int sq = __builtin_ctzll(*bb);
    // this function helps find trailing zero which tells the LSB position
    *bb &= *bb - 1;
    return sq;
}

static inline int bb_count(Bitboard bb) {
    return __builtin_popcountll(bb);
}

static inline void print_bb(Bitboard bb) {
    for (int r = 7; r >= 0; --r) {
        for (int f = 0; f < 8; ++f) {
            int sq = r * 8 + f;
            putchar((bb & BIT(sq)) ? '1' : '.');
            putchar(' ');
        }
        putchar('\n');
    }
    putchar('\n');
}

extern Bitboard FILE_MASK[8];
extern Bitboard ROW_MASK[8];
extern Bitboard DIAG_MASK[15];
extern Bitboard ANTI_MASK[15];

extern Bitboard KNIGHT_ATT[64];
extern Bitboard KING_ATT[64];
extern Bitboard PawnCapW[64];
extern Bitboard PawnCapB[64];

void init_tables(void);
#endif
