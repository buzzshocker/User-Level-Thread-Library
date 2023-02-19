//
// Created by japma on 2/17/2023.
//

#include <private.h>
#include <stdio.h>
#include <stdlib.h>
#include <uthread.h>

void exit_thread(void* arg) {
    (void)arg;
    printf("Now we have entered the thread that will exit and "
           "end this execution. Bye!\n");
    exit(0);
}

void preempt_thread(void* arg) {
    (void)arg;
    printf("The thread that will run infinitely starts.\n");
    uthread_create(exit_thread, arg);
    printf("Now we start the infinite loop.\n");
    int inf_val;
    while(1) {
        inf_val++;
    }
}

int main() {
    printf("Calling uthread run with preempt = true.\n");
    uthread_run(true, preempt_thread, NULL);
    return 0;
}
