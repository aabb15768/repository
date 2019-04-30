#ifndef _PROCESS_H_ // 避免循環引用
#define _PROCESS_H_

#include <sys/types.h>

typedef unsigned long UL;

#define PARENT_CPU 0
#define CHILD_CPU 1

#define UNIT_T() \
{ \
volatile UL i; \
for ( i=0; i<1000000UL; i++ ); \
} \

struct process {
	char name[256];
	int t_ready;
	int t_exec;
	pid_t pid; // initial: -1
};

int proc_assign_cpu ( pid_t pid, int core );
int proc_exec ( struct process proc );
int proc_block ( pid_t pid );
int proc_wakeup ( pid_t pid );

#endif