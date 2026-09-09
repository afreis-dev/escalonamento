#include <stdio.h>
#include <stdlib.h>
#include "scheduler.h"

typedef struct {
    long remaining;
    long deadline_abs;
} RuntimeState;

static int higher_priority(const TaskSet *ts, Algorithm alg, const RuntimeState *rt, int a, int b) {
    if (alg == ALG_RATE) {
        if (ts->tasks[a].period != ts->tasks[b].period) {
            return ts->tasks[a].period < ts->tasks[b].period;
        }
    } else {
        if (rt[a].deadline_abs != rt[b].deadline_abs) {
            return rt[a].deadline_abs < rt[b].deadline_abs;
        }
    }
    return a < b;
}

int run_simulation(const TaskSet *ts, Algorithm alg, const char *out_path) {
    int n = ts->count;
    long total = ts->total_time;

    RuntimeState *rt = calloc((size_t)n, sizeof(RuntimeState));
    long *lost = calloc((size_t)n, sizeof(long));
    long *completed = calloc((size_t)n, sizeof(long));
    long *killed = calloc((size_t)n, sizeof(long));
    int *owner = malloc((size_t)total * sizeof(int));
    char *reason = calloc((size_t)total + 1, sizeof(char));

    if ((n > 0 && (!rt || !lost || !completed || !killed)) || !owner || !reason) {
        fprintf(stderr, "scheduler: falha ao alocar memoria\n");
        free(rt);
        free(lost);
        free(completed);
        free(killed);
        free(owner);
        free(reason);
        return -1;
    }

    for (long t = 0; t < total; t++) {
        for (int i = 0; i < n; i++) {
            if (rt[i].remaining > 0 && rt[i].deadline_abs == t) {
                lost[i]++;
                if (t > 0 && owner[t - 1] == i) {
                    reason[t] = 'L';
                }
                rt[i].remaining = 0;
            }
        }

        for (int i = 0; i < n; i++) {
            if (t % ts->tasks[i].period == 0) {
                rt[i].remaining = ts->tasks[i].burst;
                rt[i].deadline_abs = t + ts->tasks[i].deadline;
            }
        }

        int chosen = -1;
        for (int i = 0; i < n; i++) {
            if (rt[i].remaining <= 0) {
                continue;
            }
            if (chosen == -1 || higher_priority(ts, alg, rt, i, chosen)) {
                chosen = i;
            }
        }

        owner[t] = chosen;
        if (chosen != -1) {
            rt[chosen].remaining--;
            if (rt[chosen].remaining == 0) {
                completed[chosen]++;
                reason[t + 1] = 'F';
            }
        }
    }

    for (int i = 0; i < n; i++) {
        if (rt[i].remaining > 0) {
            killed[i] = 1;
        }
    }

    FILE *out = fopen(out_path, "w");
    if (!out) {
        fprintf(stderr, "scheduler: nao foi possivel criar o arquivo de saida '%s'\n", out_path);
        free(rt);
        free(lost);
        free(completed);
        free(killed);
        free(owner);
        free(reason);
        return -1;
    }

    fprintf(out, "EXECUTION BY %s\n\n", alg == ALG_RATE ? "RATE" : "EDF");

    long t = 0;
    while (t < total) {
        int who = owner[t];
        long start = t;
        while (t < total && owner[t] == who) {
            t++;
        }
        long len = t - start;
        if (who == -1) {
            fprintf(out, "idle for %ld units\n", len);
        } else {
            char suffix;
            if (reason[t] == 'F') {
                suffix = 'F';
            } else if (reason[t] == 'L') {
                suffix = 'L';
            } else if (t == total) {
                suffix = 'K';
            } else {
                suffix = 'H';
            }
            fprintf(out, "[%s] for %ld units - %c\n", ts->tasks[who].name, len, suffix);
        }
    }

    fprintf(out, "\nLOST DEADLINES\n");
    for (int i = 0; i < n; i++) {
        fprintf(out, "[%s] %ld\n", ts->tasks[i].name, lost[i]);
    }

    fprintf(out, "\nCOMPLETE EXECUTION\n");
    for (int i = 0; i < n; i++) {
        fprintf(out, "[%s] %ld\n", ts->tasks[i].name, completed[i]);
    }

    fprintf(out, "\nKILLED\n");
    for (int i = 0; i < n; i++) {
        fprintf(out, "[%s] %ld\n", ts->tasks[i].name, killed[i]);
    }

    fclose(out);
    free(rt);
    free(lost);
    free(completed);
    free(killed);
    free(owner);
    free(reason);
    return 0;
}
