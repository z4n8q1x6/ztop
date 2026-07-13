#ifndef CPU_H
#define CPU_H

typedef struct {
  char model_name[128];
  int nb_cores;
  _Atomic long long usage;
} Cpu;

void cpu_info(Cpu *cpu);
void *cpu_usage(void *usage);
void display_cpu(Cpu *cpu);

#endif // CPU_H
