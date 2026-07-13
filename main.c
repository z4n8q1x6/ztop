#include "cpu.h"
#include "terminal.h"
#include "util.h"
#include <bits/posix2_lim.h>
#include <bits/time.h>
#include <limits.h>
#include <poll.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/poll.h>
#include <time.h>
#include <unistd.h>

void handle_sigint(int sig) {
  reset_terminal();
  exit(EXIT_SUCCESS);
}

int main() {
  signal(SIGINT, handle_sigint);
  setup_terminal();
  struct pollfd fds[1];
  fds[0].events = POLLIN;
  fds[0].fd = STDIN_FILENO;
  nfds_t nfds = 1;
  struct timespec first;
  struct timespec last;
  unsigned long long elapsed;
  unsigned long long timeout =
      33333333; // 30fps ( 1 refresh each 1/30s = (1/30)*10^9 in nanosec)

  unsigned long long remaining;

  Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
  cpu_info(&cpu);
  pthread_t cpu_thread;
  if (pthread_create(&cpu_thread, NULL, cpu_usage, &cpu.usage) == -1) {
    perror("pthread_create");
    return 1;
  }

  clock_gettime(CLOCK_MONOTONIC, &first);
  clock_gettime(CLOCK_MONOTONIC, &last);
  while (1) {
    elapsed = (last.tv_sec - first.tv_sec) * 1000000000ULL +
              (last.tv_nsec - first.tv_nsec);
    remaining = (timeout - elapsed) / 1000000ULL;
    if (remaining < 0)
      remaining = 0;
    int retval = poll(fds, 1, (int)remaining);
    if (retval > 0) {
      if (fds[0].revents & POLLIN) {
        handle_input();
      }
    }
    clock_gettime(CLOCK_MONOTONIC, &first);
    printf("\033[2J\033[H");
    display_cpu(&cpu);
    draw_usage_bar(cpu.usage);
    clock_gettime(CLOCK_MONOTONIC, &last);
  }
  return 0;
}

// Ram: (number + percentage)
// * L’espace total du disque principal ; -> memtotal
// * L’espace disponible(avaiablable) ; -> memAvailable
// * L’espace disponible(free); -> memFree
// * utilisation du disque. memtotal - avaialable
