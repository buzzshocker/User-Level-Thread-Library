#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "queue.h"
#include "private.h"

// Custom struct to be the node of the linked list for the queue
struct node {
    // Holds the data
    void* data;
    // Points to the next element in the list
    struct node* next;
};

typedef struct node* q_node;

// Helper function to create a new node
q_node new_node(void* key) {
    // Assign the temp node
    q_node temp_node = (q_node) malloc(sizeof(struct node));
    // Assign the data passed by the function to the queue
    temp_node -> data = key;
    temp_node -> next = NULL;
    return temp_node;
}

struct queue {
    // Front and rear pointers of the linked list
    q_node front;
    q_node rear;
    // Holds the count of elements in the queue
    size_t count;
};

queue_t queue_create(void) {
    // Assign the space to the queue
    queue_t q = (struct queue*) malloc(sizeof(struct queue));
    // Initialise the front and the rear of the queue
    q -> front = NULL;
    q -> rear = NULL;
    return q;
}

int queue_destroy(queue_t queue) {
    // Error checks if queue is NULL or is empty
    if (queue == NULL || queue -> count != 0) {
        return -1;
    }
    // free the queue if error conditions aren't true
    queue -> front = NULL;
    queue -> rear = NULL;
    free(queue);
    queue = NULL;
    return 0;
}

int queue_enqueue(queue_t queue, void *data) {
    // Error checks
    if (queue == NULL || data == NULL) {
        return -1;
    }
    // Create new node using helper function
    q_node eq_node = new_node(data);
    // Error check if node not created
    if (eq_node == NULL) {
        return -1;
    }
    // If it is the first node being inserted, front = rear
    if (queue -> count == 0) {
        queue -> rear = queue -> front = eq_node;
    }
    // Make the next of the current rear to be the new node inserted
    queue -> rear -> next = eq_node;
    // The new node inserted is the rear
    queue -> rear = eq_node;
    // Increment queue count since new element added
    queue -> count++;
    return 0;
}

int queue_dequeue(queue_t queue, void **data) {
    // Error checks
    if (queue -> count == 0 || queue == NULL || data == NULL) {
        return -1;
    }
    // Assign the data that was dequeued to the data variable passed in
    *data = queue -> front -> data;
    // Assign the front to be the next value of the front
    queue -> front = queue -> front -> next;
    // Decrement the queue size counter since element removed
    queue -> count--;
    // If the queue becomes empty,
    if (queue -> count == 0) {
        queue -> front = queue -> rear = NULL;
    }
    return 0;
}

int queue_delete(queue_t queue, void *data) {
    // Error checks
    if (queue == NULL || data == NULL) {
        return -1;
    }
    // Assign two temporary nodes
    q_node del_node = queue -> front;  // This one should be the queue -> front
    q_node support_node;
    // If the data is at the beginning of the queue
    if (queue -> front -> data == data) {
        // Make the front to be the next node of the front
        queue -> front = queue -> front -> next;
        // Free the temporary node
        free(del_node);
        // Decrement queue count and return
        queue -> count--;
        return 0;
    } else if (queue -> rear -> data == data) {  // If data is at the end
        // Iterate through till we find the next to last node
        while (del_node -> next -> next != NULL) {
            del_node = del_node -> next;
        }
        // Once we do, make that node the rear and the next node NULL
        support_node = queue -> rear;
        queue -> rear = del_node;
        del_node -> next = NULL;
        // Decrement count
        queue -> count--;
        // Free the previously rear node
        free(support_node);
        return 0;
    } else {
        // Loop through the queue
        while (del_node != NULL && del_node -> data != data) {
            // Assign nodes to move through the queue
            support_node = del_node;
            del_node = del_node -> next;
            // If del node becomes last one, the data isn't there
            if (del_node == NULL) {
                return -1;
            }
            // Assign the node next to the current node, to be the next to next
            // node
            support_node -> next = del_node -> next;
            // Decrement count, free the node, and return 0
            queue -> count--;
            free(del_node);
            return 0;
        }
    }
    // Return -1 if data not found in array
    return -1;
}

int queue_iterate(queue_t queue, queue_func_t func) {
    // Error checks
    if (queue == NULL || func == NULL) {
        return -1;
    }
    // Assign a temporary iterator node
    q_node it_node = queue -> front;
    while (it_node != NULL) {
        // Apply the function to every node and edit the data for the same
        func(queue, it_node -> data);
        it_node = it_node -> next;
    }
    return 0;
}

int queue_length(queue_t queue) {
    // Error checks
    if (queue == NULL) {
        return -1;
    }
    // Return the count of elements
    return queue -> count;
}
