#ifndef _LIST_H_
#define _LIST_H_
#include<stdio.h>
#include <stdlib.h>
#include <string.h>
#include "log.h"

typedef struct list_node {
	void *arg;
	struct list_node *plast;
	struct list_node *pnext;
} list_node;

typedef struct list {
	struct list_node *phead;
	struct list_node *ptail;
	struct list_node *iter;	//用于循环时使用
	int size;
} list_t;

struct list *init_list();
int list_push_back(struct list *, void *arg);	//在list的末尾添加一个元素
int list_push_front(struct list *, void *arg); //在list的头部添加一个元素
int list_pop_back(struct list *);	//在list的末尾添加一个元素
int list_pop_front(struct list *); //在list的头部添加一个元素
void list_clear(struct list *);	//清空列表
int list_size(struct list *);	//列表个数
void destroy_list(struct list **);

struct list_node * list_beign(struct list *plist);
struct list_node * list_end(struct list *plist);
struct list_node * list_next(struct list *plist);


#endif