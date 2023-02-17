#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "context.c"
#include "private.h"
#include "queue.h"
#include "uthread.h"

queue_t thread_queue;
queue_t exited_thread_queue;
struct uthread_tcb *current_thread;
struct uthread_tcb *next_thread;

struct uthread_tcb
{
    /* TODO Phase 2 */
    int state;
    uthread_ctx_t *uctx;
    void *top_of_stack;
};

struct uthread_tcb *uthread_current(void)
{
    /* TODO Phase 2/3 */
    return current_thread;
}

void uthread_yield(void)
{
    /* TODO Phase 2 */
    queue_enqueue(thread_queue, current_thread);
    queue_dequeue(thread_queue, (void** )&next_thread);
    struct uthread_tcb *temp = current_thread;
    current_thread = next_thread;
    uthread_ctx_switch(temp->uctx, next_thread->uctx);
}

void uthread_exit(void)
{
    /* TODO Phase 2 */
    queue_dequeue(thread_queue, (void** )&next_thread);
    struct uthread_tcb* temp = current_thread;
    current_thread = next_thread;
    queue_enqueue(exited_thread_queue, temp);
    uthread_ctx_switch(temp -> uctx, current_thread -> uctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
    struct uthread_tcb *t1 = (struct uthread_tcb *)malloc
            (sizeof(struct uthread_tcb));
    t1->uctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
    t1->top_of_stack = uthread_ctx_alloc_stack();
    uthread_ctx_init(t1->uctx, t1->top_of_stack, func, arg);
    next_thread = t1;
    queue_enqueue(thread_queue, t1);
    return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
    preempt_start(preempt);
    struct uthread_tcb* main_thread;
    main_thread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    main_thread->uctx = (uthread_ctx_t*) malloc(sizeof(uthread_ctx_t));
    main_thread->top_of_stack = uthread_ctx_alloc_stack();
    thread_queue = queue_create();
    exited_thread_queue = queue_create();
    current_thread = main_thread;
    uthread_create(func, arg);
    while (1)
    {
        uthread_yield();
        /* Breaking out of the loop when the thread_queue is empty */
        if (queue_length(thread_queue) == 0)
        {
            break;
        }
    }
    if (preempt == true) {
        preempt_stop();
    }
    return 0;
}

void uthread_block(void)
{
    /* TODO Phase 3 */
    struct uthread_tcb* dq_thread;
    queue_dequeue(thread_queue, (void** )&dq_thread);
    struct uthread_tcb* curr_thread;
    curr_thread = current_thread;
    current_thread = dq_thread;
    uthread_ctx_switch(curr_thread -> uctx, dq_thread -> uctx);
}


void uthread_unblock(struct uthread_tcb *uthread)
{
    /* TODO Phase 3 */
    queue_enqueue(thread_queue, uthread);
}

