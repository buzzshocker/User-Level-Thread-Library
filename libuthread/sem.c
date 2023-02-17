#include <stddef.h>
#include <stdlib.h>

#include "queue.h"
#include "sem.h"
#include "private.h"

struct semaphore {
	/* TODO Phase 3 */
    size_t count;
    queue_t blocked_queue;
} semaphore;

sem_t sem_create(size_t count) {
	/* TODO Phase 3 */
    sem_t sem = (sem_t) malloc(sizeof(struct semaphore));
    if (sem == NULL) {
        return NULL;
    }
    sem -> blocked_queue = queue_create();
    sem -> count = count;
    return sem;
}

int sem_destroy(sem_t sem)
{
	/* TODO Phase 3 */
    if (sem == NULL || queue_length(sem -> blocked_queue) != 0) {
        return -1;
    }
    sem -> count = 0;
    queue_destroy(sem -> blocked_queue);
    free(sem);
    return 0;
}

int sem_down(sem_t sem)
{
	/* TODO Phase 3 */
    if (sem == NULL) {
        return -1;
    }
    if (sem -> count > 0) {
        sem -> count--;
    } else {
        struct uthread_tcb* eq_thread = uthread_current();
        queue_enqueue(sem -> blocked_queue, eq_thread);
        uthread_block();
    }
    return 0;
}

int sem_up(sem_t sem)
{
	/* TODO Phase 3 */
    if (sem == NULL) {
        return -1;
    }
    struct uthread_tcb* dq_node;
    if (queue_length(sem -> blocked_queue) != 0) {
        queue_dequeue(sem -> blocked_queue, (void**)&dq_node);
        uthread_unblock(dq_node);
    } else {
        sem -> count++;
    }
    return 0;
}

