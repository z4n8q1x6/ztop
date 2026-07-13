#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char **get_tokens(char *line) {
  int nb_tokens = 20;
  char **tokens = malloc(sizeof(*tokens) * nb_tokens);
  if (tokens == NULL) {
    perror("malloc");
    return NULL;
  }
  int len = strlen(line);
  int count = 0;
  int inside_word = 0;
  for (int i = 0; i < len; i++) {
    if (line[i] != ' ' && !inside_word) {
      if (count >= nb_tokens - 1) {
        nb_tokens *= 2;
        char **tmp = realloc(tokens, sizeof(*tokens) * nb_tokens);
        if (tmp == NULL) {
          perror("realloc");
          free(tokens);
          return NULL;
        }
        tokens = tmp;
      }
      tokens[count++] = &line[i];
      inside_word = 1;
    } else if (line[i] == ' ' && inside_word) {
      line[i] = '\0';
      inside_word = 0;
    }
  }
  tokens[count] = NULL;
  return tokens;
}

void draw_usage_bar(int usage) {
  int remaining = 100 - usage;
  if (usage > 100)
    usage = 100;
  if (usage < 0)
    remaining = 100;

  printf("[");
  for (int i = 0; i < usage; i++) {
    printf("#");
  }
  for (int i = 0; i < remaining; i++) {
    printf("-");
  }
  printf("]\n");
}
