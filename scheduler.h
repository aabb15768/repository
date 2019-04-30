#ifndef _SCHEDULER_H_ // 避免循環引用
#define _SCHEDULER_H_

#include "process.h"

// scheduler
#define FIFO 	0
#define RR		1
#define SJF		2
#define PSJF	3

int scheduling ( struct process *proc, int n, int policy );

#endif