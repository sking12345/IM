#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define TEST 0x01



typedef union test {
	int fd;
	char *arg;
} test_t;

typedef struct test_t
{
	int fd;
	char * arg;
} test_t_t;


void test(char *buf, int fd)
{
	struct test_t tt;

	memcpy( &tt, buf + fd * sizeof(struct test_t), sizeof(struct test_t));
	printf("%d\n", tt.fd);
}

char *test1(int count)
{
	char * buf = (char*)malloc(sizeof(struct test_t) * 10);
	memset(buf, 0x00, sizeof(struct test_t) * 10);
	for (int i = 4; i < 10; ++i)
	{
		struct test_t tt;
		tt.fd = i;
		tt.arg = NULL;
		memcpy(buf + i * sizeof(struct test_t), &tt, sizeof(struct test_t));
	}
	return buf;
}

int main() {

	char *buf = test1(10);

	printf("%s\n", "x---------");
	test(buf, 1);
	test(buf, 5);
	test(buf, 8);
	test(buf, 4);
	// for (int i = 0; i < 10; ++i)
	// {
	// 	struct test_t tt;
	// 	tt.fd = i;
	// 	tt.arg = NULL;
	// 	memcpy( &tt, buf + i * sizeof(struct test_t), sizeof(struct test_t));
	// 	printf("%d\n", tt.fd);
	// }

	return 0;
}