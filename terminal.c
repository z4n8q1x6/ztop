#include "terminal.h"
#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void init_terminal() {
  struct termios t;
  if (tcgetattr(STDIN_FILENO, &t) == -1) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }
  t.c_lflag &= ~(ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &t) != 0) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }
}

void handle_input() {
  char c;
  ssize_t n = read(STDIN_FILENO, &c, 1);
  if (n == -1) {
    perror("read");
    return;
  } else if (n == 0) {
    return;
  }
  if (c == 'q') {
    reset_terminal();
    exit(EXIT_SUCCESS);
  }
}

void reset_terminal() {
  struct termios orig;
  if (tcgetattr(STDIN_FILENO, &orig) == -1) {
    perror("tcgetattr");
    exit(EXIT_FAILURE);
  }
  orig.c_lflag |= (ICANON | ECHO);
  if (tcsetattr(STDIN_FILENO, TCSANOW, &orig) != 0) {
    perror("tcsetattr");
    exit(EXIT_FAILURE);
  }
}
