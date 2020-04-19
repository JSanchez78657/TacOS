#include "spede.h"      // for IO_DELAY() needed here below
#include "extern.h"     // for current_run_pid needed here below
#include "proc.h"       // for prototypes of process functions
#include "syscall.h"    // for system calls


// IO_DELAY() should be encapsulated in a loop 1666000 times
// However, IO_DELAY() delays CPU by 0.65us
//   -> Therefore, we should loop 1s/0.65us = 1538461.53846 -> 1538462
#define DELAY_SECOND 1538462

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
    cons_printf("UserProc started\n");
  	while (1) {
	    	sys_time = GetTime();
    		cons_printf("Proc: User (pid=%d, sys_time=%d, sleep_delay=%d)\n", pid, sys_time, sleep_time);
    		Sleep(sleep_time);
  	}
}
