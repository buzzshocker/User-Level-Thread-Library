#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
    // Struct for semaphore
    // Count member to allocate the semaphore resources
    // This is the number of threads that can be run at the same time
    size_t count;
    // Queue to hold the threads that have been blocked
    queue_t blocked_queue;
};

sem_t sem_create(size_t count) {
	/* TODO Phase 3 */
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

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
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

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
    // Error checks
    if (sem == NULL) {
        return -1;
    }
    // If the queue is greater than 0 then take away the resource (decrement)
    // and give it to a thread so that it can run.
    if (sem -> count > 0) {
        sem -> count--;
    } else {  // If there are no resources that can be assigned (count == 0)
        // Take the current thread and insert it into the blocked queue
        // Since there are no resources that are available, we can't let the
        // thread run. As a result it gets blocked and we insert it into
        // the blocked queue. We call uthread_block which actually
        // blocks the thread.
        struct uthread_tcb* eq_thread = uthread_current();
        queue_enqueue(sem -> blocked_queue, eq_thread);
        uthread_block();
    }
    return 0;
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
    // Error checks
    if (sem == NULL) {
        return -1;
    }
    // Create a uthread_tcb* object that will hold the value that is dequeued
    // from the blocked thread
    struct uthread_tcb* dq_node;
    // If there is something in the blocked queue and resources are available
    if (queue_length(sem -> blocked_queue) != 0) {
        // Dequeue from the blocked queue and unblock that thread.
        // Since now we have resources available, we can allow the first thread
        // in the blocked thread. The uthread_unblock unblocks it and allows it
        // run by switching the context to the dq'ed thread
        queue_dequeue(sem -> blocked_queue, (void**)&dq_node);
        uthread_unblock(dq_node);
    } else {
        // Otherwise if the blocked thread is empty, you can make more
        // resources (increment count) available so that the other threads
        // can be run
        sem -> count++;
    }
    return 0;
}

