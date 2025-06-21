#include "zobrist_hash.h"
#include "bitboard.h"
#include <stdlib.h>

uint64_t zobrist_table[PIECE_TYPE_COUNT*2][NUM_COLUMNS*NUM_ROWS]; //table for every piece, and square
uint64_t zobrist_side; //colour
uint64_t zobrist_castle[16]; //castling
uint64_t zobrist_ep[8]; //en passant

uint64_t random_u64() {
    return ((uint64_t)rand() << 32) | rand(); //generate random 64-bit uint
}

//intialise everything
void init_zobrist() {
    for (int piece_type = 0; piece_type < PIECE_TYPE_COUNT *2; ++piece_type) {
        for (int sq = 0; sq < 64; ++sq) {
            zobrist_table[piece_type][sq] = random_u64(); //assign random uint64 to each square
        }
    }

    zobrist_side = random_u64();
    for (int i = 0; i < 16; ++i) {
        zobrist_castle[i] = random_u64();
    }

    for (int i = 0; i < 8; ++i) {
        zobrist_ep[i] = random_u64();
    }
}

//get zobrist key for a position 
uint64_t zobrist_hash(Position *pos) {
    uint64_t hash = 0ULL;

    for (int piece = 0; piece < 12; ++piece) {
        Bitboard bb = pos->pieces[piece];
        while (bb) {
            int sq = pop_lsb(&bb);
            hash ^= zobrist_table[piece][sq];
        }
    }

    if (pos->side_to_move == BLACK) {
        hash ^= zobrist_side;
    }

    hash ^= zobrist_castle[pos->castle_rights & 0xF]; 

    if (pos->enpass_square != -1) {
        int file = pos->enpass_square % 8;
        hash ^= zobrist_ep[file];
    }

    return hash;
}


