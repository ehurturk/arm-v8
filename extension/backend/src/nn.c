#include "nn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <cblas.h>

#define SZ_W1  ((size_t)NNUE_N_FEAT*NNUE_H1)
#define SZ_B1  ((size_t)NNUE_H1)
#define SZ_W2  ((size_t)NNUE_H1*NNUE_H2)
#define SZ_B2  ((size_t)NNUE_H2)
#define SZ_W3  ((size_t)NNUE_H2)


NNUE *nnue_init(const char *path) {
    FILE *f = fopen(path, "rb");
    if (!f) { printf("nnue_init fopen"); return NULL; }

    NNUE *net = calloc(1, sizeof(*net));
    if (!net) { fclose(f); return NULL; }

    net->W1 = malloc(SZ_W1 * sizeof(float));
    net->b1 = malloc(SZ_B1 * sizeof(float));
    net->W2 = malloc(SZ_W2 * sizeof(float));
    net->b2 = malloc(SZ_B2 * sizeof(float));
    net->W3 = malloc(SZ_W3 * sizeof(float));
    if (!net->W1||!net->b1||!net->W2||!net->b2||!net->W3) {
        fclose(f);
        nnue_free(net);
        return NULL;
    };

    if (fread(net->W1, sizeof(float), SZ_W1, f) != SZ_W1 ||
        fread(net->b1, sizeof(float), SZ_B1, f) != SZ_B1 ||
        fread(net->W2, sizeof(float), SZ_W2, f) != SZ_W2 ||
        fread(net->b2, sizeof(float), SZ_B2, f) != SZ_B2 ||
        fread(net->W3, sizeof(float), SZ_W3, f) != SZ_W3) {
        fclose(f);
        nnue_free(net);
        return NULL;
    }
    fclose(f);
    return net;
}

void nnue_free(NNUE *net) {
    if (!net) return;
    free(net->W1);
    free(net->b1);
    free(net->W2);
    free(net->b2);
    free(net->W3);
    free(net);
}

int nnue_eval(const NNUE *net, const Position *pos) {
    uint16_t feat[NNUE_K_ACTIVE];
    int k = 0;
    int k_self = pos->king_sq[pos->side_to_move];
    int k_opp  = pos->king_sq[!pos->side_to_move];

    for (int sq = 0; sq < 64 && k < NNUE_K_ACTIVE; ++sq) {
        for (int pi = 0; pi < 12 && k < NNUE_K_ACTIVE; ++pi) {
            if (pos->pieces[pi] & (1ULL<<sq)) {
                if (pi == (pos->side_to_move? 11:5)) continue;
                int code = (pi < 6 ? pi : pi); 
                int ref  = (pi < 6 && pos->side_to_move==0) ||
                           (pi >=6 && pos->side_to_move==1)
                           ? k_self : k_opp;
                feat[k++] = (uint16_t)((ref * 12 + code) * 64 + sq);
            }
        }
    }
    for (; k < NNUE_K_ACTIVE; ++k) feat[k] = UINT16_MAX;

    float h1[NNUE_H1], h2[NNUE_H2];

    // L1
    cblas_scopy(NNUE_H1, net->b1, 1, h1, 1);
    for (int i = 0; i < NNUE_K_ACTIVE; ++i) {
        uint16_t fi = feat[i];
        if (fi == UINT16_MAX) break;
        cblas_saxpy(NNUE_H1, 1.0f,
                    &net->W1[(size_t)fi * NNUE_H1], 1,
                    h1, 1);
    }
    // ReLU
    for (int j = 0; j < NNUE_H1; ++j)
        if (h1[j] < 0.0f) h1[j] = 0.0f;

    // L2
    cblas_scopy(NNUE_H2, net->b2, 1, h2, 1);
    cblas_sgemv(CblasRowMajor, CblasTrans,
                NNUE_H1, NNUE_H2,
                1.0f, net->W2, NNUE_H2,
                h1, 1,
                1.0f, h2, 1);
    for (int j = 0; j < NNUE_H2; ++j)
        if (h2[j] < 0.0f) h2[j] = 0.0f;

    // L3
    float out = cblas_sdot(NNUE_H2, net->W3, 1, h2, 1);

    return (int)out;
}
