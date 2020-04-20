#ifndef _SYSCALL_H_
#define _SYSCALL_H_

// no input, 1 return
int GetPid(void);

// no input, 1 return
int GetTime(void);

// 1 input, no return
void Sleep(int);

int SemGet(void);
void SemPost(int);
void SemWait(int);
void MsgSend(int, void*);
void MsgRecv(int, void*);

#endif
