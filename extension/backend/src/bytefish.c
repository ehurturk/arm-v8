#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <time.h>
#include <cblas.h>

#define N_FEAT    49152   // 64*12*64
#define K_ACTIVE     31   // max active features per position
#define H1          256   // first hidden layer size
#define H2           32   // second hidden layer size

#pragma pack(push,1)
typedef struct {
    uint16_t idx[K_ACTIVE];  // sparse feature indices
    int16_t  cp;             // target score (centipawns/4)
} Rec;
#pragma pack(pop)

int train(int argc, char **argv) {
    if (argc != 6) {
        fprintf(stderr,
          "Follow this format: %s <shard.bin> <out.nnue> <epochs> <batch_size> <lr>\n",
          argv[0]
        );
        return 1;
    }
    const char *data_path = argv[1];
    const char *out_path   = argv[2];
    int epochs    = atoi(argv[3]);
    int B         = atoi(argv[4]);
    float lr      = atof(argv[5]);


    // --- Load raw shard into RAM ---
    FILE *f = fopen(data_path, "rb");
    if (!f) { perror("fopen shard"); return 1; }
    fseek(f, 0, SEEK_END);
    size_t file_size = ftell(f);
    rewind(f);
    int N = file_size / sizeof(Rec);
    Rec *data = malloc(file_size);
    if (fread(data, sizeof(Rec), N, f) != (size_t)N) {
        fprintf(stderr, "Error while reading %s\n", data_path);
        return 1;
    }
    fclose(f);

    // --- Allocate weights, biases, activations, grads ---
    float *W1 = calloc((size_t)N_FEAT * H1, sizeof(float));
    float *b1 = calloc(H1, sizeof(float));
    float *W2 = calloc((size_t)H1 * H2, sizeof(float));
    float *b2 = calloc(H2, sizeof(float));
    float *W3 = calloc(H2, sizeof(float));

    float *h1  = malloc(H1 * sizeof(float));
    float *h2  = malloc(H2 * sizeof(float));
    float *dW1 = calloc((size_t)N_FEAT * H1, sizeof(float));
    float *db1 = calloc(H1, sizeof(float));
    float *dW2 = calloc((size_t)H1 * H2, sizeof(float));
    float *db2 = calloc(H2, sizeof(float));
    float *dW3 = calloc(H2, sizeof(float));


    for (size_t i = 0; i < (size_t)N_FEAT * H1; ++i)
        W1[i] = ((rand() / (float)RAND_MAX) - 0.5f) * 0.02f;
    for (size_t i = 0; i < (size_t)H1 * H2; ++i)
        W2[i] = ((rand() / (float)RAND_MAX) - 0.5f) * 0.02f;
    for (int i = 0; i < H2; ++i)
        W3[i] = ((rand() / (float)RAND_MAX) - 0.5f) * 0.02f;

    int total_batches = (N + B - 1) / B;
    printf("TRAIN start: N=%d, epochs=%d, batch=%d, lr=%.5g → %d batches/epoch\n",
           N, epochs, B, lr, total_batches);
    fflush(stdout);

    // --- Training loop ---
    for (int ep = 0; ep < epochs; ++ep) {
        double loss = 0.0;
        memset(dW1, 0, (size_t)N_FEAT * H1 * sizeof(float));
        memset(db1, 0, H1 * sizeof(float));
        memset(dW2, 0, (size_t)H1 * H2 * sizeof(float));
        memset(db2, 0, H2 * sizeof(float));
        memset(dW3, 0, H2 * sizeof(float));

        int count = 0;
        for (int batch = 0, n = 0; n < N; n += B, ++batch) {
            int curB = (n + B <= N ? B : N - n);

            if ((batch % 10) == 0) {
                printf("\rEpoch %d/%d — Batch %d/%d",
                       ep+1, epochs, batch+1, total_batches);
                fflush(stdout);
            }

            // process each sample in mini-batch
            for (int i = 0; i < curB; ++i) {
                Rec *r = &data[n + i];

                // FL1
                cblas_scopy(H1, b1, 1, h1, 1); // h1 = b1
                for (int k = 0; k < K_ACTIVE; ++k) {
                    uint16_t idx = r->idx[k];
                    if (idx == UINT16_MAX) break;
                    // h1 += W1[row=idx]  (each row length = H1)
                    cblas_saxpy(H1, 1.0f,
                                &W1[(size_t)idx * H1], 1,
                                h1, 1);
                }
                for (int j = 0; j < H1; ++j)
                    if (h1[j] < 0.0f) h1[j] = 0.0f;

                // FL2
                cblas_scopy(H2, b2, 1, h2, 1);
                cblas_sgemv(CblasRowMajor, CblasTrans,
                            H1, H2, 1.0f, W2, H2,
                            h1, 1, 1.0f, h2, 1);
                for (int j = 0; j < H2; ++j)
                    if (h2[j] < 0.0f) h2[j] = 0.0f;

                // FL3
                float y = cblas_sdot(H2, W3, 1, h2, 1);
                float e = y - (float)r->cp;
                loss += e * e;

                // BL3
                for (int j = 0; j < H2; ++j)
                    dW3[j] += e * h2[j];

                // dh2
                float dh2[H2];
                for (int j = 0; j < H2; ++j) {
                    dh2[j] = (h2[j] > 0.0f) ? e * W3[j] : 0.0f;
                    db2[j] += dh2[j];
                }

                //BL2
                cblas_sger(CblasRowMajor, H1, H2,
                           1.0f, h1, 1, dh2, 1,
                           dW2, H2);

                //dh1
                float dh1[H1];
                memset(dh1, 0, sizeof dh1);
                for (int j = 0; j < H2; ++j) if (dh2[j] != 0.0f) {
                    for (int i1 = 0; i1 < H1; ++i1)
                        dh1[i1] += dh2[j] * W2[i1 * H2 + j];
                }
                for (int j = 0; j < H1; ++j) {
                    if (h1[j] <= 0.0f) dh1[j] = 0.0f;
                    db1[j] += dh1[j];
                }

                // BL1
                for (int k = 0; k < K_ACTIVE; ++k) {
                    uint16_t idx = r->idx[k];
                    if (idx == UINT16_MAX) break;
                    cblas_saxpy(H1, 1.0f,
                                dh1, 1,
                                &dW1[(size_t)idx * H1], 1);
                }

                ++count;
            }

            
            float scale = -lr / count;
            cblas_saxpy(H2, scale, dW3, 1, W3, 1);
            cblas_saxpy(H2, scale, db2, 1, b2, 1);
            cblas_saxpy((size_t)H1 * H2, scale, dW2, 1, W2, 1);
            cblas_saxpy(H1, scale, db1, 1, b1, 1);
            cblas_saxpy((size_t)N_FEAT * H1, scale, dW1, 1, W1, 1);
        }

        // end of epoch
        printf("\nEpoch %d/%d complete — MSE loss=%.6f\n", ep+1, epochs, loss / (double)count);
    }

    FILE *out = fopen(out_path, "wb");
    if (!out) {
        printf("fopen output");
        return 1;
    }

    fwrite(W1, sizeof(float), (size_t)N_FEAT * H1, out);
    fwrite(b1, sizeof(float), H1, out);
    fwrite(W2, sizeof(float), (size_t)H1 * H2, out);
    fwrite(b2, sizeof(float), H2, out);
    fwrite(W3, sizeof(float), H2, out);

    fclose(out);
    printf("Weights saved to %s\n", out_path);
    return 0;
}
