#include "curl.h"

typedef struct curl_call_parame {
    void* (*callback_function)(void *buffer, void*user_parame);
    void *user_parame;
    char **pstr;
} curl_call_parame_t;


size_t process_data(void *buffer, size_t size, size_t nmemb, void *user_p) {
    struct curl_call_parame  *user_parame = ( struct curl_call_parame*)user_p;
    if (user_parame->callback_function != NULL) {
        char *buf = (char*)malloc(nmemb + 1);
        memset(buf, '\0', nmemb + 1);
        memcpy(buf, buffer, nmemb);
        (*(user_parame->callback_function))(buf, user_parame->user_parame);
        free(buf);
        buf = NULL;
    } else {
        if (user_parame->pstr != NULL)
        {
            *(user_parame->pstr) = (char*)malloc(nmemb + 1);
            memset(*(user_parame->pstr), '\0', nmemb + 1);
            memcpy(*(user_parame->pstr), buffer, nmemb );
        }
    }
    return 0;
}

void curl_end(char **str)
{
    if (*str != NULL)
    {
        free(*str);
        *str = NULL;
    }
}

int curl_get(const char *url, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame), char **pstr) {
    CURL *curl = NULL;
    CURLcode res;
    struct curl_call_parame call_parame;
    call_parame.callback_function = callback_function;
    call_parame.user_parame = user_parame;
    call_parame.pstr = pstr;
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

int curl_post(const char *url, const char *data, void *user_parame, void* (*callback_function)(void *buffer, void*user_parame), char **pstr) {
    CURL *curl = NULL;
    CURLcode res;
    struct curl_call_parame call_parame;
    call_parame.callback_function = callback_function;
    call_parame.user_parame = user_parame;
    call_parame.pstr = pstr;
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









