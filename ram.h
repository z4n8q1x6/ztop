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

// free = space that is strictly empty and untouched
// available = free space + space filled by temporary caches that linux may put
// to make access of disk faster for certain things to utilize unused space

int init_ram(_Atomic Ram *ram);
void *ram_thread(void *arg);
void print_ram(Ram *snap);

#endif // RAM_H
