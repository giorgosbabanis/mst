GCC = gcc
FLAGS = -std=gnu11 -Wfatal-errors -lnuma -fopenmp -lm -Wformat


mst: alg3_mastiff.c aux.c cc.c graph.c msf.c omp.c partitioning.c relabel.c trans.c
	$(CC) alg3_mastiff.c -o mst.exe $(FLAGS)

convert: graphTextHandler.c
	$(CC) graphTextHandler.c -o convert.exe

creator: graphCreator.c
	$(CC) graphCreator.c -o creator.exe