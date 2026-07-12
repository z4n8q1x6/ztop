#include "cpu.h"
#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

int main() {

  Cpu cpu = {.usage = 0, .nb_cores = 0, .model_name = {0}};
  cpu_info(&cpu);
  // pthread_t cpu_thread;
  // if (pthread_create(&cpu_thread, NULL, cpu_usage, &cpu.usage) == -1) {
  //   perror("pthread_create");
  //   return 1;
  // }

  struct termios t;

  // 1. Get the current terminal settings from STDIN
  if (tcgetattr(STDIN_FILENO, &t) == -1) {
    perror("tcgetattr failed");
    return 1;
  }

  t.c_cflag &= ~ICANON;
  if (tcsetattr(STDIN_FILENO, TCSANOW, &t)) {
    printf("i wanted funny\n");
    return 1;
  }
  while (1) {
    char c = getchar();
    if (c)
      printf("you typed: %c\n", c);
  }
  return 1;

  while (1) {
    printf("\033[2J\033[H");
    printf("-------------------------------------------------------------------"
           "---------\n");

    printf("\033[34m");
    printf("Cpu Model : %s\n", cpu.model_name);
    printf("Cpu Cores : %d\n", cpu.nb_cores);
    printf("\033[0m");
    if (cpu.usage <= 33) {
      printf("\033[32m"
             "Cpu Usage : %llu%%\n",
             cpu.usage);
    } else if (cpu.usage > 33 && cpu.usage < 67) {
      printf("\033[33m"
             "Cpu Usage : %llu%%\n",
             cpu.usage);
    } else {
      printf("\033[31m"
             "Cpu Usage : %llu%%\n",
             cpu.usage);
    }
    printf("\033[0m");
    printf("-------------------------------------------------------------------"
           "---------\n");
    sleep(1);
  }
  return 0;
}

// Do:
// Disable canonical mode
// Disable echo
// Read 1 char
// Restore terminal on exit

// q->quit
// signal interupt handle
// cpu usage bar
// ram
