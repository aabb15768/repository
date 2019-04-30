#ifndef _PROCESS_H_
#define _PROCESS_H_

#include <sys/types.h>
#define CHILD_CPU 1
#define PARENT_CPU 0
#define UNIT_T() {   
volatile unsigned long i;        
for (i = 0; i < 1000000UL; i++);    
}                        

struct process {
    char name[32];
    int t_ready;
    int t_exec;
    pid_t pid;
};

// assign cpu to process
int proc_assign_cpu(int pid, int core);

// execute process specified
int proc_exec(struct process proc);

// set low priority to process pid
int proc_block(int pid);

// wake up process
int proc_wakeup(int pid);

#endif
