GCC = gcc
FLAGS = -std=c99 -Wfatal-errors -lnuma -fopenmp -lm -Wformat

all: mst convert creator reader

mst: alg3_mastiff.c aux.c cc.c graph.c msf.c omp.c partitioning.c trans.c
	$(CC) alg3_mastiff.c -o mst.exe $(FLAGS)

convert: graphTextHandler.c
	$(CC) graphTextHandler.c -o convert.exe $(FLAGS)

creator: graphCreator.c
	$(CC) graphCreator.c -o creator.exe $(FLAGS)
reader: graphReader.c
	$(CC) graphReader.c -o reader.exe $(FLAGS)