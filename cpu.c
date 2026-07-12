#include "cpu.h"
#include "util.h"
#include <bits/posix2_lim.h>
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define NB_CPU_PROPERTIES 2

enum {
  USER = 1,
  NICE = 2,
  SYSTEM = 3,
  IDLE = 4,
  IOWAIT = 5,
  IRQ = 6,
  SOFTIRQ = 7,
  STEAL = 8
};

static int get_current_cpu_ticks(unsigned long long *active,
                                 unsigned long long *idle) {
  FILE *cpu_stat = fopen("/proc/stat", "r");
  char line[LINE_MAX];
  if (cpu_stat == NULL) {
    perror("fopen");
    return 0;
  }
  if (fgets(line, sizeof(line), cpu_stat) != NULL) {
    char **tokens = get_tokens(line);
    if (tokens == NULL || tokens[8] == NULL) {
      fprintf(stderr, "get_tokens\n");
      return -1;
    }
    *active = strtoll(tokens[USER], NULL, 0) + strtoll(tokens[NICE], NULL, 0) +
              strtoll(tokens[SYSTEM], NULL, 0) + strtoll(tokens[IRQ], NULL, 0) +
              strtoll(tokens[STEAL], NULL, 0) +
              strtoll(tokens[SOFTIRQ], NULL, 0);
    *idle = strtoll(tokens[IDLE], NULL, 0) + strtoll(tokens[IOWAIT], NULL, 0);
    free(tokens);
    fclose(cpu_stat);
    return 1;
  }
  return 0;
}

void *cpu_usage(void *usage) {
  unsigned long long active_t1, active_t2, idle_t1, idle_t2, active, idle;
  _Atomic unsigned long long *usage_ptr = usage;
  char **tokens;
  char line[LINE_MAX];

  if (!get_current_cpu_ticks(&active_t1, &idle_t1))
    return NULL;
  while (1) {
    sleep(1);
    if (get_current_cpu_ticks(&active_t2, &idle_t2)) {
      active = active_t2 - active_t1;
      idle = idle_t2 - idle_t1;
      if (active != 0) {
        *usage_ptr = (active * 100) / (active + idle);
      } else {
        *usage_ptr = 0;
      }
      active_t1 = active_t2;
      idle_t1 = idle_t2;
    } else {
      return NULL;
    }
  }
  return NULL;
}

void cpu_info(Cpu *cpu) {
  FILE *file = fopen("/proc/cpuinfo", "r");
  if (file == NULL) {
    perror("fopen /proc/cpuinfo");
    return;
  }

  char line[LINE_MAX];
  int fields_found = 0;
  while (fgets(line, sizeof(line), file) != NULL &&
         fields_found < NB_CPU_PROPERTIES) {
    char *colon = strchr(line, ':');
    if (colon == NULL) {
      continue;
    }

    if (cpu->model_name[0] == '\0' && strstr(line, "model name") != NULL) {
      char *val = colon + 1;

      // trim
      while (*val == ' ' || *val == '\t')
        val++;
      val[strcspn(val, "\n")] = '\0';

      strncpy(cpu->model_name, val, sizeof(cpu->model_name) - 1);
      fields_found++;
    }

    else if (cpu->nb_cores == 0 && strstr(line, "cpu cores") != NULL) {
      cpu->nb_cores = atoi(colon + 1);
      fields_found++;
    }
  }

  fclose(file);
}
