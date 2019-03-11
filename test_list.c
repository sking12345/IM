#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "list.h"


int main()
{
	struct list * plist = init_list();

	list_push_back(plist, (void*)"ddd");
	list_push_back(plist, (void*)"ddd1");
	list_push_front(plist, (void*)"dddxx");
	// list_pop_front(plist);
	// list_clear(plist);

	struct list_node *iter ;
	for (iter = list_beign(plist); iter != list_end(plist); iter = list_next(plist))
	{
		printf("for::%s\n", (void*)iter->arg);
	}

	destroy_list(&plist);
	return 0;
}