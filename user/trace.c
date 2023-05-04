#include "kernel/types.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
    int watch;

    if (argc < 3) {
        fprintf(2, "usage: %s <number> <command> [...args]\n", argv[0]);
        exit(0);
    }
    watch = atoi(argv[1]);

    trace(watch);
    exec(argv[2], argv + 2);
    exit(0);
}


