#include "movegen.h"
#include "position.h"
#include "bitboard.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

//all possible ray direction for rooks and bishop
static const int directions[NUM_DIRECTIONS] = {EAST, WEST, NORTH, SOUTH, NORTH_WEST, NORTH_EAST, SOUTH_WEST, SOUTH_EAST}; 

static int get_flags(Position *pos, int piece_index, int dest) {
    int rank = dest /8;

    if ((piece_index == WP || piece_index == BP) && (dest == pos->enpass_square)) {return FLAG_EN_PASSANT;}
    if ((piece_index == WP && rank == 7) || (piece_index == BP && rank == 0)) {return FLAG_PROMOTE_Q;} //default to queen promotion
    if (((piece_index == WK) && (dest == 6 || dest == 2)) || (piece_index == BK && (dest == 62 || dest == 58))) {return FLAG_CASTLE;}

    return 0;
}

//for single and double pawn pushes 
static Bitboard get_pawn_pushes(Position *pos, int from, int piece_index) {
    Bitboard pushes = 0ULL;

    int rank = from / 8;
    int to_one, to_two; //single or double push

    if (piece_index == WP) {
        to_one = from + 8;   //one rank ahead
        to_two = from + 16;  //two ranks ahead

        //has to be on the board, and the square one ahead is not occupied
        if (to_one < 64 && !(pos->all & (1ULL << to_one))) {  
            pushes |= (1ULL << to_one);

            //allows double push only if starting rank and 2 ahead is not occupied
            if (rank == 1 && !(pos->all & (1ULL << to_two))) {
                pushes |= (1ULL << to_two);
            }
        }

    } else if (piece_index == BP) {
        to_one = from - 8;
        to_two = from - 16;

        if (to_one >= 0 && !(pos->all & (1ULL << to_one))) {
            pushes |= (1ULL << to_one);

            if (rank == 6 && !(pos->all & (1ULL << to_two))) {
                pushes |= (1ULL << to_two);
            }
        }
    }

    return pushes;
}

// for rooks, bishops and queens attack
static Bitboard ray_attacks(Bitboard occupied, int from, const int *dirs, int num_dirs) {
    Bitboard atks = 0ULL;
    
    //consider only given directions: 0-3 are straight, 4-7 are diagonal from directions global array 
    for (int i = 0; i < num_dirs; ++i) {
        int sq = from; 
        int dir = dirs[i];
    
        while (true) {
            int next = sq + dir;  //next = one step in that direction
            if (next < 0 || next > 63) break;  //check on board

            //check it is wrap around 
            bool is_horizontal = (dir == WEST || dir == EAST);
            bool is_diagonal = (dir == NORTH_EAST || dir == NORTH_WEST || dir == SOUTH_EAST || dir == SOUTH_WEST);

            // if we have a wrap around, break. Otherwise add square as possible square 
            if (abs((next % 8) - (sq % 8)) > 1 && (is_horizontal || is_diagonal)) break;
            atks |= (1ULL << next);
            if (occupied & (1ULL << next)) break;  //if the square is occupied, search no further

            sq = next; //move onto next step if 
        }
    }
    return atks;
}

void make_move(Position *pos, Move move) {
    Bitboard from_mask = 1ULL << move.from;
    Bitboard dest_mask = 1ULL << move.to;

    //consider en_passant and castling
    bool is_enpass = (move.piece == WP || move.piece == BP) && (move.to == pos->enpass_square) && ((move.from % 8) != (move.to % 8));
    bool is_castling = (move.piece == WK && move.from == 4 && move.to == 6 && (pos->castle_rights & (1 << 0))) || 
                        (move.piece == WK && move.from == 4 && move.to == 2 && (pos->castle_rights & (1 << 1))) || 
                        (move.piece == BK && move.from == 60 && move.to == 62 && (pos->castle_rights & (1 << 2))) || 
                        (move.piece == BK && move.from == 60 && move.to == 58 && (pos->castle_rights & (1 << 3)));  

    if (is_enpass) {
        int captured_square = (move.piece == WP) ? move.to - 8 : move.to + 8;
        Bitboard mask = 1ULL << captured_square;
        pos->pieces[move.piece < 6 ? BP : WP] &= ~mask;
        pos->all &= ~mask;
        if (move.piece < 6) {
            pos->blackAll &= ~mask;
        } else {
            pos->whiteAll &= ~mask;
        }
        pos->pieces[move.piece] &= ~from_mask;
        pos->pieces[move.piece] |= dest_mask;

    } else if (is_castling) {
        pos->pieces[move.piece] &= ~from_mask;
        pos->pieces[move.piece] |= dest_mask;

         if (move.piece == WK) {
            if (move.to == 6) { // WK short
                pos->pieces[WR] &= ~(1ULL << 7);
                pos->pieces[WR] |= (1ULL << 5);
                pos->whiteAll &= ~(1ULL << 7);
                pos->whiteAll |= (1ULL << 5);
            } else if (move.to == 2) { // WQ long
                pos->pieces[WR] &= ~(1ULL << 0);
                pos->pieces[WR] |= (1ULL << 3);
                pos->whiteAll &= ~(1ULL << 0);
                pos->whiteAll |= (1ULL << 3);
            }
        } else { 
            if (move.to == 62) { // BK short
                pos->pieces[BR] &= ~(1ULL << 63);
                pos->pieces[BR] |= (1ULL << 61);
                pos->blackAll &= ~(1ULL << 63);
                pos->blackAll |= (1ULL << 61);
            } else if (move.to == 58) { // BQ long
                pos->pieces[BR] &= ~(1ULL << 56);
                pos->pieces[BR] |= (1ULL << 59);
                pos->blackAll &= ~(1ULL << 56);
                pos->blackAll |= (1ULL << 59);
            }
        }

        // disable castling rights after castling
        if (move.piece == WK) pos->castle_rights &= ~0b0011;
        if (move.piece == BK) pos->castle_rights &= ~0b1100;

    } else {
        //normal moves and captures
        pos->pieces[move.piece] &= ~from_mask;

        int initializer = (pos->side_to_move == BLACK) ? 0 : 6;
        int upper_bound = initializer + 6;

        for (int i = initializer; i < upper_bound; i++) {
            if (pos->pieces[i] & dest_mask) {
                pos->pieces[i] &= ~dest_mask;
                break;
            }
        }
        //check if the move is a promotion 
        int rank = move.to /8;
        if ((move.piece == WP && rank == 7) || (move.piece == BP && rank == 0)) {
            int new;
            //get the flags for a promotion
            switch (move.flags) {
                case FLAG_PROMOTE_R:
                    new = (move.piece == WP) ? WR : BR; 
                    break;
                case FLAG_PROMOTE_B:
                    new = (move.piece == WP) ? WB : BB; 
                    break;
                case FLAG_PROMOTE_N:
                    new = (move.piece == WP) ? WN : BN; 
                    break;
                case FLAG_PROMOTE_Q:  //default is queen
                default: 
                    new = (move.piece == WP) ? WQ : BQ;
                    break;
            }

            pos->pieces[new] |= dest_mask;

        } else {
            //otherwise just add a new piece
            pos->pieces[move.piece] |= dest_mask;
        }

        //add/remove to/from respective colours
        if (pos->side_to_move == BLACK) {
            pos->blackAll &= ~from_mask;
            pos->blackAll |= dest_mask;
            pos->whiteAll &= ~dest_mask;
        } else {
            pos->whiteAll &= ~from_mask;
            pos->whiteAll |= dest_mask;
            pos->blackAll &= ~dest_mask;
        }
    }
    
    pos->all = pos->whiteAll | pos->blackAll;

    //only for king moves, change its square
    if (move.piece == WK) pos->king_sq[WHITE] = move.to;
    if (move.piece == BK) pos->king_sq[BLACK] = move.to;

    //double push for en passant, set square for flag
    if (move.piece == WP && move.from / 8 == 1 && move.to / 8 == 3) {
        int ep_s = move.from + 8; 
        int file = ep_s % 8;
        //MUST check that opponent has pawn adjacent after double push
        bool has_adjacent_opponent_pawn = (file > 0 && (pos->pieces[BP] & (1ULL << (ep_s - 1)))) ||
                                            (file < 7 && (pos->pieces[BP] & (1ULL << (ep_s + 1))));

        pos->enpass_square = has_adjacent_opponent_pawn ? ep_s : -1;
    } else if (move.piece == BP && move.from / 8 == 6 && move.to / 8 == 4) {
        int ep_s = move.from - 8;
        int file = ep_s % 8;
        bool has_adjacent_opponent_pawn = (file > 0 && (pos->pieces[WP] & (1ULL << (ep_s - 1)))) ||
                                            (file < 7 && (pos->pieces[WP] & (1ULL << (ep_s + 1))));

        pos->enpass_square = has_adjacent_opponent_pawn ? ep_s : -1;
    } else {
        pos->enpass_square = -1;
    }

    //checking castling rights
    if (!is_castling) {
        if (move.piece == WK) pos->castle_rights &= ~0b0011;  //disable castling rights
        if (move.piece == BK) pos->castle_rights &= ~0b1100;

        if (move.from == 0 || move.to == 0) pos->castle_rights &= ~(1 << 1); // WQ
        if (move.from == 7 || move.to == 7) pos->castle_rights &= ~(1 << 0); // WK
        if (move.from == 56 || move.to == 56) pos->castle_rights &= ~(1 << 3); // BQ
        if (move.from == 63 || move.to == 63) pos->castle_rights &= ~(1 << 2); // BK
    }
    pos->side_to_move ^= 1;
}

static bool king_is_attacked(Position *pos) {
    int king_colour = pos->side_to_move ^ 1;
    int king_square = pos->king_sq[king_colour];
    int enemy_index_offset = (king_colour == BLACK) ? 0 : 6;

    if (king_colour == WHITE) {
        if (PawnCapW[king_square] & pos->pieces[enemy_index_offset]) return true;
    } else {
        if (PawnCapB[king_square] & pos->pieces[enemy_index_offset]) return true;
    }
    if (KNIGHT_ATT[king_square] & pos->pieces[enemy_index_offset + 1]) return true;
    if (KING_ATT[king_square] & pos->pieces[enemy_index_offset + 5]) return true;

    for (int i = 0; i < NUM_DIRECTIONS; ++i) { //0-3 is straight, 4-7 is diagonal, every ray direction
        int dir = directions[i];
        int k = king_square;
    
        while (true) {
            int from_file = k % 8;
            k += dir;
            Bitboard mask = 1ULL << k;
            if (k < 0 || k > 63) break;  //out of bounds
            
            int to_file = k % 8; //new k after step in specific direction

            bool is_horizontal = (dir == EAST || dir == WEST);
            bool is_diagonal = (dir == NORTH_EAST || dir == NORTH_WEST || dir == SOUTH_EAST || dir == SOUTH_WEST);

            if ((is_diagonal || is_horizontal) && (abs(to_file - from_file) > 1)) { //check for wrap-around error
                break;
            }

            if (mask & pos->all) {  //something on that square
                bool found = (i < 4) ? (pos->pieces[enemy_index_offset+3] & mask) || (pos->pieces[enemy_index_offset+4] & mask)
                            : (pos->pieces[enemy_index_offset+2] & mask) || (pos->pieces[enemy_index_offset+4] & mask);

                if (found) return true;
                break;  //cannot go over a piece that is blocking
            }
        }
    }
    //after we check all possible captures:
    return false;
}

// eliminate all illegal moves in this function by checking every possible move
static bool is_legal_move(Position *pos, int piece_index, int from, int dest) {
    Bitboard dest_mask = 1ULL << dest;
    if ((pos->side_to_move == WHITE && (pos->whiteAll & dest_mask)) ||
        (pos->side_to_move == BLACK && (pos->blackAll & dest_mask))) {
        return false; 
    }

    //castling, check if we can still castle 
    bool is_castling = (piece_index == WK && from == 4 && dest == 6 && (pos->castle_rights & (1 << 0))) || // WK
                        (piece_index == WK && from == 4 && dest == 2 && (pos->castle_rights & (1 << 1))) || // WQ
                        (piece_index == BK && from == 60 && dest == 62 && (pos->castle_rights & (1 << 2))) || // BK
                        (piece_index == BK && from == 60 && dest == 58 && (pos->castle_rights & (1 << 3)));  // BQ

    if (is_castling) {
        int king_path[3];
        int len = 3;
        //check no pieces block the castling paths
        if ((dest == 6 && ((pos->all & ((1ULL << 5) | (1ULL << 6))) != 0)) ||  // WK
            (dest == 2 && ((pos->all & ((1ULL << 1) | (1ULL << 2) | (1ULL << 3))) != 0)) || // WQ
            (dest == 62 && ((pos->all & ((1ULL << 61) | (1ULL << 62))) != 0)) || // BK
            (dest == 58 && ((pos->all & ((1ULL << 57) | (1ULL << 58) | (1ULL << 59))) != 0))) { // BQ
            return false;
        }

        //check king is not under attack while castling
        switch (dest) {
            case 6: king_path[0] = 4; king_path[1] = 5; king_path[2] = 6; break; //WK
            case 2: king_path[0] = 4; king_path[1] = 3; king_path[2] = 2; break; //WQ
            case 62: king_path[0] = 60; king_path[1] = 61; king_path[2] = 62; break; //BK
            case 58: king_path[0] = 60; king_path[1] = 59; king_path[2] = 58; break; //BQ
            default: len = 0; break;
        }

        //check we are not moving into a check, as it is illegal
        for (int i = 0; i < len; i++) {
            Position temp = *pos;
            temp.king_sq[(piece_index == WK) ? WHITE : BLACK] = king_path[i];
            temp.pieces[piece_index] = 1ULL << king_path[i];
            temp.all = temp.whiteAll | temp.blackAll;

            if (king_is_attacked(&temp)) return false;
        }
    }

    Position copy = *pos;
    Move test_move = {
        .from = (uint8_t) from,
        .to = (uint8_t) dest,
        .piece = (uint8_t) piece_index,
        .flags = get_flags(pos, piece_index, dest)
    };
    make_move(&copy, test_move);  //try to make 'fake' move on a copy of the current position

    if (king_is_attacked(&copy)) return false;  //if we move into a check, then we have illegal move
    return true;
}


//this is to generate moves for every possible piece that is left on the board
static void generate_piece_moves(Position *pos, int from, int piece_index, MoveList *move_list) {
    Bitboard atk = 0; 
    Bitboard not_own_pieces = (pos->side_to_move == BLACK) ? ~(pos->blackAll) : ~(pos->whiteAll);
    switch (piece_index) {
        case WP: 
            atk = ((PawnCapW[from] & pos->blackAll) & not_own_pieces) | get_pawn_pushes(pos, from, WP);

            //When we have an en passant possible for white
            if (pos->enpass_square != -1 && ((pos->enpass_square / 8) == 5)) { //enpassant is on 6th rank 
                if (abs((pos->enpass_square % 8) - (from % 8)) == 1) { //must have one file differnce 
                    atk |= (1ULL << pos->enpass_square); //add enpass square
                }
            }
            break;
        case BP: 
            atk = ((PawnCapB[from] & pos->whiteAll) & not_own_pieces) | get_pawn_pushes(pos, from, BP);

            //when we have an en passant possible for black
            if (pos->enpass_square != -1 && ((pos->enpass_square / 8) == 2)) {
                if (abs((pos->enpass_square % 8) - (from % 8)) == 1) {
                    atk |= (1ULL << pos->enpass_square);
                }
            }
            break;
        case WN:
        case BN:
            atk = KNIGHT_ATT[from] & not_own_pieces;
            break;
        case WB:
        case BB: //only diagonal ray directions
            atk = ray_attacks(pos->all, from, directions + 4, 4) & not_own_pieces;
            break;
        case WR:
        case BR: //only horizontal/vertical ray directions
            atk = ray_attacks(pos->all, from, directions, 4) & not_own_pieces;
            break;
        case WQ: 
        case BQ: //ray attack with all directions,
            atk = ray_attacks(pos->all, from, directions, 8) & not_own_pieces;
            break;
        case WK: 
        case BK: 
            atk = KING_ATT[from] & not_own_pieces;

            //castling moves if castling rights is true, add possible king move
            if (piece_index == WK && (pos->castle_rights & 0b0001)) atk |= (1ULL << 6); // WK short
            if (piece_index == WK && (pos->castle_rights & 0b0010)) atk |= (1ULL << 2); // WQ long
            if (piece_index == BK && (pos->castle_rights & 0b0100)) atk |= (1ULL << 62); // BK
            if (piece_index == BK && (pos->castle_rights & 0b1000)) atk |= (1ULL << 58); // BQ
            break;
    }
    
    while (atk) {
        int dest = pop_lsb(&atk); //look at every possible move (even illegal ones) which will be filtered later
        int rank = dest /8;
        if (!is_legal_move(pos, piece_index, from, dest)) continue;

        if ((piece_index == WP && rank == 7) || (piece_index == BP && rank == 0)) {
            //every possible promotion is a possible move
            int p_flags[4] = {FLAG_PROMOTE_Q, FLAG_PROMOTE_R, FLAG_PROMOTE_B, FLAG_PROMOTE_N};
            for (int i = 0; i < 4; i++) {
                Move move = {
                    .from = (uint8_t) from,
                    .to = (uint8_t) dest, 
                    .piece = (uint8_t) piece_index,
                    .flags = p_flags[i]
                };
                move_list->all_moves[move_list->count++] = move;
            }
        } else { 
            Move move = {
                .from = (uint8_t) from, 
                .to = (uint8_t) dest, 
                .piece = (uint8_t) piece_index,
                .flags = get_flags(pos, piece_index, dest) 
            };
            move_list->all_moves[move_list->count++] = move;
        }
    }
}

MoveList generate_moves(Position *pos) { // for 1 turn, i.e. white 0 OR black 1
    MoveList moves;
    moves.count = 0;

    for (int i = 0; i < PIECE_TYPE_COUNT; i++) {
        int index = (pos->side_to_move == BLACK) ? i+6 : i;
        Bitboard bb = pos->pieces[index];
        if (!bb) continue;  //this type of piece does not exist

        while (bb) {
            int src = pop_lsb(&bb);
            generate_piece_moves(pos, src, index, &moves);
        }
    }
    return moves;
}

//check for checkmate and stalemates to end a chess game, using the legal moves == 0;
bool is_checkmate(Position *pos) {
    if (!king_is_attacked(pos)) return false;
    MoveList moves = generate_moves(pos);
    return moves.count == 0; 
}

bool is_stalemate(Position *pos) {
    if (king_is_attacked(pos)) return false;
    MoveList moves = generate_moves(pos);
    return moves.count == 0;
}