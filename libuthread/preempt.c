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

void signal_handler(int signum) {
    (void)signum;
    signum = SIGVTALRM;
    uthread_yield();
}

void preempt_disable(void)
{
	/* TODO Phase 4 */
}

void preempt_enable(void)
{
	/* TODO Phase 4 */
}

void preempt_start(bool preempt)
{
	/* TODO Phase 4 */
    new_action.sa_handler = signal_handler;
    sigemptyset(&new_action.sa_mask);
    new_action.sa_flags = 0;

    new_timer.it_value.tv_sec = HZ/1000;  // Converts HZ to seconds
    new_timer.it_value.tv_usec = (1000*HZ)%1000000;  // Converts HZ to milliseconds
    new_timer.it_interval = new_timer.it_value;

    if (preempt == true) {
        sigaction(signum, &new_action, &old_action);
        setitimer(ITIMER_VIRTUAL, &new_timer, &old_timer);
    }
}

void preempt_stop(void)
{
	/* TODO Phase 4 */
    sigaction(signum, &old_action, &new_action);
    setitimer(ITIMER_VIRTUAL, &old_timer, &new_timer);
}

