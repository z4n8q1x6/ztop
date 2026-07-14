#include "ram.h"
#include "util.h"
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

enum { TOTAL, FREE, AVAILABLE, CACHED };

void *ram_info_thread(void *arg) {
  Ram *ram = arg;
  FILE *meminfo = fopen("/proc/meminfo", "r");
  if (meminfo == NULL) {
    perror("fopen");
    return NULL;
  }
  char line[LINE_MAX];
  size_t n = 4;
  char ram_infos[n][13];
  strcpy(ram_infos[TOTAL], "MemTotal");
  strcpy(ram_infos[FREE], "MemFree");
  strcpy(ram_infos[AVAILABLE], "MemAvailable");
  strcpy(ram_infos[CACHED], "Cached");

  int fields_found = 0;
  while (1) {
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
            ram->total = tmp;
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
          break;
        }
      }
    }
    ram->used = ram->total - ram->available;
    sleep(1);
  }
  return NULL;
}

// int main() {
//   Ram ram;
//   pthread_t ram_thread;
//   pthread_create(&ram_thread, NULL, ram_info_thread, &ram);
//   return 0;
// }

// _Atomic(Ram *) for thread safe + double buffering to not free during read?
