#ifndef _CURL_H_
#define _CURL_H_
#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>


size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p);
int curl_get(const char *url, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame), char **pstr);
int curl_post(const char *url, const char *data, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame), char **pstr);
void curl_end(char **str);

#endif