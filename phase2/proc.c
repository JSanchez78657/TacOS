#include "spede.h"      // for IO_DELAY() needed here below
#include "extern.h"     // for current_run_pid needed here below
#include "proc.h"       // for prototypes of process functions
#include "syscall.h"    // for system calls


// IO_DELAY() should be encapsulated in a loop 1666000 times
// However, IO_DELAY() delays CPU by 0.65us
//   -> Therefore, we should loop 1s/0.65us = 1538461.53846 -> 1538462
#define DELAY_SECOND 1538462


int sharedData = -1;
int printer_dispatch_sem = -1;

void InitProc() {
    int i;
    cons_printf("InitProc started\n");
    while (1) {
        for (i = 0; i < DELAY_SECOND; i++) {
            IO_DELAY();
        }
    }
}


void UserProc() {
    int pid = GetPid();
    int sleep_time = pid % 5 + 1;
    int sys_time;
    int mailbox = 0;
    msg_t writeMsg;
    cons_printf("UserProc Starting\n");
	  while (1) {
		    sys_time = GetTime();
		    writeMsg.time_stamp = sys_time; //should this be filled here or ISR
    		writeMsg.data = sys_time;
     		writeMsg.sender = pid; //should this be filled here or ISR
		    cons_printf("PID %d UserProc sending msg.data = %d to mbox %d at %d\n", pid, writeMsg.data, mailbox, sys_time);
    		MsgSend(mailbox, &writeMsg);
    		Sleep(sleep_time);
  	}
}

void PrinterProc() {
  int pid = GetPid();
  int time;
  if(printer_dispatch_sem == -1) printer_dispatch_sem = SemGet();
	cons_printf("Printer started\n");
	while (1) {
		SemWait(printer_dispatch_sem);
    time = GetTime();
		cons_printf("PID %d Printer read shared memory = %d at %d\n", pid, sharedData, time);
		SemPost(printer_dispatch_sem);
		Sleep(5);
	}
		
}

void DispatcherProc() {
  int pid = GetPid();
  int time;
  int mailbox = 0;
	msg_t readMsg;
  if(printer_dispatch_sem == -1) printer_dispatch_sem = SemGet();
	cons_printf("Dispatcher started\n");
	while (1) {
    MsgRecv(mailbox, &readMsg);
    time = GetTime();
		cons_printf("PID %d Dispatcher recieved msg.data = %d from msg.sender %d in mbox %d at %d\n", pid, readMsg.data, readMsg.sender, mailbox, time);
		SemWait(printer_dispatch_sem);
		sharedData = readMsg.data;
		cons_printf("PID %d Dispatcher wrote shared memory =  %d\n", pid, sharedData);
    SemPost(printer_dispatch_sem);
	}

}
