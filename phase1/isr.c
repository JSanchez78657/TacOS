#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"

void CheckSleep() {
    int i, size, pid;
    size = sleep_q.size;
    // For each process in the sleep queue, determine
    for(i = 0; i < size; ++i) {
        pid = dequeue(&sleep_q);
        // if it needs to wake up
        if(pcb[pid].wake_time <= system_time) {
            // If yes, add it to the running process queue and set the state to READY
            enqueue(pid, &run_q);
            pcb[pid].state = READY;
        }
        // If not, add it back to the sleep queue
        else enqueue(pid, &sleep_q);

void NewProcISR() {
    int pid;

    // Check if we have any available (unused) processes
    // If not, "panic" and return
    if(unused_q.size >= MAX_PROC) {
        cons_printf("Kernel Panic: no more process ID left!");
        return;
    }
    // Dequeue the process from the unused queue
    pid = dequeue(&unused_q);
    // Set the process state to READY
    pcb[pid].state = READY;
    // Initialize other process control block variables
    pcb[pid].time = 0;
    pcb[pid].total_time = 0;
    pcb[pid].wake_time = 0;

    // Move the proces into the run queue
    enqueue(pid, &run_q);
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
    if(run_pid == 0) return;
    // If the running PID is invalid, panic and set a breakpoint
    if(run_pid == -1) {
        cons_printf("Kernel Panic: No process running\n");
        breakpoint();
    }

    // Display a message indicating that the process is exiting
    cons_printf("Exiting PID %d\n", run_pid);
    // Change the state of the running process to UNUSED
    pcb[run_pid].state = UNUSED;
    // Queue it to the unused queue
    enqueue(run_pid, &unused_q);
    // clear the running pid
    run_pid = -1;
}

void TimerISR() {
    // Increment the system time
    ++system_time;

    // Check to see if we need to handle sleeping processes
    CheckSleep();
    // If the running PID is invalid, just return
    if(run_pid == -1) return;
    // Increment the running process' current run time
    ++pcb[run_pid].time;
    // Once the running process has exceeded it's scheduled
    // time slice, we need to unschedule it:
    if(pcb[run_pid].time > T_SLICE) {
        //   increase the total run time
        pcb[run_pid].total_time += pcb[run_pid].time;
        //   reset the current running time
        pcb[run_pid].time = 0;
        //   set the state to ready
        pcb[run_pid].state = READY;
        //   queue the process back into the running queue
        enqueue(run_pid, &run_q);
        //   clear the running pid
        run_pid = -1;
    }
}

void GetPidISR() {
    // Don't do anything if the running PID is invalid
    if(run_pid == -1) return;
    // Copy the running pid from the kernel to the
    // eax register via the running process' trapframe
    pcb[run_pid].trapframe_p->eax = run_pid;
}

void GetTimeISR() {
    // Don't do anything if the running PID is invalid
    if(run_pid == -1) return;
    // Copy the system time from the kernel to the
    // eax register via the running process' trapframe
    pcb[run_pid].trapframe_p->eax = system_time;
}

void SleepISR() {
    int wake_time = system_time + pcb[run_pid].trapframe_p->eax;
    if(run_pid == -1) return;
    // Calculate the wake time for the currently running process
    
	  // Add currently running process to the sleep queue
    enqueue(run_pid, &sleep_q);
	  // Change the running process state to SLEEP
    pcb[run_pid].state = SLEEP;
	  // Pull next ready process from the process queue
    /*This might not be right*/run_pid = dequeue(&run_q);
}

