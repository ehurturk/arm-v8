#ifndef NNUE_H
#define NNUE_H

#include <stdint.h>
#include "position.h"   

#define NNUE_N_FEAT   49152
#define NNUE_K_ACTIVE   31
#define NNUE_H1        256
#define NNUE_H2         32

typedef struct {
    float *W1;
    float *b1;
    float *W2;
    float *b2;
    float *W3;
} NNUE;

extern NNUE *engine_net;

NNUE *nnue_init(const char *path);

void nnue_free(NNUE *net);

int nnue_eval(const NNUE *net, const Position *pos);

#endif
