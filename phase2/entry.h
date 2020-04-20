#ifndef _ENTRY_H_
#define _ENTRY_H_

#include <spede/machine/pic.h>

// Interrupt definitions
#define TIMER_INTR    32
#define GETPID_INTR   48
#define GETTIME_INTR  49
#define SLEEP_INTR    50
#define SEMGET_INTR   51
#define SEMPOST_INTR  52
#define SEMWAIT_INTR  53
#define MSGSEND_INTR  54
#define MSGRECV_INTR  55

#define KCODE 0x08         // kernel's code segment
#define KDATA 0x10         // kernel's data segment
#define KSTACK_SIZE 16384  // kernel's stack byte size

// ISR Entries
#ifndef ASSEMBLER

__BEGIN_DECLS
// code defined in entry.S
extern void TimerEntry();     
extern void GetPidEntry();    
extern void GetTimeEntry();   
extern void SleepEntry();     
extern void Loader();
extern void SemGetEntry();
extern void SemPostEntry();
extern void SemWaitEntry();
extern void MsgSendEntry();
extern void MsgRecvEntry();        

__END_DECLS

#endif
#endif
