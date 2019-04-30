#define _GNU_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

#include "process.h"
#include "scheduler.h"

int main ( int argc, char **argv ) 
{
	char sched[100]; scanf("%s", sched);
	int n; scanf("%d", &n);
	struct process *proc=(struct process*) malloc ( n*sizeof(struct process) );
	for ( int i=0; i<n; ++i ) 
		scanf("%s%d%d", proc[i].name, &proc[i].t_ready, &proc[i].t_exec);
	int policy;
	if ( strcmp(sched, "FIFO")==0 ) policy=FIFO;
	else if ( strcmp(sched, "RR")==0 ) policy=RR;
	else if ( strcmp(sched, "SJF")==0 ) policy=SJF;
	else if ( strcmp(sched, "PSJF")==0 ) policy=PSJF;
	else {
		puts("WTF policy ~ OAO");
		exit (1);
	}
	scheduling(proc, n, policy);
	exit (0);
}