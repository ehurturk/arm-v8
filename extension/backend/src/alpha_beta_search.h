#ifndef ALPHA_BETA_SEARCH
#define ALPHA_BETA_SEARCH

#include "position.h"
#include "movegen.h"
#include "zobrist_hash.h"

#define MAX_DEPTH 8
#define TT_SIZE (1 << 20)

typedef struct {
    Position previous_pos;
} Undo;

extern TTEntry transposition_table[TT_SIZE];  //for hashing, and remembering previously evaluated positions
Move get_best_move(Position pos);

#endif 
