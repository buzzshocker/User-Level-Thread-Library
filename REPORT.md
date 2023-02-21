# User Level Thread Library
**Japman Singh Kohli and Lakshitha Anand**

## Phase 0 - Skeleton Code
Phase 0 was straightforward - simply downloading the skeleton code which had the
testers for the code implementations for the later files alongside the starter 
code which detailed what each function for each of the phases had to do. We 
started work on the makefile for libuthread as well. It is in the more 
comprehensive format as described in the project 2 discussion slides. We keep 
building upon this from 1 phase to the other. This makefile would create the 
static library (libuthread.a) along with a clean function to remove all the 
executable files once we are done.

## Phase 1 - queue API
The first phase of the project calls back to data structures and algorithms with
a queue implementation. It is a fairly simple data structure that works on a 
First In First Out (FIFO) basis. Our implementation has two structs - one struct
complementing the other. The first struct `struct node` has two members -

- `void* data`: Data (of any type) that is being inserted in the 
queue
- `struct node* next`: Pointer to the next node

From this struct, it is apparent that it is a linked list based implementation 
of the queue. We went with this since it would facilitate our operations on the 
queue well, with the required time complexities for some functions as per
the design document. To make the code cleaner, we added `typedef struct node* q_
node`. This would make it easier for us to use a pointer to `struct node`.
We also have a helper function `q_node new_node` which helps us to create a new 
node that can be inserted in the linked list - acting as a new item of the 
queue.

The second struct was the struct present in the header file `struct queue`. This
struct had three members - 

- `q_node front`: Pointer to the front of the queue
- `q_node rear`: Pointer to the read of the queue
- `size_t count`: Size of the queue

We have `front` and `rear` to help us keep track of the beginning and the end of
the queue. `count` helps us maintain the count of the elements in the queue, 
facilitating the constant runtime of the `queue_length` function. 

All the functions, as described by the header file, were implemented efficiently
with the help of the two structs, with the runtime complexity constraints being 
adhered to. The logic for each of the functions was pretty clear as defined by 
the header file. All functions work as they should with appropriate error checks 
for each. 

`queue_tester.c` tests each of the functions, with different data types, edge 
cases, and error conditions being tested out for each function. It follows the 
formatting of the `queue_tester_example.c`. 

## Phase 2 - uthread API

/// TBD ///

## Phase 3 - semaphore API
The semaphore API took us some time to understand. The implementation was much 
simpler once we understood what the semaphore is actually supposed to do. 

Firstly, we finish our `uthread` implementation by finally adding the last two 
functions - `uthread_block` and `uthread_unblock`. 

- `uthread_block` is supposed to block the currently running thread. For this, 
we `queue_dequeue` the first ready thread from the `thread_queue`. That dequeued 
thread will now become the current thread. Before that, we store the current 
thread so that we can context switch from that `current_thread` to the earlier
dequeued thread. This blocks the execution of that thread and allows the next 
thread to run. 

- `uthread_unblock` simply allows the thread that was previously blocked to be
put into the `thread_queue` again, allowing it to run again based on its
position in the queue.

Semaphores are essentially a way to control access to resources for multiple 
threads. Sometimes threads have critical functions to perform in critical 
sections. Semaphores help block access to the resources by other threads. This
allows all threads to perform their work without being interfered by other 
functions. 

Our semaphore API has a struct that helps in the implementation of all the 
functions. We use the struct, appropriately titled,  `struct semaphore` to hold 
our member variables - 

- `size_t count`: Number of threads that can be run at the same time
- `queue_t blocked_queue`: Holds the threads that are currently blocked

These two facilitate the working of our API very well. The `count`, is simply 
put, the resources available to the threads that are running concurrently. The
`blocked_queue` just holds the blocked threads.

Whenever one thread starts running, our `sem_down` function will check if there 
are any resources available i.e. `count > 0`. If that is the case, simply reduce
the number of resources and let the thread run. If there are no more resources 
available, we take the current thread and insert it into the `blocked_queue`.
Since there are no resources available, we can't let the thread run. To block it
from running, we call `uthread_block`, which stops its execution.

Our `sem_up` function works in a very similar but basically the opposite way. 
After the error checks, we check if the `blocked_queue` is empty. If it is not, 
then we have resources available at that time. Since we have resources 
available, we remove that thread from the `blocked_queue` and call 
`uthread_unblock` to let that thread to be executed. However, if it is empty 
then we just increment `count` and thus, make more resources available to other 
threads so that they whenever another thread wants to run, it has resources 
available. 

`sem_create` and `sem_destroy` are self-explanatory where the former allocates 
space for the semaphore that is to be created and the latter deallocates all the
space assigned and frees it. 

We test our implementation against the testers provided with the skeleton code. 
While initially facing some errors, we tweaked our implementations to change the 
conditions we check (that also protect against the suggested corner cases) that 
made the testers run properly. We believe that this ensures that our 
implementation works.

## Phase 4 - preemption
Preemption is a way for the Operating system to stop an operation's execution 
and let another process start its execution due to higher priority or perhaps 
the previous operator taking too long and hogging up too many resources 
(for example - an infinite loop that just increments and decrements a value).
Preemption makes sure that all operations get the resources they require. This 
helps certify one of the core OS design principles - **Fairness**. 

We have a couple global variables to help us for our functions -

- `struct sigaction new_action, old_action`: These variables are used for the 
sigaction syscall. The `new_action` stores the values to be stored for the 
action that is to be performed when the alarm is raised. `old_action` will have 
the value that will store that the action that was being performed previously
before `sigaction` was called.
- `struct itimerval new_timer, old_timer`: Similar to the above two variables,
but used for the `setitimer` syscall.
- `signum`: For the value to be passed to the signal handler.
- `sigset_t blocked_set`: `sigset` type set for storing the signals that have 
been blocked. 
- `int preempt_allowed`: int to make sure disable or enable don't run when 
they shouldn't

We also have a signal handler that will be called when `preempt_start` is 
called with the argument `true`. This just calls `uthread_yield` using 
`sigaction` and `setitimer` whenever the`signum == SIGVTALRM`. This allows the 
thread that is currently running to stop execution, allowing another thread to 
run. `new_action` handler gets set to our signal handler, while the other 
members are just empty since our program doesn't require any changes to those.
`new_timer` has all its values set to `1/HZ` for seconds and `(1000*HZ)%1000000` 
for the microseconds being assigned to their respective member variables. 

`preempt_stop` will do the opposite of the above. It will just restore the 
previous action and previous timer that was there before preempt start was 
called. 

Lastly, `disable` and `enable` are very straightforward where `disable` just 
blocks the `SIGVTALRM` signal, while `enable` just unblocks the signal. 
However, if `preempt_allowed == 1`, then don't let either of the two run. 
If it is 1, then that means we are in a critical section and we can't let
another thread interfer with the current libraries and data structures. 
Thus, if it is 1, then don't allow either of these to run.

For `test_preempt.c`, we had a simple tester which runs two threads concurrently
where one of them is an infinite thread and the other is a thread that kills 
both the processes. The preemption works as expected based on our tester.
