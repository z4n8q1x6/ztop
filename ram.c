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

void *ram_info_thread(void *arg) {
  Ram ram;
  char line[LINE_MAX];
  size_t n = 4;
  char ram_infos[n][13];
  int fields_found = 0;
  FILE *meminfo;

  strcpy(ram_infos[TOTAL], "MemTotal");
  strcpy(ram_infos[FREE], "MemFree");
  strcpy(ram_infos[AVAILABLE], "MemAvailable");
  strcpy(ram_infos[CACHED], "Cached");
  while (1) {
    meminfo = fopen("/proc/meminfo", "r");
    if (meminfo == NULL) {
      perror("fopen");
      return NULL;
    }
    while (fgets(line, sizeof(line), meminfo) != NULL && fields_found < n) {
      for (int i = 0; i < n; i++) {
        if (strncmp(line, ram_infos[i], strlen(ram_infos[i])) == 0) {
          char *colon = strchr(line, ':');
          if (colon == NULL)
            continue;

          char *val = ++colon;
          unsigned long long tmp = strtoull(val, NULL, 0);
          switch (i) {
          case TOTAL:
            ram.total = tmp;
            break;
          case AVAILABLE:
            ram.available = tmp;
            break;
          case FREE:
            ram.free = tmp;
            break;
          case CACHED:
            ram.cached = tmp;
            break;
          }
          fields_found++;
        }
      }
    }
    ram.used = ram.total - ram.available;
    *(_Atomic Ram *)arg = ram;
    fields_found = 0;
    fclose(meminfo);
    sleep(1);
  }
  return NULL;
}

void display_ram(Ram *snap) {
  printf("--------------------- RAM ---------------------\n");
  printf("Total: %zu\n", snap->total);
  printf("Available: %zu\n", snap->available);
  printf("Free : %zu\n", snap->free);
  printf("Cached: %zu\n", snap->cached);
  printf("Used: %zu\n", snap->used);
}
