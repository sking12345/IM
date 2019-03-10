#ifdef _LIST_H
#define _LIST_H
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct list_node {
	void *arg;
	struct list_node *pnext;	//下一个
	struct list_node *plast;	//下一个
} list_node_t;

/**
 * 列表操作
 */
typedef struct list {
	struct list_node *phead;
	struct list_node *ptail;
	int szie;	//大小
} list_t;



/**
 * [new_list 创建新列表]
 * @return [description]
 */
struct list * list_init(); //初使化

int list_push_back(struct list *,  void *arg);	//末尾添加一个元素
int list_push_front(struct list *, void *arg);	//头部添加一个元素
int list_pop_front(struct list *);	//删除第一个元素
int list_pop_back(struct list *); //删除最后一个元素
void list_remove_if(struct list *, int (*callback_function)(void *arg)); //如果回调返回0,删除,-1 不删除

int list_clear(struct list *);	//清空
void list_reverse(struct list *);	//把list的元素倒转
int list_size(struct list *);
struct list_node* list_begin(struct list *);
struct list_node* list_end(struct list *);
void * list_get_arg(struct list *);

// for (list_iter_t = begin(), list_iter_t != end(); list_iter_t = list_iter_t->next) {
// 	/* code */
// }
// for (list_iter_t = begin(), list_iter_t != NULL; list_iter_t = list_iter_t->next) {
// 	/* code */
// }


#endif