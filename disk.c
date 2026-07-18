#include <stdio.h>
#include <sys/statvfs.h>

int main() {
  // * L’espace total du disque principal ;
  // * L’espace utilisé ;
  // * L’espace disponible ;
  // * Le pourcentage d’utilisation du disque.
  struct statvfs buf;
  statvfs("/", &buf);
  unsigned long long free = buf.f_bfree * buf.f_bsize;
  unsigned long long available = buf.f_bavail * buf.f_bsize;
  unsigned long long priviligated = free - available;
  // unpriviliged
  unsigned long long total = (buf.f_blocks * buf.f_frsize);
  unsigned long long used = total - available;
  int usage = (used * 100 + total - 1) / total;
  printf("Total memory: %llu\n", total);
  printf("Available memory: %llu\n", available);
  printf("Used memory: %llu\n", used);
  printf("Usage: %d%%\n", usage);
  // printf("Priviligated %llu\n", priviligated);
  // printf("--------------\n");
  // printf("Filesystem block size      : %lu\n", buf.f_bsize);
  // printf("Fragment size              : %lu\n", buf.f_frsize);
  // printf("Total blocks               : %lu\n", buf.f_blocks);
  // printf("Free blocks                : %lu\n", buf.f_bfree);
  // printf("Avaialable blocks (uu      : %lu\n", buf.f_bavail);
  // printf("Total inodes               : %lu\n", buf.f_files);
  // printf("Free inodes                : %lu\n", buf.f_ffree);
  // printf("Free inodes (uu)           : %lu\n", buf.f_favail);
  // printf("Filesystem ID              : %lu\n", buf.f_fsid);
  // printf("Mount flags                : %lx\n", buf.f_flag);
  // printf("Maximum filename length    : %lu\n", buf.f_namemax);
  return 0;
}
