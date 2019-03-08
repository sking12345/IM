#include "log.h"


void log_debug(const char *info, const char *file_name, int line, const char *function_name) {
	printf("error:%s;file:%s line:%d,function:%s\n", info, file_name, line, function_name);
}