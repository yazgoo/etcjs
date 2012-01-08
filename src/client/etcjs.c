#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <curl/curl.h>
#include "etcjs.h"
char* etcjs_host_name = "localhost";
unsigned int etcjs_port = 443;
CURL* handle;
void etcjs_init(char* host_name, unsigned int port)
{
    if(host_name != 0) etcjs_host_name = host_name;
    if(port != 0) etcjs_port = port;
    handle = curl_easy_init();
}
void etcjs_cleanup()
{
    curl_easy_cleanup(handle);
}
etcjs_owner* etcjs_owner_new(char* name, char* key)
{
    etcjs_owner* owner = malloc(sizeof(etcjs_owner));
    owner->name = name;
    owner->key = key;
    return owner;
}
void etcjs_owner_delete(etcjs_owner* owner) { free(owner); }
char* etcjs_build_url(char* path)
{
    unsigned int i, j = 0;
    size_t length;
    char* result;
    for(i = etcjs_port; i /= 10; j++);
    length = strlen("http://") + strlen(etcjs_host_name)
         + 1 + j + 1 + 1 + strlen(path)+ 1;
    result = calloc(length, sizeof(char));
    snprintf(result, length, "http://%s:%d/%s", etcjs_host_name, etcjs_port, path);
    return result;
}
unsigned int str_encoded_len(char* str)
{
    return strlen(str);
}
void etcjs_append_encoded(char* result, char* arg)
{
    if(arg != NULL) strcat(result, arg);
}
unsigned int etcjs_url_length_get(va_list ap, int n)
{
    unsigned int length = 1;
    char* arg;
    int i = 0;
    for(arg = va_arg(ap, char*); i++ < n; arg = va_arg(ap, char*))
        length += str_encoded_len(arg) + 1;
    va_end(ap);
    return length;
}
char* etcjs_url_encode(va_list ap, int n, unsigned int length)
{
    char* result;
    char* arg;
    int i;
    result = (char*) malloc(length * sizeof(char));
    result[0] = result[length - 1] = 0;
    i = 0;
    for(arg = va_arg(ap, char*); i < n; arg = va_arg(ap, char*))
    {
        etcjs_append_encoded(result, arg);
        if((i % 2) == 0) strcat(result, "=");
        else strcat(result, "&");
        i++;
    }
    va_end(ap);
    return result;
}
static size_t etcjs_curl_write(
        void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t realsize = nmemb * size;
    char** destination = (char**)userp;
    *destination = (char*) malloc(realsize);
    contents = memcpy(*destination, contents, realsize);
    return realsize;
}
etcjs_result* etcjs_post(char* path, int n, ...)
{
    long error_code;
    va_list ap;
    etcjs_result* result = malloc(sizeof(etcjs_result));
    char* url = etcjs_build_url(path);
    n *= 2;
    va_start(ap, n); 
    unsigned int length = etcjs_url_length_get(ap, n);
    va_start(ap, n); 
    char* post_fields = etcjs_url_encode(ap, n, length);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, etcjs_curl_write);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&result->result);
    curl_easy_perform(handle);
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &error_code);
    free(post_fields);
    free(url);
    result->type = error_code == 200?ETCJS_CONTENT:ETCJS_ERROR;
    return result;
}
etcjs_result* etcjs_owner_create(etcjs_owner* owner)
{
    return etcjs_post("owner/create", 2,
            "name", owner->name,
            "key",  owner->key);
}
etcjs_result* etcjs_config_set(etcjs_owner* owner, char* name, char* content)
{
    return etcjs_post("config/set", 4,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name,
            "content",  content);
}
etcjs_result* etcjs_config_get(etcjs_owner* owner, char* name)
{
    return etcjs_post("config/get", 3,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name);
}
etcjs_result* etcjs_config_list(etcjs_owner* owner)
{
    etcjs_result* result = etcjs_post("config/list", 2,
            "owner",    owner->name,
            "key",      owner->key);
    result->type = ETCJS_LIST;
    return result;
}
void etcjs_result_delete(etcjs_result* result)
{
    if(result->result != 0) free(result->result);
    free(result);
}
