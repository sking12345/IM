
#include "log.h"

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

void destroy_list(struct list **plist)
{

}

