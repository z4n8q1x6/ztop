#ifndef RAM_H
#define RAM_H

#include <stddef.h>

typedef struct {
  size_t total;
  size_t available;
  size_t free;
  size_t cached;
  size_t used;
} Ram;

#endif // RAM_H
