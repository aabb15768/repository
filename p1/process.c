#define _GNU_SOURCE
#include "process.h"
#include <sched.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/syscall.h>
#include <inttypes.h>w
#include <math.h>
#include <time.h>

// assign cpu to process
int proc_assign_cpu(int pid, int cpu_num) {
    // add cpu_num cpu to cpu set
    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(cpu_num, &mask);
    // assign process with pid to cpu_num cpu
    sched_setaffinity(pid, sizeof(mask), &mask);
    return 0;
}

// execute process specified
int proc_exec(struct process proc) {
    int pid = fork();

    // fork fail
    if (pid < 0) {
        perror("fork fail");
        return -1;
    }

    // child process
    if (pid == 0) {
        unsigned long start_sec, start_nsec, end_sec, end_nsec;
        char dmesg_msg[MSG_SIZE];
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        start_sec = spec.tv_sec;
        start_nsec = spec.tv_nsec;

        for (int i = 0; i < proc.t_exec; i++) {
            UNIT_T();
        }
        clock_gettime(CLOCK_REALTIME, &spec);
        end_sec = spec.tv_sec;
        end_nsec = spec.tv_nsec;
        sprintf(to_dmesg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
        FILE *f = fopen("/dev/kmsg", "a");
        fprintf(f, "%s", dmesg_msg);
        fclose(f);
        exit(0);
    }

    proc_assign_cpu(pid, CHILD_CPU);

    return pid;
}

// set low priority to process pid
int proc_block(int pid) {
    struct sched_param param;
    param.sched_priority = 0;
    int ret = sched_setscheduler(pid, SCHED_IDLE, &param);

    return ret;
}

// wake up
int proc_wakeup(int pid) {
    struct sched_param param;
    param.sched_priority = 0;
    int ret = sched_setscheduler(pid, SCHED_OTHER, &param);

    return ret;
}
