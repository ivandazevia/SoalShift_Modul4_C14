#ifdef linux
//For pread()/pwrite() 
#define _XOPEN_SOURCE 500
#endif
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/statfs.h>

static const char *dirpath = "/home/zevi/Downloads"; //Destinasi folder yang akan di mountkan

char globalpath[1000];

static int c14_getattr(const char *path, struct stat *stbuf)
{
    int res;

    char fpath [1000];
    sprintf(fpath,"%s%s",dirpath,path);
    res = lstat(fpath, stbuf);
    if(res == -1)
        return -errno;

    return 0;
}


static int c14_getdir(const char *path, fuse_dirh_t h, fuse_dirfil_t filler)
{
    DIR *dp;
    struct dirent *de;
    int res = 0;
    char fpath [1000];
    sprintf(fpath,"%s%s",dirpath,path);

    dp = opendir(fpath);
    if(dp == NULL)
        return -errno;

    while((de = readdir(dp)) != NULL) {
        res = filler(h, de->d_name, de->d_type);
        if(res != 0)
            break;
    }

    closedir(dp);
    return res;
}

static int c14_mknod(const char *path, mode_t mode, dev_t rdev)
{
    int res;
    char fpath [1000];
    sprintf(fpath,"%s%s",dirpath,path);

    res = mknod(fpath, mode, rdev);
    if(res == -1)
        return -errno;

    return 0;
}

static int c14_rename(const char *from, const char *to)
{
    int res;
    
    char fpath [1000], ffrom[1000],fto[1000], dirpathbaru[1000], direktori[1000];
    sprintf(dirpathbaru,"mkdir -p /home/zevi/Downloads/simpanan");
    sprintf(direktori,"mkdir -p %s",dirpathbaru);
    system(direktori);
    
    sprintf(ffrom,"%s%s",dirpath,from);
    sprintf(fto,"%s%s",dirpathbaru,to);
    res = rename(ffrom, fto);
    char copy[1000];
    sprintf(copy,"cp %s %s",ffrom,fto);
    system(copy);
    if(res == -1)
        return -errno;

    return 0;
}

static int c14_read(const char *path, char *buf, size_t size, off_t offset)
{
    int fd;
    int res;

    char fpath [1000];
    sprintf(fpath,"%s%s",dirpath,path);
    fd = open(fpath, O_RDONLY);
    if(fd == -1)
        return -errno;

    strcpy(globalpath,fpath);

    res = pread(fd, buf, size, offset);
    if(res == -1)
        res = -errno;
    
    close(fd);
    return res;
}

static int c14_write(const char *path, const char *buf, size_t size, off_t offset)
{
    int fd;
    int res;

    chmod(globalpath,0000);
    char fpath [1000];
    sprintf(fpath,"%s%s",dirpath,path);
    fd = open(fpath, O_WRONLY);
    if(fd == -1)
        return -errno;

    res = pwrite(fd, buf, size, offset);
    if(res == -1)
        res = -errno;
    
    close(fd);
    return res;
}

static struct fuse_operations c14_oper = {
    .getattr    = c14_getattr,
    .getdir    = c14_getdir,
    .mknod    = c14_mknod,
    .rename    = c14_rename,
    .read    = c14_read,
    .write    = c14_write,
};

int main(int argc, char *argv[])
{
    fuse_main(argc, argv, &c14_oper);
    return 0;
}