#include "syscall.h"

int GetPid() {                    // no input, has return
    int x;

    asm("int $48; movl %%eax, %0"
        : "=g" (x)                // output from asm("...")
        :                         // no input into asm("...")
        : "eax");                // push before asm("..."), pop after

    return x;
}

int GetTime() {                   // no input, has return
    int time;

    asm("int $49; movl %%eax, %0"
        : "=g" (time)
        :
        : "eax"
    );

    return time;
}

void Sleep(int sleep_seconds) {   // has input, no return
    asm("movl %0, %%eax; int $50"
        :                         // no output from asm("...")
        : "g" (sleep_seconds)     // input into asm("...")
        : "eax");                // push before asm("..."), pop after
}

int SemGet(void) {		 // no input, has return
    int semID;

    asm("int 51; movl %%eax, %0"
	: "=g" (semID)
	:
	: "eax"
    );

    return semID;
}

void SemWait(int semID) { // has input, no return

    asm("movl %0, %%eax; int $52"
        :                         
        : "g" (semID)     
        : "eax");               

}

void SemPost(int semID) { // has input, no return

    asm("movl %0, %%eax; int $53"
        :                         
        : "g" (semID)     
        : "eax");               

}

void MsgSend(int mbox, msg_t *msg) { // has input, no return

    asm("movl %0, %%eax; movl %1, %%ebx; int $54"
        :                         
        : "g" (mbox), "g" (msg)     
        : "eax, ebx");               

}

void MsgRecv(int mbox, msg_t *msg) { // has input, no return

    asm("movl %0, %%eax; movl %1, %%ebx; int $55"
        :                         
        : "g" (mbox), "g" (msg)     
        : "eax, ebx");               

}


