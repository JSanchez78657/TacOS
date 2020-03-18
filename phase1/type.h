#ifndef _TYPE_H_
#define _TYPE_H_

#include "trapframe.h"

#define T_SLICE 100          // max timer ticks in 1 run
#define MAX_PROC 20          // max number of processes
#define Q_SIZE MAX_PROC      // queuing capacity; always ensure we have enough for our processes
#define STACK_SIZE 8196      // process runtime stack in bytes

// this is the same as constants defines: UNUSED=0, READY=1, etc.
typedef enum {
    UNUSED,
    READY,
    RUN,
    SLEEP
    // Add additional states
} state_t;

// The process control block for each process
typedef struct {
   state_t state;            // current process state
   int time;                 // run time since loaded
   int total_time;           // total run time since created
   int wake_time;            // time for process to wake up
   trapframe_t *trapframe_p; // points to trapframe of process
} pcb_t;

// Process queue definition
typedef struct {
   int q[Q_SIZE];            // indices into q[] array to place or get element
   int head;                 // queue head
   int tail;                 // queue tail
   int size;                 // queue size
} q_t;

typedef void (*func_ptr_t)(); // void-return function pointer type

#endif _TYPE_H_
