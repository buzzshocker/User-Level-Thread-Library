#include <stddef.h>
#include <stdlib.h>

#include "private.h"
#include "queue.h"
#include "sem.h"

// Struct for semaphore
struct semaphore {
    // Queue to hold the threads that have been blocked
    queue_t blocked_queue;
    // Count member to allocate the semaphore resources
    size_t count;
};

sem_t sem_create(size_t count) {
    // Allocate space for the struct itself
    sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
    // Error checks
    if (sem == NULL) {
        return NULL;
    }
    // Allocate space for the blocked queue
    sem -> blocked_queue = queue_create();
    // Initialise the sem count to reflect the number of threads that
    // we can have the threads running
    sem -> count = count;
    // Return the sem item
    return sem;
}

int sem_destroy(sem_t sem) {
    // Error checks
    if (sem == NULL || queue_length(sem -> blocked_queue) != 0) {
        return -1;
    }
    // Empty the sem count so that the resources are not there
    // and the semaphore does not exist anymore
    sem -> count = 0;
    // Destroy the queue
    queue_destroy(sem -> blocked_queue);
    // Free the space previously allocated
    free(sem);
    return 0;
}

int sem_down(sem_t sem) {
    // Error checks
    if (sem == NULL) {
        return -1;
    }
    // Enable preemption
    preempt_disable();
    // Decrement resources if they are still available
    if (sem -> count > 0) {
        sem -> count--;
    } else {
        // If no resources available
        struct uthread_tcb* eq_thread = uthread_current();
        // Add the current thread to the blocked queue
        queue_enqueue(sem -> blocked_queue, eq_thread);
        // Block the current thread from running
        uthread_block();
    }
    // Disable preemption
    preempt_enable();
    return 0;
}

int sem_up(sem_t sem) {
    // Error checks
    if (sem == NULL) {
        return -1;
    }
    // Variable to hold the node to be dequeued
    struct uthread_tcb* dq_node;
    // Enable preemption
    preempt_disable();
    // If there is something in the blocked queue and resources are available
    if (queue_length(sem -> blocked_queue) != 0) {
        // Dequeue the thread from the blocked queue
        queue_dequeue(sem -> blocked_queue, (void**)&dq_node);
        // unblock the thread and let it run
        uthread_unblock(dq_node);
    } else {
        // If the queue is empty, then make the resources available for other
        // threads to use in the future
        sem -> count++;
    }
    // Disable preemption
    preempt_enable();
    return 0;
}

