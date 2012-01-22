#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <curl/curl.h>
#include <stdio.h>
#include "etcjs.h"
char* etcjs_host_name = "localhost";
unsigned int etcjs_port = 443;
char allocated_host_name = 0;
CURL* handle;
void etcjs_init(char* host_name, unsigned int port)
{
    if(host_name != 0) etcjs_host_name = host_name;
    if(port != 0) etcjs_port = port;
    handle = curl_easy_init();
}
void* etcjs_do_with_configuration(char* name, void* (*callback) (FILE* f))
{
    char config[512];
    FILE* config_file;
    snprintf(config, 512, "%s/.config/etcjs_cli/%s", getenv("HOME"), name);
    config_file = fopen(config, "r");
    void* result = callback(config_file);
    if(config_file) fclose(config_file);
    return result;
}
void* load_server_configuration(FILE* f)
{
    char* host_name = 0;
    unsigned int port = 0;
    if(f)
    {
        host_name = (char*) calloc(256, sizeof(char));
        fscanf(f, "%256[^:]:%d", host_name, &port);
        if(host_name) allocated_host_name = 1;
    }
    etcjs_init(host_name, port);
    return 0;
}
void* load_owner_configuration(FILE* f)
{
    char* name = 0;
    char* key = 0;
    if(f)
    {
        name = (char*) calloc(256, sizeof(char));
        key = (char*) calloc(256, sizeof(char));
        fscanf(f, "%256s\n", name);
        fscanf(f, "%256s\n", key);
    }
    etcjs_owner* owner = etcjs_owner_new(name, key);
    if(f) owner->allocated = 1;
    return owner;
}
void etcjs_init_from_configuration()
{
    etcjs_do_with_configuration("server", load_server_configuration);
}
etcjs_owner* etcjs_owner_new_from_configuration()
{
    return (etcjs_owner*)
        etcjs_do_with_configuration("owner", load_owner_configuration);
}
void etcjs_cleanup()
{
    curl_easy_cleanup(handle);
    if(allocated_host_name) free(etcjs_host_name);
}
etcjs_owner* etcjs_owner_new(char* name, char* key)
{
    etcjs_owner* owner = malloc(sizeof(etcjs_owner));
    owner->name = name;
    owner->key = key;
    owner->allocated = 0;
    return owner;
}
void etcjs_owner_delete(etcjs_owner* owner)
{
    if(owner->allocated)
    {
        free(owner->key);
        free(owner->name);
    }
    free(owner);
}
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
    size_t previous_size = strlen(*destination);
    size_t new_size = previous_size + realsize + 1;
    *destination = (char*) realloc(*destination, new_size);
    contents = memcpy(*destination + previous_size, contents, realsize);
    *(*destination + new_size -1) = 0;
    return realsize;
}
etcjs_result* etcjs_post(char* path, etcjs_type type, int n, ...)
{
    long error_code;
    va_list ap;
    etcjs_result* result = malloc(sizeof(etcjs_result));
    char* url = etcjs_build_url(path);
    result->result = (char*) malloc(1);
    *((char*)result->result) = 0;
    n *= 2;
    va_start(ap, n); 
    unsigned int length = etcjs_url_length_get(ap, n);
    va_start(ap, n); 
    char* post_fields = etcjs_url_encode(ap, n, length);
    curl_easy_setopt(handle, CURLOPT_URL, url);
    curl_easy_setopt(handle, CURLOPT_POSTFIELDS, post_fields);
    curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, etcjs_curl_write);
    curl_easy_setopt(handle, CURLOPT_WRITEDATA, (void *)&(result->result));
    curl_easy_perform(handle);
    curl_easy_getinfo(handle, CURLINFO_RESPONSE_CODE, &error_code);
    free(post_fields);
    free(url);
    result->type = error_code == 200?type:ETCJS_ERROR;
    return result;
}
etcjs_result* etcjs_owner_create(etcjs_owner* owner)
{
    return etcjs_post("owner/create", ETCJS_CONTENT, 2,
            "name", owner->name,
            "key",  owner->key);
}
char* to_s(unsigned int n)
{
    unsigned char i;
    unsigned int m = n;
    for(i = 2; m != 0; m /= 10) i++;
    char* result = (char*) calloc(i, sizeof(char));
    snprintf(result, i, "%d", n);
    return result;
}
etcjs_result* etcjs_config_set(
        etcjs_owner* owner, char* name, char* content,
        char whole, unsigned int offset, unsigned int size)
{
    char* offset_string, * size_string;
    if(!whole)
    {
        offset_string = to_s(offset);
        size_string = to_s(size);
    }
    etcjs_result* result =
        etcjs_post("config/set", ETCJS_CONTENT, 4 + (whole?0:2),
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name,
            "content",  content,
            "offset", offset_string,
            "size", size_string);
    if(whole) return result;
    free(offset_string);
    free(size_string);
    return result;
}
etcjs_result* etcjs_config_touch(etcjs_owner* owner, char* name)
{
    return etcjs_post("config/touch", ETCJS_CONTENT, 3,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name);
}
etcjs_result* etcjs_config_delete(etcjs_owner* owner, char* name)
{
    return etcjs_post("config/delete", ETCJS_CONTENT, 3,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name);
}
etcjs_result* etcjs_config_get(etcjs_owner* owner, char* name)
{
    return etcjs_post("config/get", ETCJS_CONTENT, 3,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name);
}
etcjs_result* etcjs_config_list(etcjs_owner* owner)
{
    etcjs_result* result = etcjs_post("config/list", ETCJS_LIST, 2,
            "owner",    owner->name,
            "key",      owner->key);
    return result;
}
etcjs_result* etcjs_config_stat(etcjs_owner* owner, char* name)
{
    etcjs_result* result = etcjs_post("config/stat", ETCJS_CONTENT, 3,
            "owner",    owner->name,
            "key",      owner->key,
            "name",     name);
    return result;
}
void etcjs_result_delete(etcjs_result* result)
{
    if(result->result != 0) free(result->result);
    free(result);
}
