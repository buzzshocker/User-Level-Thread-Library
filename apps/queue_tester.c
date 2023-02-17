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
	}                                   \
    printf("\n");             \
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

void test_queue_destroy_pass(void) {

    char data = 'x', *ptr;
    queue_t q;

    fprintf(stderr, "*** TEST queue_destroy_pass ***\n");

    q = queue_create();
    queue_enqueue(q, &data);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(queue_destroy(q) == 0);
}

void test_queue_enqueue_error(void) {
    queue_t q = queue_create();
   // size_t data = 12;

    fprintf(stderr, "*** TEST queue_enqueue_error ***\n");

    TEST_ASSERT(queue_enqueue(q, NULL) == -1);
    TEST_ASSERT(queue_destroy(q) == 0);
    TEST_ASSERT(queue_length(q) == 0);
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

void test_queue_dequeue_string(void) {
    char* data = "hello there", **ptr;
    queue_t q = queue_create();

    fprintf(stderr, "*** TEST queue_dequeue_string ***\n");

    queue_enqueue(q, &data);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(ptr == &data);
}

static void iterator_inc_int(queue_t q, void *data) {
    int *a = (int*)data;

    if (*a == 21)
        queue_delete(q, data);
    else
        *a *= 2;
}

void test_iterator_int(void) {
    queue_t q;
    int data[] = {3, 6, 9, 12, 15, 18, 21, 24, 27, 30, 33};
    size_t i;
    int* ptr;

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    fprintf(stderr, "*** TEST test_iterator_int ***\n");

    queue_iterate(q, iterator_inc_int);
    TEST_ASSERT(data[3] == 24);
    TEST_ASSERT(queue_length(q) == 10);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(ptr == &data[0]);
}

static void iterator_inc_char(queue_t q, void *data) {
    char *a = (char*)data;

    if (*a == 'f') {
        queue_length(q);
        return;
    }
    else{
        *a += 2;
    }
}

void test_iterator_char(void) {
    queue_t q;
    char data[] = {'a', 'm', 'b', 'f', 'x', 'n', 'd'};
    size_t i;
    char* ptr;

    q = queue_create();
    for (i = 0; i < sizeof(data) / sizeof(data[0]); i++)
        queue_enqueue(q, &data[i]);

    fprintf(stderr, "*** TEST test_iterator_char ***\n");

    queue_iterate(q, iterator_inc_char);
    TEST_ASSERT(data[3] == 'f');
    TEST_ASSERT(queue_length(q) == 7);
    queue_dequeue(q, (void** )&ptr);
    TEST_ASSERT(ptr == &data[0]);
}

void test_delete(){
    queue_t q;
    int data[] = {1, 2, 3, 5, 6, 8};
    q = queue_create();

    for (size_t i = 0; i < sizeof(data)/ sizeof(data[0]); i++) {
        queue_enqueue(q, & data[i]);
    }

    queue_delete(q, &data[4]);
    TEST_ASSERT(queue_length(q) == 5);
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
    test_delete();
	return 0;
}
