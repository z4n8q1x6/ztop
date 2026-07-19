#include "disk.h"
#include "util.h"
#include <math.h>
#include <stdio.h>
#include <sys/statvfs.h>
#include <unistd.h>

static int read_disk(Disk *disk);

int init_disk(_Atomic Disk *disk) {
  Disk disk_snap;
  if (read_disk(&disk_snap)) {
    *disk = disk_snap;
    return 1;
  }
  return 0;
}

static int read_disk(Disk *disk) {
  struct statvfs buf;
  if (statvfs("/", &buf) == -1)
    return 0;
  disk->free = buf.f_bfree * buf.f_bsize;
  disk->available = buf.f_bavail * buf.f_bsize;
  disk->privileged = disk->free - disk->available;
  disk->total = (buf.f_blocks * buf.f_frsize);

  disk->used = disk->total - disk->available;
  disk->usage = (disk->used * 100) / disk->total;
  return 1;
}

void *disk_thread(void *arg) {
  Disk disk;
  while (1) {
    if (read_disk(&disk)) {
      *(Disk *)arg = disk;
    }
    sleep(5);
  }
  return NULL;
}

void print_disk(Disk *disk) {
  printf("--------------------- DISK ---------------------\n");
  printf("Total memory: %.0lfG\n", round(BYTE_TO_GIB((double)disk->total)));
  printf("Avaialable memory: %.0lfG\n",
         round(BYTE_TO_GIB((double)disk->available)));
  printf("Free memory: %.0lfG\n", round(BYTE_TO_GIB((double)disk->free)));
  printf("Used memory: %.0lfG\n", round(BYTE_TO_GIB((double)disk->used)));
  printf("Usage: %d%%\n", disk->usage);
  print_usage_bar(disk->usage);
}
