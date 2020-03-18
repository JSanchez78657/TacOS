#ifndef _TOOL_H
#define _TOOL_H

// Needed for q_t
#include "type.h"

void bzero(char *, int);
int dequeue(q_t *);
void enqueue(int, q_t *);

#endif

