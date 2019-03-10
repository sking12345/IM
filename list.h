#ifdef _LIST_H
#define _LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct _list {
	void *arg;
	struct _list *next;	//下一个
	struct _list *last; //上一个
} _list_t;

/**
 * 列表操作
 */
typedef struct list {
	struct _list *head;
	struct _list *tail;
	int szie;	//大小
} list_t;

typedef struct _list list_iter_t;

/**
 * [new_list 创建新列表]
 * @return [description]
 */
struct list * list_init(); //初使化

void list_push_back(struct list *,  void *arg);	//末尾添加一个元素
void list_push_front(struct list *, void *arg);	//头部添加一个元素
void list_pop_front(struct list *);	//删除第一个元素
void list_pop_back(struct list *); //删除最后一个元素
void list_remove_if(struct list *, int (*callback_function)(void *arg)); //如果回调返回0,删除,-1 不删除

void list_clear(struct list *);	//清空
void list_reverse(struct list *);	//把list的元素倒转
void list_size(struct list *);
void list_begin();
void list_end();
void list_get_arg();

// for (list_iter_t = begin(), list_iter_t != end(); list_iter_t = list_iter_t->next) {
// 	/* code */
// }
// for (list_iter_t = begin(), list_iter_t != NULL; list_iter_t = list_iter_t->next) {
// 	/* code */
// }


#endif