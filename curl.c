#include <curl/curl.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>

typedef struct curl_call_parame {
    void* (*callback_function)(void *buffer, void*user_parame);
    void *user_parame;
} curl_call_parame_t;


size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p) {
    struct curl_call_parame  *user_parame = ( struct curl_call_parame*)user_p;
    if (user_parame->callback_function != NULL) {
        (*(user_parame->callback_function))(buffer, user_parame->user_parame);
    }
    return 0;
}

int curlGet(const char *url, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame)) {
    CURL *curl = NULL;
    CURLcode res;
    struct curl_call_parame call_parame;
    call_parame.callback_function = callback_function;
    call_parame.user_parame = user_parame;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, process_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&call_parame);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
        return -1;
    } else {
        return 0;
    }
}

int curlPost(const char *url, const char *data, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame)) {
    CURL *curl = NULL;
    CURLcode res;
    struct curl_call_parame call_parame;
    call_parame.callback_function = callback_function;
    call_parame.user_parame = user_parame;
    curl = curl_easy_init();
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, strlen(data));
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, process_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void*)&call_parame);
    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
    if (res != CURLE_OK && res != CURLE_WRITE_ERROR) {
        return -1;
    } else {
        return 0;
    }
}

void* test(void *buffer, void*user_parame) {
    printf("%s\n", buffer);
    return NULL;
}

int main() {
    curlGet("127.0.0.1", "ddd", test);
}








