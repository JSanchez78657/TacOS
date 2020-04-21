#include "spede.h"
#include "type.h"
#include "isr.h"
#include "tool.h"
#include "extern.h"
#include "proc.h"
#include "main.h"
#include "entry.h"

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
    }
}

void NewProcISR() {
    int pid;

    // Check if we have any available (unused) processes
    // If not, "panic" and return
    if(unused_q.size <= 0) {
        cons_printf("Kernel Panic: no more process ID left!");
        return;
    }
    

    // Dequeue & Initialize
    pid = dequeue(&unused_q);
    pcb[pid].state = READY;
    pcb[pid].time = 0;
    pcb[pid].total_time = 0;
    pcb[pid].wake_time = 0;
    enqueue(pid, &run_q);

    // Setup PCB
    bzero(stack[pid], STACK_SIZE);
    pcb[pid].trapframe_p = (trapframe_t *)&(stack[pid][STACK_SIZE - sizeof(trapframe_t)]);

    // pid [0,1,2] = Init, Printer, Dispatcher
    if (pid == 0) {
        pcb[pid].trapframe_p->eip = (unsigned int)InitProc;   
    } else if (pid == 1) {
        pcb[pid].trapframe_p->eip = (unsigned int)PrinterProc; 
    } else if (pid == 2) {
	pcb[pid].trapframe_p->eip = (unsigned int)DispatcherProc;
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
        //breakpoint();
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
    if(run_pid == -1) return;
    // Calculate the wake time for the currently running process
    pcb[run_pid].wake_time = system_time + (pcb[run_pid].trapframe_p->eax * 100);
    pcb[run_pid].trapframe_p->eax = 0;
	  // Add currently running process to the sleep queue
    enqueue(run_pid, &sleep_q);
	  // Change the running process state to SLEEP
    pcb[run_pid].state = SLEEP;
	  // Pull next ready process from the process queue

    /*This might not be right*/    
    run_pid = -1;
}

void SemGetISR() {
  // Dequeue a semaphore from the semaphore queue
  int sem_id = dequeue(&semaphore_q);
  semaphore_t sem;
  // If the semaphore is valid, ensure that the semaphore data is initialized
  if(sem_id != -1) {
    sem = semaphore[sem_id];
    sem.count = 0;
    while(dequeue(&sem.wait_q) != -1);
  }
  // Return the semaphore ID to the caller
  pcb[run_pid].trapframe_p->eax = sem_id;
}

void SemWaitISR() {
  semaphore_t sem;
  if(run_pid == -1) return;
  sem = semaphore[pcb[run_pid].trapframe_p->eax];
  // Enqueue a process to the semaphore's wait queue if the semaphore is held
  if(sem.count > 0) {
    enqueue(run_pid, &sem.wait_q);
    pcb[run_pid].state = WAITING;
    run_pid = -1;
  }
  // Increment the semaphore access count
  ++sem.count;
  semaphore[pcb[run_pid].trapframe_p->eax] = sem;
}

void SemPostISR() {
  semaphore_t sem;
  int proc_id;
  int sem_id;
  if(run_pid == -1) return;
  sem_id = pcb[run_pid].trapframe_p->eax;
  sem = semaphore[sem_id];
  // If a process is in the semaphore's wait queue, enqueue it back to the running queue
  if(sem.wait_q.size > 0) {
    proc_id = dequeue(&sem.wait_q);
    pcb[proc_id].state = READY;
    enqueue(proc_id, &run_q);
  }
  // Decrement the semaphore access count (hint: what happens if the count < 0?)
  --sem.count;
  semaphore[pcb[run_pid].trapframe_p->eax] = sem;
  if(sem.count < 0) enqueue(sem_id, &semaphore_q);
}

void MsgSendISR() {
  mbox_t mailbox;
  msg_t* message;
  int proc_id;
  if(run_pid == -1) return;
  mailbox = mbox[pcb[run_pid].trapframe_p->eax];
  message = (msg_t*) pcb[run_pid].trapframe_p->ebx;
  // If a process is waiting:
  if(mailbox.wait_q.size > 0) {
    // a. Dequeue it, move it to the running queue
    proc_id = dequeue(&mailbox.wait_q);
    pcb[proc_id].state = READY;
    enqueue(proc_id, &run_q);
    // b. Update the message pointer so the process in MsgRecvISR() can process it
    pcb[proc_id].trapframe_p->ebx = (unsigned int) message;
  }
  // Enqueue the message to the queue if no process is waiting
  else msg_enqueue(message, &mailbox);

  mbox[pcb[run_pid].trapframe_p->eax] = mailbox;
}

void MsgRecvISR() {
  mbox_t *mailbox;
  msg_t* message;
  if(run_pid == -1) return;
  mailbox = &mbox[pcb[run_pid].trapframe_p->eax];
  // Dequeue a message from the message queue if one exists and return it to the user
  message = msg_dequeue(mailbox);
 

  if(message != NULL) pcb[run_pid].trapframe_p->ebx = (unsigned int) message;
  // If there is no message in the queue, move the process to the wait queue
  else {
    pcb[run_pid].state = WAITING;
    enqueue(run_pid, &(mailbox->wait_q));
    run_pid = -1;
  }

  
}
