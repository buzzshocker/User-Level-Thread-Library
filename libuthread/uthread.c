#include <assert.h>
#include <signal.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "queue.h"
#include "uthread.h"

/* queue to hold all the running and yet to run thread objects */
queue_t thread_queue;
/* queue to hold all the thread objects that have
    ceased from the running thread */
queue_t exited_thread_queue;
/* TCB of the current thread */
struct uthread_tcb* current_thread;
/* TCB of the next thread */
struct uthread_tcb* next_thread;

struct uthread_tcb
{
    /* TODO Phase 2 */
    // /* keeps track of the whether the process
    //     is running, blocked or ready*/
    uthread_ctx_t* uctx;
    void* top_of_stack;
};

struct uthread_tcb *uthread_current(void)
{
    /* TODO Phase 2/3 */
    return current_thread;
}

void uthread_yield(void)
{
    /* TODO Phase 2 */
    /* push the currently running thread into the queue */
    queue_enqueue(thread_queue, current_thread);
    /* then pop the next thread from the queue for context switching */
    queue_dequeue(thread_queue, (void **)&next_thread);
    /* creating a temporary TCB that has a copy of the current thread */
    struct uthread_tcb *temp = current_thread;
    /* changing the current thread to be the next thread for the subsequent processes */
    current_thread = next_thread;
    /* context switch between currently running thread and the next thread */
    uthread_ctx_switch(temp->uctx, next_thread->uctx);
}

void uthread_exit(void)
{
    /* TODO Phase 2 */
    /* pop the next thread from the thread_queue */
    queue_dequeue(thread_queue, (void **)&next_thread);
    /* creating a temporary TCB that has a copy of the
    current thread */
    struct uthread_tcb *temp = current_thread;
    /* next thread will become the running thread */
    current_thread = next_thread;
    /* enqueue the currently running thread into the
    exited_thread_queue to stop thread execution */
    queue_enqueue(exited_thread_queue, temp);
    /* context switch between currently running thread
    and the next thread */
    uthread_ctx_switch(temp->uctx, current_thread->uctx);
}

int uthread_create(uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
    /* create a new thread t1 and allocate space */
    struct uthread_tcb *t1 = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    /* allocate space for t1's thread context */
    t1->uctx = (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));
    /* allocate the stack segment for t1 */
    t1->top_of_stack = uthread_ctx_alloc_stack();
    /* initialize t1's execution context */
    uthread_ctx_init(t1->uctx, t1->top_of_stack, func, arg);
    /* making t1 to be the next_thread and enqueuing it
    into the thread queue*/
    next_thread = t1;
    queue_enqueue(thread_queue, t1);
    return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg)
{
    /* TODO Phase 2 */
    preempt_start(preempt);
    /* declaring the first TCB obj -> main_thread */
    struct uthread_tcb *main_thread;
    /* allocating space for the main_thread */
    main_thread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    /* allocate space for main_thread's thread context */
    main_thread->uctx = (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));
    /* allocate the stack segment for main_thread */
    main_thread->top_of_stack = uthread_ctx_alloc_stack();
    /* creating and allocating space for thread_queue and
    exited_thread_queue */
    thread_queue = queue_create();
    exited_thread_queue = queue_create();
    /* main_thread is the currently running thread */
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
    if (preempt == true)
    {
        preempt_stop();
    }
    return 0;
}

void uthread_block(void)
{
    /* TODO Phase 3 */
    struct uthread_tcb *dq_thread;
    /* dequeue the first ready thread from thread_queue */
    queue_dequeue(thread_queue, (void **)&dq_thread);
    /* store the current thread in a temp TCB obj */
    struct uthread_tcb *curr_thread;
    curr_thread = current_thread;
    /* dequeued thread will now become the current thread */
    current_thread = dq_thread;
    /* context switch from that current_thread to the dequeued thread */
    uthread_ctx_switch(curr_thread->uctx, dq_thread->uctx);
}

void uthread_unblock(struct uthread_tcb *uthread)
{
    /* TODO Phase 3 */
    /* unblocking the thread that was previously blocked by
    enqueuing into the thread_queue*/
    queue_enqueue(thread_queue, uthread);
}
