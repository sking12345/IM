
#include "list.h"

struct list *init_list()
{
	struct list* plist = (struct list*)malloc(sizeof(struct list));
	if (plist == NULL)
	{
		return NULL;
	}
	plist->phead = plist->ptail = NULL;
	plist->size = 0;
	return plist;
}

int list_push_back(struct list *plist, void *arg)
{
	if (plist == NULL)
	{
		log_print("list is null");
		return -1;
	}
	struct list_node *lnode = (struct list_node*)malloc(sizeof(struct list_node));
	lnode->arg = arg;
	lnode->pnext = NULL;
	lnode->plast = NULL;
	if (plist->ptail == NULL)
	{
		plist->phead = plist->ptail = lnode;
	} else {
		lnode->plast = plist->ptail;
		plist->ptail->pnext = lnode;
		plist->ptail = lnode;
	}
	plist->size++;
	return 1;
}
int list_push_front(struct list *plist, void *arg)
{
	if (plist == NULL)
	{
		log_print("list is null");
		return -1;
	}
	struct list_node *lnode = (struct list_node*)malloc(sizeof(struct list_node));
	lnode->arg = arg;
	lnode->plast = NULL;
	if (plist->phead == NULL)
	{
		plist->phead = plist->ptail = lnode;
	} else {
		lnode->pnext = plist->phead;
		plist->phead = lnode;
	}
	plist->size++;
	return 1;
}
//删除末尾的
int list_pop_back(struct list *plist)
{
	if (plist == NULL)
	{
		log_print("list is null");
		return -1;
	}
	if (plist->ptail == NULL)
	{
		log_print("list tail is NULL");
		return -2;
	}
	struct list_node *node = plist->ptail;
	plist->ptail->plast->pnext = NULL;
	plist->ptail = plist->ptail->plast;
	free(node);
	node = NULL;
	plist->size--;
	return 1;
}
//删除头部
int list_pop_front(struct list *plist)
{
	if (plist == NULL)
	{
		log_print("list is null");
		return -1;
	}
	if (plist->phead == NULL)
	{
		log_print("list tail is NULL");
		return -2;
	}
	struct list_node *node = plist->phead;
	plist->phead->pnext->plast = NULL;
	plist->phead =  plist->phead->pnext;
	free(node);
	node = NULL;
	plist->size--;
	return 1;
}
void list_clear(struct list *plist)
{
	struct list_node *node = NULL;
	struct list_node *list_head = plist->phead;
	while (list_head != NULL)
	{
		node = list_head;
		list_head = list_head->pnext;
		free(node);
		node = NULL;
	}
	plist->ptail = NULL;
	plist->phead = NULL;
	plist->size = 0;
	return;
}
int list_size(struct list *plist) {
	return plist->size;
}
void destroy_list(struct list **plist)
{
	if (*plist == NULL)
	{
		return;
	}
	list_clear(*plist);
	free(*plist);
	*plist = NULL;
	return ;
}

struct list_node * list_beign(struct list *plist)
{
	plist->iter = plist->phead;
	return plist->phead;
}
struct list_node * list_end(struct list *plist)
{
	return NULL;
}
struct list_node * list_next(struct list *plist)
{
	plist->iter = plist->iter->pnext;
	return plist->iter;
}



















