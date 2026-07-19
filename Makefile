all:
	gcc main.c cpu.c util.c terminal.c ram.c disk.c -o ztop -lm
