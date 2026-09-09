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

    return 0;
}
