CC = gcc
CFLAGS = -Wall -o $@

HDIR = src/headers

SRCS = $(wildcard src/*.c)
OBJS = $(subst src, .bin, $(SRCS:.c=.o))
HDRS = $(wildcard src/headers/*.h)

TARGET = ./markov


.PHONY: test clean

.bin:
	mkdir -p .bin

$(TARGET): .bin $(OBJS)
	$(CC) $(CFLAGS) $(OBJS)

.bin/%.o: src/%.c
	$(CC) $(CFLAGS) -I $(HDIR) -c $<

test: $(TARGET)
	$(TARGET) --ignA —«» -i src/texts/II

clean:
	rm -rf .bin $(TARGET)
