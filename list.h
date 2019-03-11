#ifndef _LIST_H_
#define _LIST_H_
#include<stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct list_node {
	void *arg;
	struct list_node *plast;
	struct list_node *pnext;
} list_node;

typedef struct list {
	struct list_node *phead;
	struct list_node *ptail;
	int size;
} list_t;

struct list *init_list();
void destroy_list(struct list **);

#endif