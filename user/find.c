#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/fs.h"

void find_cwd(char *dir_path, char *name)
{
    char buf[512], *p;
    struct dirent de;
    struct stat st;
    int fd;

    if ((fd = open(dir_path, 0)) < 0) {
        fprintf(2, "find: cannot open %s\n", dir_path);
        return;
    }
    if (fstat(fd, &st) < 0) {
        fprintf(2, "find: cannot stat %s\n", dir_path);
        close(fd);
        return;
    }
    
    switch (st.type) {
    case T_DEVICE:
    case T_FILE:
        fprintf(2, "find: %s is not a directory path\n", dir_path);
        break;
    
    case T_DIR:
        if(strlen(dir_path) + 1 + DIRSIZ + 1 > sizeof buf){
            printf("ls: path too long\n");
            break;
        }
        strcpy(buf, dir_path);
        p = buf+strlen(buf);
        *p++ = '/';
        while (read(fd, &de, sizeof(de)) == sizeof(de)) {
            if(de.inum == 0)
                continue;
            memmove(p, de.name, DIRSIZ);
            p[DIRSIZ] = 0;
            if (!strcmp(name, p))
                printf("%s\n", buf);
            if (!strcmp(de.name, ".") || !strcmp(de.name, ".."))
                continue;
            stat(buf, &st);
            if (st.type == T_DIR)
                find_cwd(buf, name);
        }
        break;
    }
    close(fd);
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        fprintf(2, "usage: %s <dir> [<name>]\n", argv[0]);
        exit(0);
    }
    else if (argc == 2)
    {
        argv[0] = "ls";
        exec("/ls", argv);
    }
    find_cwd(argv[1], argv[2]);
    exit(0);
}