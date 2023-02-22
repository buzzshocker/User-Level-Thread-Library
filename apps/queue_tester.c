#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "../libuthread/queue.h"

#define TEST_ASSERT(assert)				\
do {									\
	printf("ASSERT: " #assert " ... ");	\
	if (assert) {						\
		printf("PASS\n");				\
	} else {							\
		printf("FAIL\n");				\
		exit(1);						\
	}                                   \
} while (0)

/* Create */
void test_create(void) {
	fprintf(stderr, "*** TEST create ***\n");
	TEST_ASSERT(queue_create() != NULL);
}

/* Enqueue/Dequeue simple */
void test_queue_simple(void) {
	int data = 3, *ptr;
	queue_t q;

	fprintf(stderr, "\n*** TEST queue_simple ***\n");

    q = queue_create();
	queue_enqueue(q, &data);
	queue_dequeue(q, (void**)&ptr);
	TEST_ASSERT(ptr == &data);
}

// Tests the error cases of queue_destroy
void test_queue_destroy_fail(void) {
    int data = 3;
    queue_t q;

    fprintf(stderr, "\n*** TEST queue_destroy_fail ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    TEST_ASSERT(queue_destroy(q) == -1);
}

// Tests the regular case of queue destroy
void test_queue_destroy_pass(void) {
    char data = 'x', *ptr;
    queue_t q;

    fprintf(stderr, "\n*** TEST queue_destroy_pass ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(queue_destroy(q) == 0);
}

// Tests the error case of enqueue
void test_queue_enqueue_error(void) {
    queue_t q = queue_create();

    fprintf(stderr, "\n*** TEST queue_enqueue_error ***\n");

    TEST_ASSERT(queue_enqueue(q, NULL) == -1);
    TEST_ASSERT(queue_destroy(q) == 0);
    TEST_ASSERT(queue_length(q) == 0);
}

// Test dequeue with int
void test_queue_dequeue_int(void) {
    int data = 6, *ptr;
    queue_t q = queue_create();

    fprintf(stderr, "\n*** TEST queue_dequeue_int ***\n");

    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data);
}

// Test dequeue with char
void test_queue_dequeue_char(void) {
    char data = 'x', *ptr;
    queue_t q = queue_create();

    fprintf(stderr, "\n*** TEST queue_dequeue_char ***\n");

    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data);
}

// Test dequeue with string
void test_queue_dequeue_string(void) {
    char* data = "hello there", **ptr;
    queue_t q = queue_create();

    fprintf(stderr, "\n*** TEST queue_dequeue_string ***\n");

    queue_enqueue(q, &data);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data);
}

// Helper for queue_iterate
static void iterator_inc_int(queue_t q, void *data) {
    int *a = (int*)data;

    if (*a == 21)
        queue_delete(q, data);
    else
        *a *= 2;
}

// Test for iterator with int
void test_iterator_int(void) {
    queue_t q;
    int data[] = {3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33};
    size_t i;
    int* ptr;

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    fprintf(stderr, "\n*** TEST test_iterator_int ***\n");

    queue_iterate(q, iterator_inc_int);
    TEST_ASSERT(data[3] == 24);
    TEST_ASSERT(queue_length(q) == 10);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data[0]);
}

// Helper for queue_iterate
static void iterator_inc_char(queue_t q, void *data) {
    char *a = (char*)data;

    if (*a == 'f') {
        queue_length(q);
        return;
    } else {
        *a += 2;
    }
}

// Test for iterator with int
void test_iterator_char(void) {
    queue_t q;
    char data[] = {'a', 'm', 'b', 'f', 'x', 'n', 'd'};
    size_t i;
    char* ptr;

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    fprintf(stderr, "\n*** TEST test_iterator_char ***\n");

    queue_iterate(q, iterator_inc_char);
    TEST_ASSERT(data[3] == 'f');
    TEST_ASSERT(queue_length(q) == 7);
    queue_dequeue(q, (void**)&ptr);
    TEST_ASSERT(ptr == &data[0]);
}

// Test for delete when item is in the middle
void test_delete_middle(){
    queue_t q;
    int data[] = {1, 2, 3, 5, 6, 8};
    q = queue_create();
    for (size_t i = 0; i < sizeof(data)/ sizeof(data[0]); i++) {
        queue_enqueue(q, & data[i]);
    }

    fprintf(stderr, "\n*** TEST test_delete_middle ***\n");

    // We delete the value from the queue
    TEST_ASSERT(queue_delete(q, &data[4]) == 0);
    // The size should now be 5 (reduced from 6)
    TEST_ASSERT(queue_length(q) == 5);
}

// Test for delete when item is at the front
void test_delete_front(){
    queue_t q;
    char data[] = {'a', 'b', 'c', 'd'};
    char* dq_value;
    q = queue_create();
    for (size_t i = 0; i < sizeof(data)/ sizeof(data[0]); i++) {
        queue_enqueue(q, & data[i]);
    }

    fprintf(stderr, "\n*** TEST test_delete_front ***\n");

    TEST_ASSERT(data[0] == 'a');
    TEST_ASSERT(queue_delete(q, &data[0]) == 0);
    TEST_ASSERT(queue_length(q) == 3);
    // At this point we dequeue and see that the front is different from earlier
    queue_dequeue(q, (void**)&dq_value);
    // We check with data[1] as the data array won't change
    TEST_ASSERT(dq_value == &data[1]);
}

// Test delete when item is at the rear
void test_delete_rear() {
    queue_t q;
    int data[] = {1, 5, 0};
    int* dq_value1, *dq_value2, *dq_value3;
    q = queue_create();
    for (size_t i = 0; i < sizeof(data)/ sizeof(data[0]); i++) {
        queue_enqueue(q, &data[i]);
    }

    fprintf(stderr, "\n*** TEST test_delete_rear ***\n");

    TEST_ASSERT(data[2] == 0);
    TEST_ASSERT(queue_delete(q, &data[2]) == 0);
    TEST_ASSERT(queue_length(q) == 2);
    // Dequeue the other elements to verify 0 was removed
    queue_dequeue(q, (void**)&dq_value1);
    TEST_ASSERT(dq_value1 == &data[0]);
    queue_dequeue(q, (void**)&dq_value2);
    TEST_ASSERT(dq_value2 == &data[1]);
    // Now if we dequeue, we should face an error as the queue is empty now
    TEST_ASSERT(queue_dequeue(q, (void**)&dq_value3) == -1);
}

// Test delete error case
void test_delete_error() {
    queue_t q;
    q = queue_create();
    int data = 3;

    fprintf(stderr, "\n*** TEST test_delete_error ***\n");

    TEST_ASSERT(queue_delete(NULL, &data) == -1);
    TEST_ASSERT(queue_delete(q, NULL) == -1);
}

int main(void) {
	test_create();
	test_queue_simple();
    test_queue_destroy_fail();
    test_queue_destroy_pass();
    test_queue_enqueue_error();
    test_queue_dequeue_int();
    test_queue_dequeue_char();
    test_queue_dequeue_string();
    test_iterator_int();
    test_iterator_char();
    test_delete_middle();
    test_delete_front();
    test_delete_rear();
    test_delete_error();
	return 0;
}
