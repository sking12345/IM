#include "log.h"


void log_debug(const char *info, const char *file_name, int line, const char *function_name) {

	printf("function:%s\n error:%s\n file:%s line:%d\n", function_name, info, file_name, line);
}

void log_print(const char *info)
{
	printf("%s\n", info );
}