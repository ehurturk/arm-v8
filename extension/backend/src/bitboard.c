#include "bitboard.h"
#include <stdbool.h>

Bitboard FILE_MASK[8];
Bitboard ROW_MASK[8];
Bitboard DIAG_MASK[15];
Bitboard ANTI_MASK[15];

Bitboard KNIGHT_ATT[64];
Bitboard KING_ATT[64];
Bitboard PawnCapW[64];
Bitboard PawnCapB[64];

static inline bool on_board(int const file, int const row) {
    return ((file >= 0) && (file < NUM_COLUMNS) && (row >= 0) && (row < NUM_ROWS));
}

void init_tables(void) {
    static bool done = false;
    if (done) return;

    const int kdr[8] = {+2, +2, -2, -2, -1, +1, -1, +1};
    const int kdf[8] = {-1, +1, -1, +1, +2, +2, -2, -2};

    for (int f = 0; f < NUM_COLUMNS; ++f) {
        for (int r = 0; r < NUM_ROWS; ++r) {
            int const currBit = bb_pos(f, r);
            Bitboard const currPos = BIT(currBit);

            // All the masks
            FILE_MASK[f] |= currPos;
            ROW_MASK[r] |= currPos;
            ANTI_MASK[f + r] |= currPos;
            DIAG_MASK[f - r + 7] |= currPos;

            // All the Knight Attacks
            for (int i = 0; i < 8; ++i) {
                if (on_board(f + kdf[i], r + kdr[i])) {
                    KNIGHT_ATT[currBit] |= BIT(bb_pos(
                                                    f + kdf[i],
                                                    r + kdr[i]
                                                    ));
                }
            }

            // All the King Attacks
            for (int df = -1; df <= 1; ++df) {
                for (int dr = -1; dr <= 1; ++dr) {
                    if (df == 0 && dr == 0) continue;
                    if (on_board(f + df, r + dr)) {
                        KING_ATT[currBit] |= BIT(bb_pos(f + df, r + dr)) ;
                    }
                }
            }

            // All the Pawn attacks
            if (on_board(f + 1, r + 1)) {
                PawnCapW[currBit] |= BIT(bb_pos(f + 1, r + 1));
            }
            if (on_board(f - 1, r + 1)) {
                PawnCapW[currBit] |= BIT(bb_pos(f - 1, r + 1));
            }
            if (on_board(f + 1, r - 1)) {
                PawnCapB[currBit] |= BIT(bb_pos(f + 1, r - 1));
            }
            if (on_board(f - 1, r - 1)) {
                PawnCapB[currBit] |= BIT(bb_pos(f - 1, r - 1));
            }
        }
    }
    done = true;
}