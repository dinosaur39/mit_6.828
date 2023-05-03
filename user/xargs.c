#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/param.h"
#include "user/user.h"

#define MAXLINE 1024

int get_argv(char **argv, char *line)
{
    int argc = 0;
    char *line_pos = line;

    while (1) {
        while (*line_pos == ' ') {
            *line_pos = '\0';
            line_pos++;
        }
        if (*line_pos == '\0')
            break;
        argv[argc] = line_pos;
        while (1) {
            if (*line_pos == '\0' || *line_pos == ' ') {
                argc++;
                break;
            }
            line_pos++;
        }
    }

    argv[argc] = "";
    return argc;
}

int readline(char *line_buf) {
    int cnt = 0;
    char *p = line_buf;
    while (read(0, p, 1) > 0 && *p != '\n') {
        p++;
        cnt++;
    }
    *p = '\0';
    return cnt - 1;
}

int
main(int argc, char *argv[])
{

    int i;
    char *new_argv[MAXARG];
    char argv_line[MAXLINE];

    for (i = 0; i < argc - 1; i++) {
        new_argv[i] = argv[i + 1];
    }
    while (readline(argv_line) > 0) {
        if (fork() == 0) {
            get_argv(new_argv + (argc - 1), argv_line);
            exec(new_argv[0], new_argv);
        }
        wait(&i);
    }
    exit(0);
}