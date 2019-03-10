#include "list.h"

struct list * list_init() {
	struct list * plist = (struct list*)malloc(sizeof(struct list));
	plist->szie = 0;
	plist->phead = plist->ptail = NULL;
}

int list_push_back(struct list *plist,  void *arg) {
	struct list_node *plist_node = malloc(sizeof(struct list_node));
	plist_node->pnext = NULL;
	plist_node->arg = arg;
	if (plist->phead == NULL) {
		plist->phead = plist->ptail = plist_node;
		plist_node->plast = plist->phead;
	} else {
		plist->ptail->pnext = plist_node;
		plist->ptail = plist_node;
		plist_node->plast = plist->ptail->plast;
	}
	plist->szie++;
	return 1;
}
int list_push_front(struct list *plist, void *arg) {
	struct list_node *plist_node = malloc(sizeof(struct list_node));
	plist_node->arg = arg;
	if (plist->phead == NULL) {
		plist->phead = plist->ptail = plist_node;
		plist->phead->plast = plist_node;
	} else {
		plist->phead->plast = plist_node;
		plist_node->pnext = plist->phead;
		plist->phead = plist_node;
	}
	plist->szie++;
	return 1;
}

int list_pop_front(struct list *plist) {
	struct list_node *plist_node = plist->phead;
	plist->phead = plist->phead->next;
	free(plist_node);
	plist_node = NULL;
	plist->szie--;
	return 1;
}
//
int list_pop_back(struct list *plist) {
	struct list_node *plist_node = plist->ptail;
	plist->ptail = plist->ptail->plast;
	free(plist_node);
	plist_node = NULL;
	plist->szie--;
	return 1;
}

int list_clear(struct list *plist) {//清空
	struct list_node *plist_node_head = plist->phead;
	struct list_node * node = NULL;
	while (plist_node_head != NULL) {
		node = plist_node_head;
		plist_node_head = plist_node_head->next;
		free(node);
		node = NULL;
	}
	plist->phead = plist->ptail = NULL;
	plist->szie = 0;
	return 0;
}


// void list_reverse(struct list *);	//把list的元素倒转
int list_size(struct list *plist) {
	return plist->szie;
}
struct list_node *list_begin(struct list *plist) {
	return plist->phead;
}
struct list_node *list_end(struct list *plist) {
	return plist->ptail;
}
struct list_node *list_next(struct list_node *node) {
	return node->next;
}

void* list_get_arg(struct list *plist) {
	return plist->arg;
}

void list_destroy(struct list **plist) {
	list_clear(*plist);
	free(*plist);
	*plist = NULL;
}










