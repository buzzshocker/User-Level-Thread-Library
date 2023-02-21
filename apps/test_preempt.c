#include <private.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>
#include <signal.h>

int inf_val = 0;

void exit_thread(void* arg) {
    (void)arg;
    printf("inf_val = %d\n", inf_val);
    printf("Now we have entered the thread that will raise SIGINT and "
           "end this execution. Bye!\n");
    raise(SIGINT);
}

void preempt_thread(void* arg) {
    (void)arg;
    printf("The thread that will run infinitely starts.\n");
    uthread_create(exit_thread, arg);
    printf("Now we start the infinite loop that increments and decrements "
           "a global value - inf_value.\n");
    while(inf_val >= 0) {
        inf_val++;
        inf_val--;
    }
}

int main() {
    printf("Calling uthread run with preempt = true.\n");
    uthread_run(true, preempt_thread, NULL);
    return 0;
}
