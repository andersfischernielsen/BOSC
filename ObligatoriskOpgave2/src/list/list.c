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

pthread_mutex_t mutex;
pthread_mutex_t mutexes[512]; // 512 lists totally in one run.


/* list_new: return a new list structure */
List *list_new(void)
{
  // mutex
  pthread_mutex_lock(&mutex);
  List *l;

  l = (List *) malloc(sizeof(List));
  l->len = 0;

  /* insert root element which should never be removed */
  l->first = l->last = (Node *) malloc(sizeof(Node));
  l->first->elm = NULL;
  l->first->next = NULL;
  pthread_mutex_unlock(&mutex);
  return l;
}

/* list_add: add node n to list l as the last element */
void list_add(List *l, Node *n)
{ 
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
  // mutex
  pthread_mutex_lock(&mutex);
  if(l->len == 0) return NULL;
  
  Node *n = l->first->next;
  l->first->next = l->first->next->next;
  l->len--;

  if(l->len == 0) l->last = l->first;
  pthread_mutex_unlock(&mutex);
  //lock = 0;
  return n;
}

/* node_new: return a new node structure */
Node *node_new(void)
{
  // mutex
  pthread_mutex_lock(&mutex);
  Node *n;
  
  n = (Node *) malloc(sizeof(Node));
  n->elm = NULL;
  n->next = NULL;
  
  pthread_mutex_unlock(&mutex);
  return n;
}

/* node_new_str: return a new node structure, where elm points to new copy of s */
Node *node_new_str(char *s)
{
  // mutex
  pthread_mutex_lock(&mutex);
  Node *n;
  n = (Node *) malloc(sizeof(Node));
  n->elm = (void *) malloc((strlen(s)+1) * sizeof(char));
  strcpy((char *) n->elm, s);
  n->next = NULL;
  pthread_mutex_unlock(&mutex);
  return n;
}

pthread_mutex_t get_or_add_mutex(List *list)
{
  int i = 0;
  while(i < 512)
  {
    if (mutexes[i] != NULL && mutexes[i] == &list) return mutexes[i];
    else if(mutexes[i]==NULL) mutexes[i] = &list;
    
    i++;
  }
}