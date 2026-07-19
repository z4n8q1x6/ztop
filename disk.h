#ifndef DISK_H
#define DISK_H

typedef struct {
  unsigned long long total;
  unsigned long long available;
  unsigned long long free;
  unsigned long long privileged;
  unsigned long long used;
  int usage;
} Disk;

#define BYTE_TO_GIB(NUMBER) ((NUMBER) / 1073741824.0l)

int init_disk(_Atomic Disk *disk);
void *disk_thread(void *arg);
void print_disk(Disk *disk);

#endif // DISK_H
