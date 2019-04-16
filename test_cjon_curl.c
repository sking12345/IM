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
		printJson(root);
		cJSON * item = NULL;//cjson对象
		item = cJSON_GetObjectItem(root, "xxx");//
		printf("%s\n", item->valuestring);
	}
	return NULL;
}



int main() {
	curlGet("127.0.0.1", "ddd", test);
}