#ifndef ZOBRIST_HASH_H
#define ZOBRIST_HASH_H
#include <stdint.h>
#include "movegen.h"
#include "position.h"

#define TT_SIZE (1 << 20)

typedef enum {
    TT_EXACT,
    TT_LOWER,
    TT_UPPER
} TTFlag;

typedef struct {
    uint64_t key;
    int depth;
    int score;
    TTFlag flag;
    Move best_move;
} TTEntry;

void init_zobrist();
uint64_t zobrist_hash(Position *pos);

#endif
