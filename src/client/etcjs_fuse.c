#include <fuse.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include "etcjs.h"
#define _GNU_SOURCE
#include <unistd.h>
#include <time.h>
#include <locale.h>
void get_uid_gid(struct stat *stbuf)
{
    static char read = 0;
    static uid_t uid;
    static gid_t gid;
    if(!read)
    {
        uid_t ruid, euid, suid;
        gid_t rgid, egid, sgid;
        getresuid(&ruid, &euid, &suid);
        getresgid(&rgid, &egid, &sgid);
        uid = ruid;
        gid = rgid;
        read = 1;
    }
    stbuf->st_uid = uid;
    stbuf->st_gid = gid;
}
etcjs_owner* etcjs_fuse_setup()
{
    etcjs_init_from_configuration();
    return etcjs_owner_new_from_configuration();
}
void etcjs_fuse_teardown(etcjs_owner* owner, etcjs_result* result)
{
    etcjs_result_delete(result);
    etcjs_owner_delete(owner);
    etcjs_cleanup();
}
void get_time(char* str, time_t* time)
{
    setlocale(LC_TIME, "EN");
    struct tm _tm;
    strptime(str, "%a %b %d %Y %H:%M:%S GMT%z", &_tm);
    *time = mktime(&_tm);
}
static int etcjs_getattr(const char *path, struct stat *stbuf)
{
    int res = 0;
    printf("getattr %s\n", path);
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
        if(result->type != ETCJS_ERROR)
        {
            char* c, * start;
            start = result->result;
            char section[64] = { 0 };
            char str[64] = { 0 };
            unsigned int value;
            for(c = start; *c ; c++)
            {
                if(*c != '\n') continue;
                *c = 0;
                if(sscanf(start, "%[^=]=%d", section, &value) != 2)
                    if(sscanf(start, "%[^=]=%[^\t\n]", section, str))
                    {
                        if(!strcmp(section, "atime"))
                        {
                            get_time(str, &stbuf->st_atime);
                            stbuf->st_ctime = stbuf->st_mtime = stbuf->st_atime;
                        }
                        else if(!strcmp(section, "ctime"))
                            get_time(str, &stbuf->st_ctime);
                        else if(!strcmp(section, "mtime"))
                            get_time(str, &stbuf->st_mtime);
                        continue;
                    }
                if(!strcmp(section, "mode")) stbuf->st_mode = value;
                else if(!strcmp(section, "size")) stbuf->st_size = value;
                else if(!strcmp(section, "nlink")) stbuf->st_nlink = value;
                else if(!strcmp(section, "blocks")) stbuf->st_blocks = value;
                else if(!strcmp(section, "blksize")) stbuf->st_blksize = value;
                else if(!strcmp(section, "rdev")) stbuf->st_rdev = value;
                start = c + 1;
            }
            get_uid_gid(stbuf);
        }
        else res = -ENOENT;
        etcjs_fuse_teardown(owner, result);
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
            start = ++c;
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
    if(type == ETCJS_ERROR) return -ENOENT;
    fi->fh = 1;
    return 0;
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
    len = strlen(result->result);
    if (offset < len)
    {
        if (offset + size > len)
            size = len - offset;
        memcpy(buf, result->result + offset, size);
    }
    else size = 0;
    etcjs_fuse_teardown(owner, result);
    return size;
}
static int etcjs_write(const char* path, const char* buff,
        size_t size, off_t offset, struct fuse_file_info *fi)
{
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_set(owner, (char*) path + 1, buff, 0, offset, size);
    etcjs_type type = result->type;
    etcjs_fuse_teardown(owner, result);
    return type == ETCJS_ERROR ? -ENOENT : size;
}
static int etcjs_mknod(const char* path, mode_t mode, dev_t dev)
{
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_touch(owner, (char*) path);
    etcjs_type type = result->type;
    etcjs_fuse_teardown(owner, result);
    return type == ETCJS_ERROR ? -ENOENT : 0;
}
static int etcjs_unlink(const char* path, mode_t mode, dev_t dev)
{
    etcjs_owner* owner;
    etcjs_result* result;
    owner = etcjs_fuse_setup();
    result = etcjs_config_delete(owner, (char*) path);
    etcjs_type type = result->type;
    etcjs_fuse_teardown(owner, result);
    return type == ETCJS_ERROR ? -ENOENT : 0;
}
int etcjs_flush(const char* path, struct fuse_file_info* fi)
{
    return 0;
}
static int etcjs_utime(const char* path, struct utimbuf* time)
{
    return 0;
}
static int etcjs_chmod(const char* path, mode_t mode) 
{
    return 0;
}
static int etcjs_chown(const char* path, uid_t uid, gid_t gid) 
{
    return 0;
}
static int etcjs_truncate(const char* path, off_t offset) 
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
    .flush = etcjs_flush,
    .mknod = etcjs_mknod,
    .unlink = etcjs_unlink,
    .utime = etcjs_utime,
    .chmod = etcjs_chmod,
    .chown = etcjs_chown,
    .truncate = etcjs_truncate,
};

int main(int argc, char *argv[])
{
    return fuse_main(argc, argv, &etcjs_oper);
}
