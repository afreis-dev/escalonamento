#ifndef SCHEDULER_H
#define SCHEDULER_H

#define MAX_NAME 64
#define LOGIN "afsr"

typedef struct {
    char name[MAX_NAME];
    long period;
    long deadline;
    long burst;
} Task;

typedef struct {
    Task *tasks;
    int count;
    long total_time;
} TaskSet;

typedef enum {
    ALG_RATE,
    ALG_EDF
} Algorithm;

int read_taskset(const char *path, TaskSet *out);
void free_taskset(TaskSet *ts);
int run_simulation(const TaskSet *ts, Algorithm alg, const char *out_path);

#endif
