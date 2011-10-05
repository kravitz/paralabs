.PHONY: all clean

all: task_01 task_02 task_03 task_04 task_05

task_01: task_01.c
	gcc -pthread task_01.c -o task_01

task_02: task_02.c
	gcc -pthread task_02.c -o task_02

task_03: task_03.c
	gcc -pthread task_03.c -o task_03

task_04: task_04.c
	gcc -fopenmp task_04.c -o task_04

task_05: task_05.c quicksort.c quicksort.h ctest.h
	gcc -std=c99 -fopenmp -g task_05.c quicksort.c -o task_05

clean:
	rm -f *.o task_01 task_02 task_03 task_04 task_05
