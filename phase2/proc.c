#include "spede.h"      // for IO_DELAY() needed here below
#include "extern.h"     // for current_run_pid needed here below
#include "proc.h"       // for prototypes of process functions
#include "syscall.h"    // for system calls


// IO_DELAY() should be encapsulated in a loop 1666000 times
// However, IO_DELAY() delays CPU by 0.65us
//   -> Therefore, we should loop 1s/0.65us = 1538461.53846 -> 1538462
#define DELAY_SECOND 1538462


int sharedData;

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
    msg_t writeMsg;
    cons_printf("UserProc Starting\n");
	  while (1) {
		    sys_time = GetTime();
		    writeMsg.time_stamp = sys_time; //should this be filled here or ISR
    		writeMsg.data = sys_time;
     		writeMsg.sender = pid; //should this be filled here or ISR
    		MsgSend(1,&writeMsg);
    		Sleep(sleep_time);
  	}
}

void PrinterProc() {
	int sem = SemGet(); // need the kernel support for sem 0
	int readMem;
	sem = 0; //synchronize sempahores
	cons_printf("PrinterProc Starting\n");
	while (1) {
		SemWait(sem);
		readMem = sharedData;
		cons_printf("Printer Read %d\n", sharedData);
		SemPost(sem);
		Sleep(5);
	}
		
}

void DispatcherProc() {
	int sem = SemGet(); // need the kernel support for sem 0
	msg_t *readMsg;
	sem = 0; //synchronize semaphores
	cons_printf("DispatcherProc Starting\n");
	while (1) {
		MsgRecv(1, readMsg);
		SemWait(sem);
		sharedData = readMsg->data;
		cons_printf("Dispatcher Wrote %d\n",sharedData);
    		SemPost(sem);
	}

}
