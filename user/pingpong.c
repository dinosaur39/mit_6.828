#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define DEFAULT_CHAR 'p'

int
main(int argc, char *argv[])
{
  int fstp[2], sndp[2], pid;
  char c, sendchar;

  if (argc < 2) {
    sendchar = DEFAULT_CHAR;
  } else {
    sendchar = argv[1][0];
  }


  if (pipe(fstp) < 0 || pipe(sndp) < 0) {
    printf("pingpong: pipe failed\n");
    exit(1);
  }
  pid = fork();
  if (pid == 0) {//child process
    sleep(10);
    read(fstp[0], &c, 1);
    printf("%d: received ping %c\n", getpid(), c);
    write(sndp[1], &c, 1);
    exit(0);
  } else {
    write(fstp[1], &sendchar, 1);
    sleep(20);
    read(sndp[0], &c, 1);
    printf("%d: received pong %c\n", getpid(), c);
    exit(0);
  }

}