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
