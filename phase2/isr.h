#ifndef _ISR_H_
#define _ISR_H_

// Kernel Process ISRs
void NewProcISR();
void ProcExitISR();

// Timer ISR
void TimerISR();

// System call ISRs
void GetPidISR();
void GetTimeISR();
void SleepISR();
void SemGetISR();
void SemWaitISR();
void SemPostISR();
void MsgSendISR();
void MsgRecvISR();

// Cross Process Communication ISRs
void SemGetISR();
void SemPostISR();
void SemWaitISR();
void MsgSendISR();
void MsgRecvISR();

#endif
