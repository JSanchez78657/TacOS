#ifndef _TOOL_H
#define _TOOL_H

// Needed for q_t
#include "type.h"

void bzero(char *, int);
int dequeue(q_t *);
void enqueue(int, q_t *);
void msg_enqueue(msg_t *, mbox_t *);
msg_t *msg_dequeue(mbox_t *);

#endif

