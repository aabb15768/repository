#define _GNU_SOURCE // https://codeday.me/bug/20170617/26742.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/syscall.h>

#include "process.h"

// syscall added, refer to mycall/
#define PRINTK 		355
#define GET_TIME 	314
#define MSG_SIZE 300


int proc_assign_cpu ( pid_t pid, int core ) {
	if ( core > sizeof(cpu_set_t) ) {
		fprintf(stderr, "core number too big\n");
		return (-1);
	}
	cpu_set_t mask;
	CPU_ZERO(&mask);
	CPU_SET(core, &mask);
//sched_setaffinity,  sched_getaffinity  -  set  and get a thread's CPU affinity mask
	if ( sched_setaffinity(pid, sizeof(mask), &mask) < 0 ) {
		perror("proc_assign, sched_setaffinity");
		exit (1);
	}
	return (0);
}
int proc_exec ( struct process proc ) {
	pid_t pid=fork(); // returns child process PID
	if ( pid < 0 ) {
		perror("proc_exec, fork");
		return (-1);
	}
	if ( pid == 0 ) { // child process
		UL start_sec, start_nsec, end_sec, end_nsec;
		char dmesg_msg[MSG_SIZE];
        struct timespec spec;
        clock_gettime(CLOCK_REALTIME, &spec);
        start_sec = spec.tv_sec;
        start_nsec = spec.tv_nsec;
		for ( int i=0; i<proc.t_exec; ++i ) UNIT_T();
        clock_gettime(CLOCK_REALTIME, &spec);
        end_sec = spec.tv_sec;
        end_nsec = spec.tv_nsec;
		sprintf(dmesg_msg, "[project1] %d %lu.%09lu %lu.%09lu\n", getpid(), start_sec, start_nsec, end_sec, end_nsec);
		printf("%s\n", dmesg_msg);
		//syscall(PRINTK, dmesg_msg);
		exit(0);
	}
	proc_assign_cpu(pid, CHILD_CPU); // PARENT_CPU != CHILD_CPU
	return (pid);
}
int proc_block ( pid_t pid ) {
	struct sched_param param;
	param.sched_priority=0;
	int res;
	if ( (res=sched_setscheduler(pid, SCHED_IDLE, &param)) < 0 ) {
		perror("proc_block, sched_setscheduler");
		exit (1);
	}
	return (res);
}
int proc_wakeup ( pid_t pid ) {
	struct sched_param param;
	param.sched_priority=0;
	int res;
	if ( (res=sched_setscheduler(pid, SCHED_OTHER, &param)) < 0 ) {
		perror("proc_wakeup, sched_setscheduler");
		exit (1);
	}
	return (res);
}