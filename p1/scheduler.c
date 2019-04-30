#define _GNU_SOURCE // https://codeday.me/bug/20170617/26742.html

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sched.h>
#include <errno.h>
#include <unistd.h>

#include <sys/wait.h>

#include "process.h"
#include "scheduler.h"

// static 變數，當變數有宣告時加上 static 限定時，一但變數生成，它就會一直存在記憶體之中，即使函式執行完畢，變數也不會消失
static int now_who; 		// (-1) = no one
static int now_time; 		// (0) unit
static int finished;		// (0) process
static int last_switch; 	// RR
static int RR_timequant; 	// RR

int cmp ( const void *a, const void *b ) { return (((struct process*) a)->t_ready - ((struct process*) b)->t_ready); }

int nxt_proc ( struct process *proc, int n, int policy ) {
	// non-preemptive
	if ( now_who != -1 && (policy == FIFO || policy == SJF) ) return (now_who);
	int nxt_who=-1;
	if ( policy == FIFO ) {
		for ( int i=0; i<n; ++i ) {
			if ( proc[i].pid == -1 || proc[i].t_exec == 0 ) continue;
			if ( nxt_who == -1 || proc[i].t_ready < proc[nxt_who].t_ready ) nxt_who=i;
		}
	}
	else if ( policy == RR ) {
		if ( now_who == -1 ) {
			for ( int i=0; i<n; ++i ) {
				if ( proc[i].pid != -1 && proc[i].t_exec > 0 ) {
					nxt_who=i;
					break;
				}
			}
		}
		else if ( (now_time-last_switch)%500 == 0 ) {
			nxt_who=(now_who+1)%n;
			while ( proc[nxt_who].pid == -1 || proc[nxt_who].t_exec == 0 ) nxt_who=(now_who+1)%n;
			
		}
		else nxt_who=now_who;
	}
	else if ( policy == SJF ) {
		for ( int i=0; i<n; ++i ) {
			if ( proc[i].pid == -1 || proc[i].t_exec == 0 ) continue;
			if ( nxt_who == -1 || proc[i].t_exec < proc[nxt_who].t_exec ) nxt_who=i;
		}
	}
	else if ( policy == PSJF ) {

	}
	else {
		puts("WTF policy ~ OAO");
		exit(1);
	}
	return (nxt_who);
}

int scheduling ( struct process *proc, int n, int policy ) {
	qsort(proc, n, sizeof(struct process), cmp);
	for ( int i=0; i<n; ++i ) proc[i].pid=-1;
	proc_assign_cpu(getpid(), PARENT_CPU);
	proc_wakeup(getpid());

	now_time=0, now_who=-1, finished=0;
	RR_timequant=500;

	while ( 1 ) {
		if ( now_who != -1 && proc[now_who].t_exec == 0 ) {
			//printf("%d finish at time %d\n", proc[now_who].pid, now_time);
			waitpid(proc[now_who].pid, NULL, 0); // wait finish
			printf("%s %d\n", proc[now_who].name, proc[now_who].pid);
			now_who=-1, ++finished;
			if ( finished == n ) break;
		}
		for ( int i=0; i<n; ++i ) { // check for ready and execution
			if ( proc[i].t_ready == now_time ) {
				proc[i].pid=proc_exec(proc[i]);
				proc_block(proc[i].pid);
			}
		}
		int nxt_who=nxt_proc(proc, n, policy);
		if ( nxt_who != -1 ) {
			if ( nxt_who != now_who ) {
				//printf("time %d: switch from %d to %d\n", now_time, now_who, nxt_who);
				proc_wakeup(proc[nxt_who].pid);
				proc_block(proc[now_who].pid);
				now_who=nxt_who, last_switch=now_time;
			}
		}
		UNIT_T(); ++now_time;
		if ( now_who != -1 ) proc[now_who].t_exec--; // 這裡維護 now_time, 所以也同時要維護 proc[now_who].t_exec
	}
	return (0);
}
