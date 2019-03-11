#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include "list.h"


int main()
{
	// struct list*list_test = list_init1();
	// // destroy_list(&list_test);
	printf("%ld\n", sizeof(list_t));
	return 0;
}