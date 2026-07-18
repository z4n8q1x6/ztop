#ifndef RAM_H
#define RAM_H

#include <stddef.h>

typedef struct {
  size_t total;
  size_t available;
  size_t free;
  size_t cached;
  size_t used;
  int usage;
} Ram;

int init_ram(Ram *ram);
void *ram_thread(void *arg);
void print_ram(Ram *snap);

#endif // RAM_H
