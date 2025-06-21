// main.c — Play White vs. your NNUE-powered engine as Black
// =========================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bitboard.h"
#include "position.h"
#include "movegen.h"
#include "alpha_beta_search.h"
#include "nn.h"

// Global NNUE pointer
NNUE *engine_net = NULL;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <nnue_file>\n", argv[0]);
        return 1;
    }

    // 1) Init bitboard attack tables
    init_tables();

    // 2) Load your trained NNUE
    engine_net = nnue_init(argv[1]);
    if (!engine_net) {
        fprintf(stderr, "Failed to load NNUE from %s\n", argv[1]);
        return 1;
    }

    // 3) Set up starting position
    Position *pos = make_empty_Position();
    parse_fen(pos,
        "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"
    );

    // 4) Game loop: White (you) then Black (engine)
    char line[32];
    while (1) {
        // --- Your move ---
        printf("Your move (e.g. e2e4, quit): ");
        if (!fgets(line, sizeof line, stdin)) break;   // EOF
        line[strcspn(line, "\r\n")] = '\0';            // strip newline
        if (strcmp(line, "quit") == 0) break;

        if (strlen(line) < 4) {
            printf("  >> invalid input, try again\n");
            continue;
        }

        // Parse UCI from/to
        int from = (line[0] - 'a') + 8 * (line[1] - '1');
        int to   = (line[2] - 'a') + 8 * (line[3] - '1');

        // Generate legal moves and look for a match
        MoveList ml = generate_moves(pos);
        if (ml.count == 0) {
            printf("  >> no legal moves: game over\n");
            break;
        }

        int found = 0;
        for (int i = 0; i < ml.count; ++i) {
            Move m = ml.all_moves[i];
            if (m.from == from && m.to == to) {
                make_move(pos, m);
                found = 1;
                break;
            }
        }
        if (!found) {
            printf("  >> illegal move: %s\n", line);
            continue;
        }

        // --- Engine (Black) move ---
        Move best = get_best_move(*pos);
        // Print it in UCI form
        char uf = 'a' + (best.from % 8);
        char ur = '1' + (best.from / 8);
        char tf = 'a' + (best.to   % 8);
        char tr = '1' + (best.to   / 8);
        printf("Engine plays: %c%c%c%c\n", uf, ur, tf, tr);

        make_move(pos, best);
    }

    // 5) Cleanup
    nnue_free(engine_net);
    free(pos);
    printf("Goodbye!\n");
    return 0;
}
