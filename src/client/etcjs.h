#ifndef ETCJS_H
#define ETCJS_H
typedef struct
{
    char* name;
    char* key;
    char allocated;
} etcjs_owner;
typedef enum { ETCJS_ERROR, ETCJS_CONTENT, ETCJS_LIST } etcjs_type;
typedef struct
{
    etcjs_type type;
    void* result;
} etcjs_result;
void etcjs_init(char* host_name, unsigned int port);
void etcjs_init_from_configuration();
void etcjs_cleanup();
etcjs_owner* etcjs_owner_new(char* name, char* key);
etcjs_owner* etcjs_owner_new_from_configuration();
void etcjs_owner_delete(etcjs_owner* owner);
etcjs_result* etcjs_owner_create(etcjs_owner* owner);
etcjs_result* etcjs_config_set(etcjs_owner* owner, char* name, char* content,
        char whole, unsigned int offset, unsigned int size);
etcjs_result* etcjs_config_get(etcjs_owner* owner, char* name);
etcjs_result* etcjs_config_touch(etcjs_owner* owner, char* name);
etcjs_result* etcjs_config_delete(etcjs_owner* owner, char* name);
etcjs_result* etcjs_config_stat(etcjs_owner* owner, char* name);
etcjs_result* etcjs_config_list(etcjs_owner* owner);
void etcjs_result_delete(etcjs_result* result);
#endif
