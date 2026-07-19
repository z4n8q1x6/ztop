#include "ram.h"
#include "util.h"
#include <limits.h>
#include <pthread.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { TOTAL, FREE, AVAILABLE, CACHED };
static const char ram_infos[4][13] = {"MemTotal", "MemFree", "MemAvailable",
                                      "Cached"};
static const size_t n_ram_infos = sizeof(ram_infos) / sizeof(ram_infos[0]);
static int read_ram(Ram *ram);

int init_ram(_Atomic Ram *ram) {
  Ram ram_snap;
  if (read_ram(&ram_snap)) {
    *ram = ram_snap;
    return 1;
  }
  return 0;
}

static int read_ram(Ram *ram) {
  char line[LINE_MAX];
  int fields_found = 0;
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if (meminfo == NULL) {
    perror("fopen");
    return 0;
  }
  while (fgets(line, sizeof(line), meminfo) != NULL &&
         fields_found < n_ram_infos) {
    for (int i = 0; i < n_ram_infos; i++) {
      if (strncmp(line, ram_infos[i], strlen(ram_infos[i])) == 0) { // check
        char *colon = strchr(line, ':');
        if (colon == NULL)
          continue;

        char *val = ++colon;
        unsigned long long tmp = strtoull(val, NULL, 0);
        switch (i) {
        case TOTAL:
          ram->total = tmp;
          break;
        case AVAILABLE:
          ram->available = tmp;
          break;
        case FREE:
          ram->free = tmp;
          break;
        case CACHED:
          ram->cached = tmp;
          break;
        }
        fields_found++;
      }
    }
  }
  ram->used = ram->total - ram->available;
  if (ram->total != 0)
    ram->usage = (ram->used * 100) / ram->total;
  else
    ram->usage = 0;
  fclose(meminfo);
  return 1;
}

void *ram_thread(void *arg) {
  Ram ram;

  while (1) {
    if (read_ram(&ram)) {
      *(_Atomic Ram *)arg = ram;
    }
    sleep(1);
  }
  return NULL;
}

void print_ram(Ram *snap) {
  printf("--------------------- RAM ---------------------\n");
  printf("Total: %zu\n", snap->total);
  printf("Available: %zu\n", snap->available);
  printf("Free : %zu\n", snap->free);
  printf("Cached: %zu\n", snap->cached);
  printf("Used: %zu\n", snap->used);
  printf("Usage: %d%%\n", snap->usage);
  print_usage_bar(snap->usage);
}
