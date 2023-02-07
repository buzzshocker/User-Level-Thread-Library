#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else	{							\
		printf("FAIL\n");				\
		exit(1);						\
	}									\
} while(0)

/* Create */
void test_create(void)
{
	fprintf(stderr, "*** TEST create ***\n");

	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void)
{
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "*** TEST queue_simple ***\n");

	q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

void test_queue_destroy_fail(void) {

    int data = 3;
    queue_t q;
    fprintf(stderr, "*** TEST queue_destroy_fail ***\n");
    q = queue_create();
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_destroy(q) == -1);
}

void test_queue_dequeue_int(void) {
    int data = 6, *ptr;
    queue_t q = queue_create();
    fprintf(stderr, "*** TEST queue_dequeue_int ***\n");
    queue_enqueue(q, &data);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(ptr == &data);
}

void test_queue_dequeue_char(void) {
    char data = 'x', *ptr;
    queue_t q = queue_create();
    fprintf(stderr, "*** TEST queue_dequeue_char ***\n");
    queue_enqueue(q, &data);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(ptr == &data);
}

void test
int main(void)
{
	test_create();
	test_queue_simple();
    test_queue_destroy_fail();
    test_queue_dequeue_int();
    test_queue_dequeue_char();
	return 0;
}
