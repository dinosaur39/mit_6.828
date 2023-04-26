#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

#define CHLD_PROC_CNT 5
#define DEFAULT_MAX_NUM 50
#define MIN_PRIME 2
#define WSIZE sizeof(int)

void sieve_chldprocess(int inputfd, int outputfd)
{
  int prime, num, end = 0;
  read(inputfd, &prime, WSIZE);
  while (prime > 0) {
    printf("prime %d\n", prime);

    read(inputfd, &num, WSIZE);
    while (num > 0) {
      if (num % prime)
        write(outputfd, &num, WSIZE);

      read(inputfd, &num, WSIZE);
    }
    write(outputfd, &end, WSIZE);
    read(inputfd, &prime, WSIZE);
  }
  write(outputfd, &end, WSIZE);
}

void sieve_mainprocess(int inputfd, int outputfd, int max_num)
{
  int i, end = 0;
  for (i = MIN_PRIME; i <= max_num; i++) {
    write(outputfd, &i, WSIZE);
  }
  write(outputfd, &end, WSIZE);

  sieve_chldprocess(inputfd, outputfd);
}

int
main(int argc, char *argv[])
{
  int max = DEFAULT_MAX_NUM, i;
  int readpipe[2], writpipe[2], inputfd, outputfd;

  if (argc == 2) {
    i = atoi(argv[1]);
    if (i >= 2)
      max = i;
  }

  pipe(readpipe);
  outputfd = dup(readpipe[1]);

  for(i = 0; i < CHLD_PROC_CNT; i++) {
    pipe(writpipe);
    if (fork() == 0) {
      close(readpipe[1]);
      close(writpipe[0]);
      inputfd = readpipe[0];
      outputfd = writpipe[1];

      sieve_chldprocess(inputfd, outputfd);

      close(inputfd);
      close(outputfd);
      exit(0);
    }

    close(readpipe[0]);
    close(readpipe[1]);
    readpipe[0] = writpipe[0];
    readpipe[1] = writpipe[1];
  }

  inputfd = writpipe[0];
  sieve_mainprocess(inputfd, outputfd, max);

  return 0;
}