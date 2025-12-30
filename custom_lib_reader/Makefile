CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp -D_GNU_SOURCE -I../custom_lib
LDFLAGS = -fopenmp -lm

SRCS := reader.c $(wildcard ../custom_lib/*.c)

OBJS = $(SRCS:.c=.o)

TARGET = reader

all: $(TARGET)

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean