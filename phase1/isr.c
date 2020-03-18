#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"

void CheckSleep() {
    // For each process in the sleep queue, determine
    // if it needs to wake up
    // If yes, add it to the running process queue and set the state to READY
    // If not, add it back to the sleep queue
}

void NewProcISR() {
    int pid;

    // Check if we have any available (unused) processes
    // If not, "panic" and return

    // Dequeue the process from the unused queue

    // Set the process state to READY
    // Initialize other process control block variables

    // Move the proces into the run queue

    // Ensure the stack for the process is cleared
    bzero(stack[pid], sizeof(stack[pid]));

    // Allocate the trapframe data
    pcb[pid].trapframe_p = (trapframe_t *)&stack[pid][STACK_SIZE - sizeof(trapframe_t)];

    // If the PID is 0, this is our init process, otherwise it is a user process
    if (pid == 0) {
        pcb[pid].trapframe_p->eip = (unsigned int)InitProc;   // InitProc process
    } else {
        pcb[pid].trapframe_p->eip = (unsigned int)UserProc;   // other processes
    }

    // Set up the trapframe data
    pcb[pid].trapframe_p->eflags = EF_DEFAULT_VALUE|EF_INTR; // set INTR flag
    pcb[pid].trapframe_p->cs = get_cs();                     // standard fair
    pcb[pid].trapframe_p->ds = get_ds();                     // standard fair
    pcb[pid].trapframe_p->es = get_es();                     // standard fair
    pcb[pid].trapframe_p->fs = get_fs();                     // standard fair
    pcb[pid].trapframe_p->gs = get_gs();                     // standard fair
}

void ProcExitISR() {
    // If the running PID is 0 (InitProc) return so we don't ever exit InitProc

    // If the running PID is invalid, panic and set a breakpoint

    // Display a message indicating that the process is exiting

    // Change the state of the running process to UNUSED
    // Queue it to the unused queue
    // clear the running pid
}

void TimerISR() {
    // Increment the system time

    // Check to see if we need to handle sleeping processes

    // If the running PID is invalid, just return

    // Increment the running process' current run time

    // Once the running process has exceeded it's scheduled
    // time slice, we need to unschedule it:
    //   increase the total run time
    //   reset the current running time
    //   set the state to ready
    //   queue the process back into the running queue
    //   clear the running pid
}

void GetPidISR() {
    // Don't do anything if the running PID is invalid

    // Copy the running pid from the kernel to the
    // eax register via the running process' trapframe
}

void GetTimeISR() {
    // Don't do anything if the running PID is invalid

    // Copy the system time from the kernel to the
    // eax register via the running process' trapframe
}

void SleepISR() {
    // Calculate the wake time for the currently running process

	// Add currently running process to the sleep queue

	// Change the running process state to SLEEP

	// Pull next ready process from the process queue
}

