#include <stdio.h>
#include <stdlib.h>
#define TEST 0x01
int main()
{
#if TEST == 0x00
	printf("%s\n", "ddd");
#endif
#if TEST == 0x01
	printf("%s\n", "ddd1");
#endif
	return 0;
}