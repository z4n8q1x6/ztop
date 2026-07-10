#include "cpu.h"
#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>

int main() {

  // active : user : 1,
  //     nice : 2, system : 3, iqr : 6, steal : 8
  //     not active : idle : 4, iowait : 5
  //
  pthread_t cpu_thread;
  int thread_id;
  unsigned long long c_usage;
  pthread_create(&cpu_thread, NULL, cpu_usage, &c_usage);
  while (1) {
    printf("\033[2J\033[H");
    printf("-------------------------------------------------------------------"
           "---------\n");
    cpu_info();
    printf("usage = %lld%%\n", c_usage);
    printf("-------------------------------------------------------------------"
           "---------\n");
    sleep(1);
  }

  return 0;
}
