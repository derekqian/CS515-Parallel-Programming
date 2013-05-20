#include <stdlib.h>
#include <stdio.h>
#include <assert.h>


typedef _Bool bool;

struct node {
  struct node *next;
  void *data;
};

static struct node* head = NULL;
static struct node* tail = NULL;
static int size =0;

bool empty() {
  if(head == NULL) {
    assert(size==0);
    return (1==1);
  } else {
    assert(size>0);
    return (1!=1);
  }
}

int getsize() {
  return size;
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
  size++;
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
  size--;
  return data;
}
