#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "etcjs.h"
static const char *etcjs_str = "Hello World!\n";
static const char *etcjs_path = "/etcjs";
etcjs_owner* etcjs_fuse_setup()
{
    etcjs_init("localhost", 1337);
    return etcjs_owner_new("yazgoo", "foo");
}
void etcjs_fuse_teardown(etcjs_owner* owner, etcjs_result* result)
{
    etcjs_result_delete(result);
    etcjs_owner_delete(owner);
    etcjs_cleanup();
}
static int etcjs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    memset(stbuf, 0, sizeof(struct stat));
    if(strcmp(path, "/") == 0)
    {
        stbuf->st_mode = S_IFDIR | 0755;
        stbuf->st_nlink = 2;
    }
    else
    {
        etcjs_owner* owner;
        etcjs_result* result;
        owner = etcjs_fuse_setup();
        result = etcjs_config_stat(owner, (char*) path + 1);
        etcjs_type type = result->type;
        etcjs_fuse_teardown(owner, result);
        if(type != ETCJS_ERROR)
        {
            stbuf->st_mode = S_IFREG | 0444;
            stbuf->st_nlink = 1;
            stbuf->st_size = strlen(etcjs_str);
        }
        else res = -ENOENT;
    }
    return res;
}
static int etcjs_readdir(const char *path, void *buf, fuse_fill_dir_t filler,
        off_t offset, struct fuse_file_info *fi)
{
    (void) offset;
    (void) fi;
    /*if(strcmp(path, "/") != 0)
        return -ENOENT;*/
    filler(buf, ".", NULL, 0);
    filler(buf, "..", NULL, 0);
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_list(owner);
    char* c, *line, *start = result->result;
    for(c = result->result; *c; )
        if(*c == '\n')
        {
            size_t size = c - start + 1;
            line = (char*) calloc(size, sizeof(char));
            memcpy(line, start, size);
            line[size-1] = 0;
            filler(buf, line, NULL, 0);
            free(line);
            start = c++;
        } 
        else c++;
    etcjs_fuse_teardown(owner, result);
    return 0;
}
static int etcjs_open(const char *path, struct fuse_file_info *fi)
{
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_stat(owner, (char*) path + 1);
    etcjs_type type = result->type;
    etcjs_fuse_teardown(owner, result);
    return type == ETCJS_ERROR ? -ENOENT : 0;
}
static int etcjs_read(const char *path, char *buf, size_t size, off_t offset,
                      struct fuse_file_info *fi)
{
    size_t len;
    (void) fi;
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_get(owner, (char*) path + 1);
    if(result->type == ETCJS_ERROR)
    {
        etcjs_fuse_teardown(owner, result);
        return -ENOENT;
    }
    len = strlen(result->result) / 2;
    if (offset < len)
    {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, result->result + offset, size);
        buf[size - 1] = 0;
    }
    else size = 0;
    etcjs_fuse_teardown(owner, result);
    return size;
}
static int etcjs_write(const char* path, const char* buff,
        size_t size, off_t offset,struct fuse_file_info *fi)
{
    return 0;
}

static struct fuse_operations etcjs_oper =
{
    .getattr = etcjs_getattr,
    .readdir = etcjs_readdir,
    .open = etcjs_open,
    .read = etcjs_read,
    .write = etcjs_write,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &etcjs_oper);
}
