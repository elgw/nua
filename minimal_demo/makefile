CC=gcc -Wall -Wextra

# and nua
LDFLAGS+=-lnua
# SDL2
CFLAGS+=`sdl2-config --cflags`
LDFLAGS+=`sdl2-config --libs`
LDFLAGS+=-lSDL2_image
# Vulkan
CFLAGS+=`pkg-config vulkan --cflags`
LDFLAGS+=`pkg-config vulkan --libs`
# xml2
CFLAGS+=`xml2-config --cflags`
LDFLAGS+=`xml2-config --libs`
# kazmath
LDFLAGS+=-lkazmath
# math
LDFLAGS+=-lm

LDF=-Wl,--start-group $(LDFLAGS) -Wl,--end-group

minimal_demo: minimal_demo.c
	$(CC) $(CFLAGS) minimal_demo.c $(LDF) -o minimal_demo
