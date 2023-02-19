#include <signal.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "private.h"
#include "uthread.h"

/*
 * Frequency of preemption
 * 100Hz is 100 times per second
 */
#define HZ 100

struct sigaction new_action, old_action;
struct itimerval new_timer, old_timer;
int signum;
sigset_t block_set;

void signal_handler(int signum) {
    // Signal handler calls uthread_yield if SIGVTALRM is raised
    if (signum == SIGVTALRM) {
        uthread_yield();
    }
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
    // Block SIGVTALRM signals if preempt_disable is called
    sigemptyset(&block_set);
    sigaddset(&block_set, SIGVTALRM);
    sigprocmask(SIG_BLOCK, &block_set, NULL);
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
    // Unblocks any previously blocked signals - SIGVTALRM for us
    sigprocmask(SIG_UNBLOCK, &block_set, NULL);
}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
    // Assigns the new action to perform when preempt_start is called
    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    // Sets timer values
    new_timer.it_value.tv_sec = 1/HZ;  // Converts HZ to seconds
    new_timer.it_value.tv_usec = (1000*HZ)%1000000;  // Converts HZ to milliseconds
    new_timer.it_interval = new_timer.it_value;

    if (preempt == true) {
        // Raise errors if sigaction fails, otherwise fire SIGVTALRM that
        // the signal handler will take care of
        if (sigaction(SIGVTALRM, &new_action, &old_action) == -1) {
            perror("sigaction");
        }
        // Set the timer and handle errors
        if (setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer) == -1) {
            perror("setitimer");
        }
    }
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
    // Revert action and timer back to the previous thread that we switched
    // from when preempt start was called
    sigaction(SIGVTALRM, &old_action, &new_action);
    setitimer(ITIMER_VIRTUAL, &old_timer, &new_timer);
}

