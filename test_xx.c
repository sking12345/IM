#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 0x01



typedef union test {
	int fd;
	char *arg;
} test_t;
int main() {
	test_t test = 1;
	printf("%s\n", "ok");
	return 0;
}