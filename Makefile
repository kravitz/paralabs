.PHONY: all clean

all: task_01 task_02 task_03 task_04 task_05 task_06 task_07 task_07_worker task_08 task_08_worker

task_01: task_01.c
	gcc -pthread task_01.c -o task_01

task_02: task_02.c
	gcc -pthread task_02.c -o task_02

task_03: task_03.c
	gcc -pthread task_03.c -o task_03

task_04: task_04.c
	gcc -fopenmp task_04.c -o task_04

task_05: task_05.c quicksort.c quicksort.h ctest.h generic_sequence.c generic_sequence.h
	gcc -std=c99 -fopenmp -g task_05.c quicksort.c generic_sequence.c -o task_05

task_06: task_06.c mergesort.c mergesort.h ctest.h generic_sequence.c generic_sequence.h
	gcc -std=c99 -fopenmp -g task_06.c mergesort.c quicksort.c generic_sequence.c -o task_06

task_07: task_07.c matrix.h matrix.c messages.h
	mpicc -std=c99 -g -lm task_07.c matrix.c -o task_07

task_07_worker: task_07_worker.c matrix.h matrix.c messages.h
	mpicc -std=c99 -g task_07_worker.c matrix.c -o task_07_worker 

task_08: task_08.c matrix.h matrix.c maze.h
	mpicc -std=c99 -g task_08.c -o task_08

task_08_worker: task_08_worker.cpp matrix.h matrix.c maze.h
	mpic++ -g task_08_worker.cpp -o task_08_worker

clean:
	rm -f task_01 task_02 task_03 task_04 task_05 task_06 task_07 task_07_worker
