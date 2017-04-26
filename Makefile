 

CPPFLAGS=$(shell sdl2-config --cflags) $(shell $(PKG_CONFIG) freetype2 --cflags) $(EXTRA_CPPFLAGS) -Wall -Wextra -DSQLITE_ENABLE_RTREE=1 -g
LDLIBS=-I. $(shell sdl2-config --libs) $(shell $(PKG_CONFIG) freetype2 --libs) $(EXTRA_LDLIBS) -lGLEW -lGL  -lm -lpthread -ldl 
PKG_CONFIG?=pkg-config
all:    src/platform_specific.o src/text_utils.o src/mem_handling.o src/ext/sqlite/sqlite3.o src/handle_db.o src/shader_utils.o  src/utils.o src/getData.o src/eventHandling.o src/twkb.o src/varint.o src/twkb_decode.o src/rendering.o src/touch.o src/main.o  
	gcc -o tileLess src/platform_specific.o  src/text_utils.o src/mem_handling.o src/ext/sqlite/sqlite3.o src/shader_utils.o src/handle_db.o src/utils.o src/getData.o  src/eventHandling.o src/twkb.o src/varint.o src/twkb_decode.o src/rendering.o src/touch.o src/main.o $(CPPFLAGS) $(LDLIBS)
clean:
	rm -f src/*.o src/ext/sqlite/*.o
.PHONY: all clean
