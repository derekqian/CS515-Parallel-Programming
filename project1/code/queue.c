#include <stdlib.h>
#include <stdio.h>


typedef _Bool bool;

struct node {
  struct node *next;
  void *data;
};

static struct node* head = NULL;
static struct node* tail = NULL;

bool empty() {
  if(head == NULL) {
    return (1==1);
  } else {
    return (1!=1);
  }
}

void enqueue(void *data) {
  if (tail == NULL) {
    tail = (struct node *) malloc(sizeof(struct node));
    head = tail;
  } else {
    tail->next = (struct node *) malloc(sizeof(struct node));
    tail = tail->next;
  }
  tail->next = NULL;
  tail->data = data;
}

void* dequeue() {
  if(empty()) {
    return NULL;
  }
  struct node *temp = head;
  void *data = temp->data;
  head = head->next;
  if (head == NULL) {
    tail = head;
  }
  free(temp);
  return data;
}
