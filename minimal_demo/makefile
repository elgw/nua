CC=gcc -Wall -Wextra

CFLAGS+=`sdl2-config --cflags`
CFLAGS+=`pkg-config vulkan --cflags`
CFLAGS+=`xml2-config --cflags`

LDFLAGS+=-lnua

minimal_demo: minimal_demo.c
	$(CC) $(CFLAGS) minimal_demo.c $(LDFLAGS) -o minimal_demo
