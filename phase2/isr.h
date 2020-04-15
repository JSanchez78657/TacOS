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

#endif
