/******************************************************************************
   list.c

   Implementation of simple linked list defined in list.h.

******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "list.h"

int lock = 0;

pthread_mutex_t mutex = 0;


/* list_new: return a new list structure */
List *list_new(void)
{
  List *l;

  l = (List *) malloc(sizeof(List));
  l->len = 0;

  /* insert root element which should never be removed */
  l->first = l->last = (Node *) malloc(sizeof(Node));
  l->first->elm = NULL;
  l->first->next = NULL;
  return l;
}

/* list_add: add node n to list l as the last element */
void list_add(List *l, Node *n)
{
  // // lock 1
  // while(test_and_set(&lock)){}
  
  // // lock 2
  // int key = 0;
  // key = 1;
  // while(key == 1) {
  //   swap(&lock, &key);
  // }
  
  // mutex
  pthread_mutex_lock(&mutex);
  
  l->last->next = n;
  l->last = n;
  l->len++;
  
  pthread_mutex_unlock(&mutex);
  //lock = 0;
}

/* list_remove: remove and return the first (non-root) element from list l */
Node *list_remove(List *l)
{
  if(l->len == 0) return NULL;
  
  // // lock 1
  // while(test_and_set(&lock)){}
  
  // // lock 2
  // int key = 0;
  // key = 1;
  // while(key == 1) {
  //   swap(&lock, &key);
  // }
  
  // mutex
  pthread_mutex_lock(&mutex);
  
  Node *n = l->first->next;
  l->first->next = l->first->next->next;
  l->len--;

  pthread_mutex_unlock(&mutex);
  //lock = 0;
  return n;
}

/* node_new: return a new node structure */
Node *node_new(void)
{
  Node *n;
  n = (Node *) malloc(sizeof(Node));
  n->elm = NULL;
  n->next = NULL;
  return n;
}

/* node_new_str: return a new node structure, where elm points to new copy of s */
Node *node_new_str(char *s)
{
  Node *n;
  n = (Node *) malloc(sizeof(Node));
  n->elm = (void *) malloc((strlen(s)+1) * sizeof(char));
  strcpy((char *) n->elm, s);
  n->next = NULL;
  return n;
}

// Lock implementation 1.
int test_and_set(int *target)
{
  int old = *target;
  *target = 1;
  return old;
}

// Lock implementation 2.
void swap(int *a, int *b)
{
  int temp = *a; 
  *a = *b; 
  *b = temp;
}