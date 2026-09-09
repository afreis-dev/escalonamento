#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "scheduler.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "scheduler: numero incorreto de argumentos\n");
        fprintf(stderr, "uso: %s <rate|edf> <arquivo>\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[1], "rate") != 0 && strcmp(argv[1], "edf") != 0) {
        fprintf(stderr, "scheduler: algoritmo invalido '%s' (use rate ou edf)\n", argv[1]);
        return 1;
    }

    TaskSet ts;
    if (read_taskset(argv[2], &ts) != 0) {
        return 1;
    }

    Algorithm alg = strcmp(argv[1], "rate") == 0 ? ALG_RATE : ALG_EDF;

    char out_path[MAX_NAME + 16];
    snprintf(out_path, sizeof(out_path), "%s_%s.out", argv[1], LOGIN);

    int rc = run_simulation(&ts, alg, out_path);
    free_taskset(&ts);

    return rc == 0 ? 0 : 1;
}
