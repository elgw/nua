CC=gcc
CFLAGS=-Wall -Wextra -std=gnu11
LDFLAGS=

ifdef DEBUG
CFLAGS+=-g3
else
CFLAGS+=-O3 -march=native -DNDEBUG
LDFLAGS+=-flto
endif

# SDL2
CFLAGS+=`sdl2-config --cflags`
LDFLAGS+=`sdl2-config --libs`
LDFLAGS+=-lSDL2_image

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

LDF=-Wl,--start-group $(LDFLAGS) -Wl,--end-group

nuademo: src/nuademo.c src/nuademo.h libnua.so
	$(CC) $(CFLAGS) src/nuademo.c -lnua -L./ $(LDF) -o nuademo

VSFILES=src/vshape_ut.c src/vshape.c
vshape_ut: $(VSFILES)
	gcc -Wall -Wextra -g3 $(VSFILES) -lm -o vshape_ut

cmm_io_ut: cmm_io.o src/cmm_io_ut.c src/cmm_io.c
	$(CC) $(CFLAGS) src/cmm_io_ut.c cmm_io.o $(LDF) -o cmm_io_ut

NUA_SO_OBJECTS=nua_object.o nua.o cmm_io.o nua_util.o nua_vkutil.o vshape.o
libnua.so: $(NUA_SO_OBJECTS)
	gcc -shared -fpic -o libnua.so $(NUA_SO_OBJECTS)

CFLAGS+=-fpic

FILES=src/nua.c src/nua.h

SHADERS_INCLUDES=shaders/shader_ball_frag.spv.h shaders/shader_ball_vert.spv.h shaders/shader_connect_frag.spv.h shaders/shader_connect_vert.spv.h shaders/shader_domain_frag.spv.h shaders/shader_domain_vert.spv.h

%.spv.h: %.spv
	xxd -i ./$< > $@

%.spv: %.vert
	glslangValidator -V ./$< -o $@

%.spv: %.frag
	glslangValidator -V ./$< -o $@


nua.o: $(FILES)
	$(CC) -c $(CFLAGS) $(FILES)

nua_vkutil.o: src/nua_vkutil.c src/nua_vkutil.h
	$(CC) -c $(CFLAGS) src/nua_vkutil.c

vshape.o: src/vshape.c src/vshape.h
	$(CC) -c $(CFLAGS) src/vshape.c

cmm_io.o: src/cmm_io.c src/cmm_io.h
	$(CC) -c $(CFLAGS) src/cmm_io.c

nua_object.o: src/nua_object.c src/nua_object.h $(SHADERS_INCLUDES)
	$(CC) -c $(CFLAGS) src/nua_object.c

nua_util.o: src/nua_util.c src/nua_util.h
	$(CC) -c $(CFLAGS) src/nua_util.c

shaders: shaders/shader_ball_frag.spv shaders/shader_ball_vert.spv shaders/shader_connect_frag.spv shaders/shader_connect_vert.spv shaders/shader_domain_frag.spv shaders/shader_domain_vert.spv

clean:
	rm -f nuademo
	rm -f vshape_ut
	rm -f *.o
	rm -f shaders/*.spv
	rm -f shaders/*.spv.h
	rm -f src/*.gch
	rm -f cmm_io_ut
	rm -f libnua.so
