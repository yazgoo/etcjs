#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "etcjs.h"
#ifndef NULL
#define NULL 0
#endif
typedef struct
{
    char* owner, *key, *action, *configuration, *host_name;
    unsigned int port;
} params;
void help()
{
    printf("Usage:\n");
    printf("Options:\n");
    exit(1);
}
void parse_params(int argc, char** argv, params* p)
{
    char c;
    p->owner = p->key = p->action = p->configuration = p->host_name = 0;
    p->port = 80;
    while ((c = getopt(argc, argv, "o:k:a:c:hp:n:")) != -1)
    {
        switch(c)
        {
            case 'o': p->owner = optarg; break;
            case 'k': p->key = optarg; break;
            case 'a': p->action = optarg; break;
            case 'c': p->configuration = optarg; break;
            case 'n': p->host_name = optarg; break;
            case 'p': p->port = atoi(optarg); break;
            case 'h':
            default: help();
        }
    }
    if(p->owner == NULL || p->key == NULL || p->action == NULL) help();
}
etcjs_result do_action(params* p)
{
    etcjs_owner* owner;
    etcjs_result result;
    etcjs_init(p->host_name, p->port);
    owner = etcjs_owner_new(p->owner, p->key);
    switch(p->action[0])
    {
        case 'c': result = etcjs_owner_create(owner); break;
        case 'g': result = etcjs_config_get(owner, p->configuration); break;
        case 's': result = etcjs_config_set(owner, p->configuration, ""); break;
        case 'l': result = etcjs_config_list(owner); break;
        default: help();
    }
    etcjs_owner_delete(owner);
    etcjs_cleanup();
    return result;
}
display_result(etcjs_result result)
{
    switch(result.type)
    {
        case ETCJS_ERROR: printf("error %s", (char*) result.result); break;
        case ETCJS_CONTENT: printf("%s", (char*) result.result); break;
        case ETCJS_LIST: printf("list: TODO"); break;
    }
    printf("\n");
}
int main(int argc, char** argv)
{
    params p;
    parse_params(argc, argv, &p);
    display_result(do_action(&p));
}
