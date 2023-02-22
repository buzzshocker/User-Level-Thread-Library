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

// Queue to hold all the running and yet to run thread objects
queue_t thread_queue;
// Queue to hold all the thread objects that have ceased from the running thread
queue_t exited_thread_queue;
// TCB of the current thread
struct uthread_tcb* current_thread;
// TCB of the next thread
struct uthread_tcb* next_thread;

struct uthread_tcb {
    // Keeps track of the whether the process is running, blocked or ready
    uthread_ctx_t* uctx;
    void* top_of_stack;
};

struct uthread_tcb *uthread_current(void) {
    return current_thread;
}

void uthread_yield(void) {
    // Disable preemption while entering critical section
    preempt_disable();
    // Push the currently running thread into the queue
    queue_enqueue(thread_queue, current_thread);
    // Then pop the next thread from the queue for context switching
    queue_dequeue(thread_queue, (void **)&next_thread);
    // Creating a temporary TCB that has a copy of the current thread
    struct uthread_tcb *curr_node = current_thread;
    // Changing the current thread to be the next thread for the subsequent
    // processes
    current_thread = next_thread;
    // Context switch between currently running thread and the next thread
    uthread_ctx_switch(curr_node->uctx, next_thread->uctx);
    // Enable preemption while exiting the critical section
    preempt_enable();
}

void uthread_exit(void) {
    // Disable preemption while entering critical section
    preempt_disable();
    // Pop the next thread from the thread_queue
    queue_dequeue(thread_queue, (void **)&next_thread);
    // Creating a temporary TCB that has a copy of the current thread
    struct uthread_tcb *dq_node = current_thread;
    // Next thread will become the running thread
    current_thread = next_thread;
    // Enqueue the currently running thread into the exited_thread_queue to stop
    // thread execution
    queue_enqueue(exited_thread_queue, dq_node);
    // Context switch between currently running thread and the next thread
    uthread_ctx_switch(dq_node->uctx, current_thread->uctx);
    // Enable preemption while exiting the critical section
    preempt_enable();
}

int uthread_create(uthread_func_t func, void *arg) {
    // Disable preemption as we enter critical section
    preempt_disable();
    // Create a new thread t1 and allocate space
    struct uthread_tcb *t1 = (struct uthread_tcb *)malloc
            (sizeof(struct uthread_tcb));
    // Allocate space for t1's thread context
    t1->uctx = (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));
    // Allocate the stack segment for t1
    t1->top_of_stack = uthread_ctx_alloc_stack();
    // Initialize t1's execution context
    uthread_ctx_init(t1->uctx, t1->top_of_stack, func, arg);
    // Make t1 the next_thread and enqueue it into the thread queue
    next_thread = t1;
    queue_enqueue(thread_queue, t1);
    // Enable preemption as we are done with critical section
    preempt_enable();
    return 0;
}

int uthread_run(bool preempt, uthread_func_t func, void *arg) {
    // Start preemption
    preempt_start(preempt);
    // Declaring the first TCB obj -> main_thread
    struct uthread_tcb *main_thread;
    // Disable preemption as we enter critical section
    preempt_disable();
    // Allocating space for the main_thread
    main_thread = (struct uthread_tcb *)malloc(sizeof(struct uthread_tcb));
    // Allocate space for main_thread's thread context
    main_thread->uctx = (uthread_ctx_t *)malloc(sizeof(uthread_ctx_t));
    // Allocate the stack segment for main_thread
    main_thread->top_of_stack = uthread_ctx_alloc_stack();
    // Creating and allocating space for thread_queue and exited_thread_queue
    thread_queue = queue_create();
    exited_thread_queue = queue_create();
    // Enable preemption as exit critical section
    preempt_enable();
    // Main_thread is the currently running thread
    current_thread = main_thread;
    uthread_create(func, arg);
    while (1) {
        uthread_yield();
        // Breaking out of the loop when the thread_queue is empty
        if (queue_length(thread_queue) == 0) {
            break;
        }
    }
    // Stop preemption if condition passes
    if (preempt == true) {
        preempt_stop();
    }
    return 0;
}

void uthread_block(void) {
    // Disable preemption as we enter critical section
    preempt_disable();
    struct uthread_tcb *dq_thread;
    // Dequeue the first ready thread from thread_queue
    queue_dequeue(thread_queue, (void **)&dq_thread);
    // Store the current thread in a temp TCB obj
    struct uthread_tcb *curr_thread;
    curr_thread = current_thread;
    // Dequeued thread will now become the current thread
    current_thread = dq_thread;
    // Context switch from that current_thread to the dequeued thread
    uthread_ctx_switch(curr_thread->uctx, dq_thread->uctx);
    // Enable preemption as we are done with critical section
    preempt_enable();
}

void uthread_unblock(struct uthread_tcb *uthread) {
    // Disable preemption as we enter critical section
    preempt_disable();
    // Unblocking the thread that was previously blocked by enqueuing into the
    // thread_queue
    queue_enqueue(thread_queue, uthread);
    // Enable preemption as we are done with critical section
    preempt_enable();
}
