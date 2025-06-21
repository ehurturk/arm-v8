#include "position.h"
#include <stdlib.h>
#include "string.h"
#include "assert.h"
#include <ctype.h>

static void clear_position(Position *p)
{
    memset(p, 0, sizeof(Position));
    p->enpass_square = -1;
}

void parse_fen(Position *p, const char *fen)
{
    clear_position(p);

    int row = 7, file = 0;
    const char *s = fen;

    while (*s && *s != ' ') {
        char c = *s++;

        if (c == '/') {
            row--; file = 0;
            continue;
        }
        if (isdigit((unsigned char)c)) {
            file += c - '0';
            continue;
        }

        const int sq = bb_pos(file, row);
        Bitboard mask = BIT(sq);
        switch (c) {
            case 'P': p->pieces[WP] |= mask; break;
            case 'N': p->pieces[WN] |= mask; break;
            case 'B': p->pieces[WB] |= mask; break;
            case 'R': p->pieces[WR] |= mask; break;
            case 'Q': p->pieces[WQ] |= mask; break;
            case 'K': p->pieces[WK] |= mask; p->king_sq[0] = sq; break;
            case 'p': p->pieces[BP] |= mask; break;
            case 'n': p->pieces[BN] |= mask; break;
            case 'b': p->pieces[BB] |= mask; break;
            case 'r': p->pieces[BR] |= mask; break;
            case 'q': p->pieces[BQ] |= mask; break;
            case 'k': p->pieces[BK] |= mask; p->king_sq[1] = sq; break;
            default:  fprintf(stderr, "unknown piece '%c'\n", c); return;
        }
        file++;
    }

    //side
    if (*s == ' ') ++s;
    p->side_to_move = (*s == 'w') ? 0 : 1;
    while (*s && *s != ' ') ++s;
    // castle
    if (*s == ' ') ++s;
    if (*s == '-') ++s;   /* no rights */
    else {
        while (*s && *s != ' ') {
            switch (*s++) {
                case 'K': p->castle_rights |= CASTLE_WK; break;
                case 'Q': p->castle_rights |= CASTLE_WQ; break;
                case 'k': p->castle_rights |= CASTLE_BK; break;
                case 'q': p->castle_rights |= CASTLE_BQ; break;
                default:  break;
            }
        }
    }
    //enpass the goat
    if (*s == ' ') ++s;
    if (*s != '-') {
        const int fileChar = *s++ - 'a';
        const int rowChar = *s++ - '1';
        p->enpass_square = bb_pos(fileChar, rowChar);
    }
    else { ++s; }

    for (int i = 0; i < 6; ++i) {
        p->whiteAll |= p->pieces[i];
        p->blackAll |= p->pieces[i + 6];
    }
    p->all = p->whiteAll | p->blackAll;
}

Position *make_empty_Position() {
    Position *pos = malloc(sizeof(Position));
    assert(pos != NULL);
    memset(pos, 0, sizeof(Position));
    return pos;
}