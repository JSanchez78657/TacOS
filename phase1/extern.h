#ifndef _EXTERN_H_
#define _EXTERN_H_

#include "type.h"   // q_t, pcb_t, MAX_PROC, STACK_SIZE

// System time
extern int system_time;

// ID of running process, -1 means not set
extern int run_pid;

// Process queues
extern q_t run_q;
extern q_t unused_q;
extern q_t sleep_q;

// process table
extern pcb_t pcb[MAX_PROC];

// runtime stacks of processes
extern char stack[MAX_PROC][STACK_SIZE];

#endif
