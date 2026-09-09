#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "scheduler.h"

static int parse_positive_long(const char *s, long *out) {
    if (s == NULL || *s == '\0') {
        return -1;
    }
    char *end;
    errno = 0;
    long v = strtol(s, &end, 10);
    if (*end != '\0' || errno == ERANGE || v <= 0) {
        return -1;
    }
    *out = v;
    return 0;
}

int read_taskset(const char *path, TaskSet *out) {
    FILE *f = fopen(path, "r");
    if (!f) {
        fprintf(stderr, "scheduler: nao foi possivel abrir o arquivo '%s'\n", path);
        return -1;
    }

    char line[512];
    if (!fgets(line, sizeof(line), f)) {
        fprintf(stderr, "scheduler: arquivo vazio\n");
        fclose(f);
        return -1;
    }
    line[strcspn(line, "\r\n")] = '\0';

    long total_time;
    if (parse_positive_long(line, &total_time) != 0) {
        fprintf(stderr, "scheduler: tempo total invalido: '%s'\n", line);
        fclose(f);
        return -1;
    }

    Task *tasks = NULL;
    int count = 0;
    int cap = 0;
    int line_no = 1;

    while (fgets(line, sizeof(line), f)) {
        line_no++;
        line[strcspn(line, "\r\n")] = '\0';

        char *rest = line;
        while (*rest == ' ' || *rest == '\t') {
            rest++;
        }
        if (*rest == '\0') {
            continue;
        }

        char name[MAX_NAME];
        char period_s[64];
        char deadline_s[64];
        char burst_s[64];
        char extra[8];

        int n = sscanf(line, "%63s %63s %63s %63s %7s",
                        name, period_s, deadline_s, burst_s, extra);
        if (n < 4) {
            fprintf(stderr, "scheduler: linha %d: campo faltando\n", line_no);
            free(tasks);
            fclose(f);
            return -1;
        }
        if (n > 4) {
            fprintf(stderr, "scheduler: linha %d: campos em excesso\n", line_no);
            free(tasks);
            fclose(f);
            return -1;
        }

        long period, deadline, burst;
        if (parse_positive_long(period_s, &period) != 0) {
            fprintf(stderr, "scheduler: linha %d: periodo invalido: '%s'\n", line_no, period_s);
            free(tasks);
            fclose(f);
            return -1;
        }
        if (parse_positive_long(deadline_s, &deadline) != 0) {
            fprintf(stderr, "scheduler: linha %d: deadline invalido: '%s'\n", line_no, deadline_s);
            free(tasks);
            fclose(f);
            return -1;
        }
        if (parse_positive_long(burst_s, &burst) != 0) {
            fprintf(stderr, "scheduler: linha %d: burst invalido: '%s'\n", line_no, burst_s);
            free(tasks);
            fclose(f);
            return -1;
        }
        if (deadline > period || burst > deadline) {
            fprintf(stderr, "scheduler: linha %d: tarefa '%s' viola C <= D <= P (C=%ld D=%ld P=%ld)\n",
                    line_no, name, burst, deadline, period);
            free(tasks);
            fclose(f);
            return -1;
        }

        if (count == cap) {
            cap = cap == 0 ? 8 : cap * 2;
            Task *tmp = realloc(tasks, (size_t)cap * sizeof(Task));
            if (!tmp) {
                fprintf(stderr, "scheduler: falha ao alocar memoria\n");
                free(tasks);
                fclose(f);
                return -1;
            }
            tasks = tmp;
        }

        strncpy(tasks[count].name, name, MAX_NAME - 1);
        tasks[count].name[MAX_NAME - 1] = '\0';
        tasks[count].period = period;
        tasks[count].deadline = deadline;
        tasks[count].burst = burst;
        count++;
    }

    fclose(f);
    out->tasks = tasks;
    out->count = count;
    out->total_time = total_time;
    return 0;
}

void free_taskset(TaskSet *ts) {
    free(ts->tasks);
    ts->tasks = NULL;
    ts->count = 0;
}
