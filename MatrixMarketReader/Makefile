CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp -D_GNU_SOURCE -I../lib
LDFLAGS = -fopenmp -lm

# Source files
SRCS = mtxReader.c \
       ../lib/storage_formats/matrix_market/matrix_market.c \
	   ../lib/omp_functions.c \
	   ../lib/io.c \
	   ../lib/string_util.c \
	   ../lib/plot/plot.c \
	   ../lib/parallel_io.c \
	   ../lib/pthread_functions.c \
	   ../lib/topology/hardware_topology.c \
	   ../lib/array_metrics.c \
	   ../lib/random.c \
	   ../lib/hash/hash.c \
# 	   ../lib/aux/csr_converter.c \
	   

OBJS = $(SRCS:.c=.o)

TARGET = mtxReader

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
