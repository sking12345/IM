#include <map>
#include <list>
#include "component/type.h"
#include <string.h>
#include <iostream>
using namespace std;

int main()
{
	struct data_apk apk;
	memset(&apk.buf, 0x00, sizeof(apk.buf));
	memcpy(&apk.buf, "ddd", strlen("ddd"));
	printf("%s\n", apk.buf);

	struct apk_list apk_list;
	apk_list.sockfd = 10;
	apk_list.list.push_back(apk);
	apk_list.list.push_back(apk);
	map<int, struct apk_list> map1;
	map1.insert(pair<int, struct apk_list>(1, apk_list));

	map<int, struct apk_list>::iterator iter = map1.find(1);


	struct apk_list apk_list1 = iter->second;
	printf("%d\n", apk_list1.sockfd);

	list<struct data_apk>::iterator iter_list;

	for (iter_list = apk_list1.list.begin(); iter_list != apk_list1.list.end(); iter_list++)
	{
		printf("%s\n", iter_list->buf);
	}

	return 0;
}
