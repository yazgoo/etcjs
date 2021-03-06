#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include "etcjs.h"
#ifndef NULL
#define NULL 0
#endif
typedef struct
{
    char* owner, *key, *action, *configuration, *host_name, *data;
    unsigned int port;
} params;
void help()
{
    printf("Usage:\n");
    printf("  etcjs_cli [ -k key -o owner ] -a action [ -c configuration ]");
    printf(" [ -h ]\n");
    printf("\t[ -n hostname -p port ] [ -d data ] \n");
    printf("Options:\n");
    printf("  -o owner\t\tthe owner of the configurations\n");
    printf("  -k key\t\tthe owner key\n");
    printf("  -a action\t\tcreate set get list, ");
    printf("(only 1st letter matters) \n\t\t\tavailable actions: \n");
    printf("\t\t\t  c: create owner\n");
    printf("\t\t\t  g: get configuration\n");
    printf("\t\t\t  S: get configuration stat\n");
    printf("\t\t\t  s: set configuration\n");
    printf("\t\t\t  l: list configurations\n");
    printf("  -c configuration\tthe name of the configuration\n");
    printf("  -h help\t\tdisplay this help\n");
    printf("  -n hostname\t\tthe server name (default is localhost)\n");
    printf("  -p port\t\tthe server port (default is 443)\n");
    printf("  -d data\t\tthe data to post (with set action)\n");
    printf("  \t\t\tif not specified will create a blank one\n");
    printf("Examples:\n");
    printf("  etcjs_cli -o john -k sesame -a c -n etc.gpisc.in\n");
    printf("\twill create the user john with password sesame on etc.gpisc.in\n");
    printf("  etcjs_cli -o john -k sesame -a s -c myconf -d blah -p 1337\n");
    printf("\twill set the configuration named myconf for user john\n");
    printf("\ton server running on localhost on port 1337\n");
    exit(1);
}
void parse_params(int argc, char** argv, params* p)
{
    char c;
    p->owner = p->key = p->action =
        p->configuration = p->host_name = p->data = 0;
    p->port = 80;
    while ((c = getopt(argc, argv, "o:k:a:c:hp:n:d:")) != -1)
    {
        switch(c)
        {
            case 'o': p->owner = optarg; break;
            case 'k': p->key = optarg; break;
            case 'a': p->action = optarg; break;
            case 'c': p->configuration = optarg; break;
            case 'n': p->host_name = optarg; break;
            case 'd': p->data = optarg; break;
            case 'p': p->port = atoi(optarg); break;
            case 'h':
            default: help();
        }
    }
    if((p->owner && !p->key) || (p->host_name && !p->port) || !p->action)
        help();
}
etcjs_result* do_action(params* p)
{
    etcjs_owner* owner;
    etcjs_result* result;
    if(!p->host_name) etcjs_init_from_configuration();
    else etcjs_init(p->host_name, p->port);
    owner = p->owner ? etcjs_owner_new(p->owner, p->key)
        : etcjs_owner_new_from_configuration();
    switch(p->action[0])
    {
        case 'c': result = etcjs_owner_create(owner); break;
        case 'g': result = etcjs_config_get(owner, p->configuration); break;
        case 'S': result = etcjs_config_stat(owner, p->configuration); break;
        case 's': result = etcjs_config_set(owner, p->configuration,
                          p->data == 0?"":p->data, 1, 0, 0); break;
        case 'l': result = etcjs_config_list(owner); break;
        default: help();
    }
    etcjs_owner_delete(owner);
    etcjs_cleanup();
    return result;
}
void display_result(etcjs_result* result)
{
    switch(result->type)
    {
        case ETCJS_ERROR: printf("error %s", (char*) result->result); break;
        case ETCJS_LIST:
        case ETCJS_CONTENT: printf("%s", (char*) result->result); break;
    }
    etcjs_result_delete(result);
}
int main(int argc, char** argv)
{
    params p;
    parse_params(argc, argv, &p);
    display_result(do_action(&p));
    return 0;
}
