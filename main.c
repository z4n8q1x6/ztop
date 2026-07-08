#include <stdio.h>
#include <string.h>

#define NB_CPU_PROPERTIES 2

int main() {
  FILE *cpu_info = fopen("/proc/cpuinfo", "r");
  if (cpu_info == NULL) {
    perror("fopen");
    return 1;
  }

  // format:
  // model name	: Intel(R) Core(TM) Ultra 7 155U
  // cpu cores	: 12
  // cpu MHz		: 1200.000
  char line[1024];
  int property_count = 0;
  char *properties[] = {"model name", "cpu cores"};
  while (fgets(line, sizeof(line), cpu_info) != NULL &&
         property_count < NB_CPU_PROPERTIES) {
    strtok(line, ":");
    for (int i = 0; i < NB_CPU_PROPERTIES; i++) {
      if (strncmp(properties[i], line, strlen(properties[i])) == 0) {
        printf("%s : %s", properties[i], strtok(NULL, ":"));
        property_count++;
      }
    }
  }
  // Le pourcentage CPU devra être calculé à partir de deux lectures successives
  // de `/proc/stat`.
  return 0;
}
