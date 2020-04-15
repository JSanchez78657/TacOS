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

void msg_enqueue(msg_t *msg_p, mbox_t *mbox_p) {
    // Can't enqueue if the mailbox is full
    if (mbox_p->size == MBOX_SIZE) {
        cons_printf("Mailbox is full\n");
        return;
    }

    // Add the message to the mailbox queue
    mbox_p->msg[mbox_p->tail] = *msg_p;

    // Advance the tail to the next mailbox entry
    mbox_p->tail++;

    // If the end of the mailbox queue is reached, reset ail to the start
    if (mbox_p->tail == MBOX_SIZE) {
        mbox_p->tail = 0;
    }

    // Increment the size
    mbox_p->size++;
}

msg_t *msg_dequeue(mbox_t *mbox_p) {
    msg_t *msg_p;

    if (mbox_p->size == 0) {
        // mailbox is empty
        return NULL;
    }

    // Get the first message in the mailbox queue
    msg_p = &mbox_p->msg[mbox_p->head];
    mbox_p->head++;

    // If the end of the mailbox queue is reached, reset head to the start
    if (mbox_p->head == MBOX_SIZE) {
        mbox_p->head = 0;
    }

    // Decrement queue size since we pulled an item off
    mbox_p->size--;

    return msg_p;
}
