#include "alpha_beta_search.h"
#include "nn.h"
#include <stdio.h>
#include <assert.h>
#include <stdbool.h>
#include <limits.h>
#include <stdlib.h>

TTEntry transposition_table[TT_SIZE];

static Undo undo_stack[MAX_DEPTH]; //a stack to save previous positions 

static const Position *q_sort_pos; 

static const int piece_values[12] = {
    100, 320, 330, 500, 900, 20000,   // White: P N B R Q K
    100, 320, 330, 500, 900, 20000    // Black: p n b r q k
};

extern NNUE *engine_net;

bool init_evaluator(const char *nnue_path) {
    engine_net = nnue_init(nnue_path);
    if (!engine_net) {
        fprintf(stderr, "Failed to load NNUE from %s\n", nnue_path);
        return false;
    }
    return true;
}

static int evaluate_position(Position *pos) {
    return nnue_eval(engine_net, pos);
}

void clear_evaluator(void) {
    nnue_free(engine_net);
    engine_net = NULL;
}

int move_score(const Position *pos, const Move *move) {
    Bitboard dest_mask = 1ULL << move->to;
    int initializer = (pos->side_to_move == WHITE) ? 6: 0;
    int upper_bound = initializer + PIECE_TYPE_COUNT;

    for (int i = initializer; i < upper_bound; i++) {
        if (pos->pieces[i] & dest_mask) {  //captures
            return piece_values[i];
        }
    }

    return 0;
}

int move_compare(const void *a, const void *b) {
    const Move *ma = (const Move *)a;  //casting to Move type
    const Move *mb = (const Move *)b;

    return move_score(q_sort_pos, mb) - move_score(q_sort_pos, ma);   //descending order
}

static int ab_search(Position *pos, int depth, int alpha, int beta, bool maximizing) {
    uint64_t key = zobrist_hash(pos); 
    TTEntry *entry = &transposition_table[key % TT_SIZE];  

    if (entry->key == key && entry->depth >= depth) {  //this is at least as reliable as what we need now
        //if even UB is < maximizing, or LB is > minimizing, then prune
        if (entry->flag == TT_EXACT || (entry->flag == TT_LOWER && entry->score >= beta) || (entry->flag == TT_UPPER && entry->score <= alpha)) {
            return entry->score;  //prune search
        }
    }

    MoveList moves = generate_moves(pos);
    
    if (entry->key == key) {
        for (int i = 0; i < moves.count; i++) {
            if (moves.all_moves[i].from == entry->best_move.from && moves.all_moves[i].to == entry->best_move.to 
                && moves.all_moves[i].flags == entry->best_move.flags) {
                Move temp = moves.all_moves[0];  
                moves.all_moves[0] = moves.all_moves[i]; //prioritize entry->best_move
                moves.all_moves[i] = temp;
                break;
            }
        }
    }   
    
    //sort in into descending order by 
    q_sort_pos = pos;
    if (moves.count > 1) {
        qsort(&moves.all_moves[1], moves.count - 1, sizeof(Move), move_compare);
    }

    if (depth == 0  || moves.count == 0) {
        return evaluate_position(pos);
    }

    int best = (maximizing) ? INT_MIN : INT_MAX;
    Move best_move = (Move) {0};

    for (int i = 0; i <moves.count; i++) {
        Move curr_move = moves.all_moves[i];
        assert(depth < MAX_DEPTH);
        undo_stack[depth].previous_pos = *pos;  //save previous position, marked by 'depth'

        make_move(pos, curr_move); 
        int score = ab_search(pos, depth - 1, alpha, beta, !maximizing);
        *pos = undo_stack[depth].previous_pos; 

        if (maximizing) {
            if (score > best) {
                best = score;
                best_move = curr_move;
            }
            if (score > alpha) alpha = score;
        } else {
            if (score < best) {
                best = score;
                best_move = curr_move;
            }
            if (score < beta) beta = score;
        }

        if (beta <= alpha) break;  //prune
    }

    //store this position into a transposition table with details
    entry->key = key; //key is specified by position
    entry->depth = depth;
    entry->score = best;
    entry->best_move = best_move;

    if (best <= alpha) {
        entry->flag = TT_UPPER;
    } else if (best >= beta) {
        entry->flag = TT_LOWER;
    } else {
        entry->flag = TT_EXACT;
    }

    return best;
}

//Main function that calls all helper functions
Move get_best_move(Position pos) {  //MAIN loop function that checks every possible move
    Move best_move = (Move){0};
    int best_score = INT_MIN;
    MoveList moves = generate_moves(&pos);

    for (int i = 0; i<moves.count; i++) {
        Move move = moves.all_moves[i];
        Position backup = pos;  //make a copy to come back to after a single move 

        make_move(&pos, move); 
        int score = ab_search(&pos, MAX_DEPTH - 1, INT_MIN, INT_MAX, pos.side_to_move == WHITE);
        pos = backup;

        if (best_score < score) {
            best_score = score;
            best_move = moves.all_moves[i];
        }
    }
    return best_move;
} 