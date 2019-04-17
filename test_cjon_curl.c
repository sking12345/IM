#include <stdio.h>
#include <stdlib.h>
#include "curl.h"
#include "cJSON.h"
void printJson(cJSON * root) { //以递归的方式打印json的最内层键值对
	for (int i = 0; i < cJSON_GetArraySize(root); i++) { //遍历最外层json键值对
		cJSON * item = cJSON_GetArrayItem(root, i);
		if (cJSON_Object == item->type)     //如果对应键的值仍为cJSON_Object就递归调用printJson
			printJson(item);
		else {                              //值不为json对象就直接打印出键和值
			printf("%s->", item->string);
			printf("%s\n", cJSON_Print(item));
		}
	}
}

void* test(void *buffer, void*user_parame) {
	printf("%s\n", buffer);
	cJSON * root = NULL;
	root = cJSON_Parse(buffer);
	if (!root) {
		printf("Error before: [%s]\n", cJSON_GetErrorPtr());
	} else {
		cJSON * item = NULL;//cjson对象
		item = cJSON_GetObjectItem(root, "xx");//
		printf("%s\n", item->valuestring);
	}
	return NULL;
}

int main() {
	char * str = NULL;
	// curl_get("www.phpframe.com", "ddd", NULL, &str);
	// printf("%s\n", str);
	// cJSON * root  = cJSON_Parse(str);
	// cJSON * item = NULL;//cjson对象
	// item = cJSON_GetObjectItem(root, "xx");//
	// printf("%s\n", item->valuestring);
	// curl_end(&str);
	// curl_post("http://www.phpframe.com/project/?ctl=index&act=login", "&username=admin&password=123456", NULL, NULL, &str);
	// printf("%s\n", str);
	// cJSON * root  = cJSON_Parse(str);
	// cJSON * item = NULL;//cjson对象
	// item = cJSON_GetObjectItem(root, "token");//
	// char *token = (char*)malloc(strlen(item->valuestring) + 1);
	// memset(token, '\0', strlen(item->valuestring) + 1);
	// memcpy(token, item->valuestring, strlen(item->valuestring));
	// printf("tken:%s\n", token);
	// curl_end(&str);

	curl_get("http://www.phpframe.com/project/?ctl=admin&act=index&token=test1419oxmxTYwwVCQYKn929uZR", NULL, NULL, &str);
	printf("%s\n", str);
	curl_end(&str);
}
















