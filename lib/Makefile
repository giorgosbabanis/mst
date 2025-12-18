# === Build static library libmylib.a ===

CC = gcc
CFLAGS = -Wall -Wextra -O2 -fopenmp -D_GNU_SOURCE
INCLUDES = -I.

# Find all .c files in this directory and subdirectories
SRCS := $(shell find . -name "*.c")
OBJS := $(SRCS:.c=.o)

TARGET = lib.a

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $(TARGET) $(OBJS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

clean:
	rm -f $(OBJS) $(TARGET)

.PHONY: all clean
