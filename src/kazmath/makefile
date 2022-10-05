CC=gcc
CFLAGS=-W -std=gnu11 -O3 -march=native -mtune=native
LDFLAGS=-lm

SRCS=$(wildcard *.c)
OBJECTS=$(SRCS:.c=.o)

lib_nuakazmath.a: $(OBJECTS)
	ar rcs lib_nuakazmath.a $(OBJECTS)

%.o: %.c
	$(CC) -fPIC -c $(CFLAGS) ./$< $(LDFLAGS)

clean:
	rm -f *.o
	rm -f *.a
