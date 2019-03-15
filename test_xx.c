#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 0x01

typedef struct test {

	char *buf;
} test_t;

char * new_mall() {
	char * buf = (char*)malloc(10);
	return buf;
}

void  free_mall(struct test * tt) {
	free(tt->buf);
	tt->buf = NULL;
}

int main() {

	char *buf = new_mall();
	printf("%p\n", buf);

	struct test *tt = (struct test*)malloc(sizeof(struct test));
	tt->buf = buf;
	printf("%p\n", tt->buf );
	free_mall(tt);
	printf("%p\n", tt->buf );
	free(tt);
	tt = NULL;

	return 0;
}