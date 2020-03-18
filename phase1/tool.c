#include "spede.h"
#include "type.h"
#include "extern.h"
#include "tool.h"

void bzero(char *p, int size) {
    while (size-- > 0) {
        *p++ = 0;
    }
}

void enqueue(int pid, q_t *p) {
    // Returns and display error if queue is full
    if (p->size == Q_SIZE) {
        cons_printf("Error: Queue is full\n");
        return;
    }

    // Add the PID to the tail of the queue
    p->q[p->tail] = pid;

    // Move the tail forward
    p->tail++;

    // If we are at the end of the array, move the tail to the beginning
    if (p->tail == Q_SIZE) {
        p->tail = 0;
    }

    // Increment size (since we just added an item to the queue)
    p->size++;
}

int dequeue(q_t *p) {
    int pid;

    // return -1 if queue is empty
    if (p->size == 0) {
        return -1;
    }

    // Get the PID from the head of the queue
    pid = p->q[p->head];

    // Move the head forward
    p->head++;

    // If we are at the end of the array, move the head to the beginning
    if (p->head == Q_SIZE) {
        p->head = 0;
    }

    // Decrement size (since we just removed an item from the queue)
    p->size--;

    return pid;
}
