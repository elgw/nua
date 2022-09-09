CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11
LDFLAGS=

ifdef DEBUG
CFLAGS+=-g3
else
CFLAGS+=-O3 -march=native -DNDEBUG
LDFLAGS+=-flto
endif

CFLAGS+=`sdl2-config --cflags`
LDFLAGS+=`sdl2-config --libs`

CFLAGS+=`pkg-config vulkan --cflags`
LDFLAGS+=`pkg-config vulkan --libs`

# libz
LDFLAGS+=`pkg-config zlib --libs`

# libxml2
CFLAGS+=`xml2-config --cflags`
LDFLAGS+=`xml2-config --libs`

# threads
LDFLAGS+=-pthread

# Kazmath
# https://github.com/Kazade/kazmath
LDFLAGS+=-lkazmath

# standard math library
LDFLAGS+=-lm

# STB image
# https://github.com/nothings/stb
CFLAGS+=`pkg-config stb --cflags`
LDFLAGS+=`pkg-config stb --libs`


nuademo: src/nuademo.c src/nuademo.h nucleard.o vshape.o cmm_io.o vertex.o nua_util.o nucleard.o nua_vkutil.o
	$(CC) $(CFLAGS) src/nuademo.c nucleard.o vshape.o cmm_io.o vertex.o nua_util.o nua_vkutil.o $(LDFLAGS) -o nuademo


FILES=src/nucleard.c src/nucleard.h
nucleard.o: $(FILES)  shaders
	$(CC) -c $(CFLAGS) $(FILES)

nua_vkutil.o: src/nua_vkutil.c src/nua_vkutil.h
	$(CC) -c $(CFLAGS) src/nua_vkutil.c

vshape.o: src/vshape.c src/vshape.h
	$(CC) -c $(CFLAGS) src/vshape.c

cmm_io.o: src/cmm_io.c src/cmm_io.h
	$(CC) -c $(CFLAGS) src/cmm_io.c

vertex.o: src/vertex.c src/vertex.h
	$(CC) -c $(CFLAGS) src/vertex.c

nua_util.o: src/nua_util.c src/nua_util.h
	$(CC) -c $(CFLAGS) src/nua_util.c

shaders: shaders/shader_ball_frag.spv shaders/shader_ball_vert.spv shaders/shader_connect_frag.spv shaders/shader_connect_vert.spv shaders/shader_domain_frag.spv shaders/shader_domain_vert.spv

cmm_io_ut: cmm_io.o src/cmm_io_ut.c src/cmm_io.c
	$(CC) $(CFLAGS) src/cmm_io_ut.c cmm_io.o $(LDFLAGS) -o cmm_io_ut

%.spv: %.vert
	glslangValidator -V ./$< -o $@

%.spv: %.frag
	glslangValidator -V ./$< -o $@


VSFILES=vshape_ut.c vshape.c
vshape_ut: $(VSFILES)
	gcc -Wall -Wextra -g3 $(VSFILES) -lm -o vshape_ut

clean:
	rm -f nuademo
	rm -f vshape_ut
	rm -f *.o
	rm -f shaders/*.spv
	rm -f src/*.gch
	rm -f cmm_io_ut
