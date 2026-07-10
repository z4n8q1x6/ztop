#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NB_CPU_PROPERTIES 2

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

void *cpu_usage(void *usage) {
  unsigned long long active_t1, active_t2, idle_t1, idle_t2, active, idle;
  unsigned long long *usage_ptr = (unsigned long long *)usage;
  char **tokens;
  char line[LINE_MAX];
  FILE *cpu_stat;

  while (1) {
    cpu_stat = fopen("/proc/stat", "r");
    if (cpu_stat == NULL) {
      perror("fopen");
      return NULL;
    }
    if (fgets(line, sizeof(line), cpu_stat) != NULL) {
      tokens = get_tokens(line);
      if (tokens == NULL) {
        fprintf(stderr, "get_tokens\n");
        return NULL;
      }
      active_t1 = atol(tokens[1]) + atol(tokens[2]) + atol(tokens[3]) +
                  atol(tokens[6]) + atol(tokens[8]);
      idle_t1 = atol(tokens[4]) + atol(tokens[5]);
      free(tokens);
      fclose(cpu_stat);
    }

    sleep(1);

    cpu_stat = fopen("/proc/stat", "r");
    if (cpu_stat == NULL) {
      perror("fopen");
    }
    if (fgets(line, sizeof(line), cpu_stat) != NULL) {
      tokens = get_tokens(line);
      if (tokens == NULL) {
        fprintf(stderr, "get_tokens\n");
      }
      active_t2 = atol(tokens[1]) + atol(tokens[2]) + atol(tokens[3]) +
                  atol(tokens[6]) + atol(tokens[8]);
      idle_t2 = atol(tokens[4]) + atol(tokens[5]);
      active = active_t2 - active_t1;
      idle = idle_t2 - idle_t1;
      *usage_ptr = (active * 100) / (active + idle);
      free(tokens);
      fclose(cpu_stat);
    }
  }
  return NULL;
}

void cpu_info() {
  FILE *cpu_info = fopen("/proc/cpuinfo", "r");
  if (cpu_info == NULL) {
    perror("fopen");
  }
  char line[LINE_MAX];
  int property_count = 0;
  char *properties[] = {"model name", "cpu cores"};
  char *saveptr;

  while (fgets(line, sizeof(line), cpu_info) != NULL &&
         property_count < NB_CPU_PROPERTIES) {
    char *key = strtok_r(line, ":", &saveptr);
    char *value = strtok_r(NULL, ":", &saveptr);
    if (key == NULL || value == NULL) {
      perror("strtok_r");
    }
    for (int i = 0; i < NB_CPU_PROPERTIES; i++) {
      if (strncmp(properties[i], key, strlen(properties[i])) == 0) {
        printf("%s : %s", properties[i], value);
        property_count++;
      }
    }
  }

  fclose(cpu_info);
}
