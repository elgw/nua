CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11 -O3
LDFLAGS=-lm

SRCS=$(wildcard *.c)
OBJECTS=$(SRCS:.c=.o)

lib_nuakazmath.a: $(OBJECTS)
	ar rcs lib_nuakazmath.a $(OBJECTS)

%.o: %.c
	$(CC) -fPIC -c $(CFLAGS) ./$< $(LDFLAGS)

clean:
	rm *.o
	rm *.a
