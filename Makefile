 

CPPFLAGS=$(shell sdl2-config --cflags) $(shell $(PKG_CONFIG) freetype2 --cflags) $(EXTRA_CPPFLAGS) -Wall -Wextra -DSQLITE_ENABLE_RTREE=1 -g
LDLIBS=-I. $(shell sdl2-config --libs) $(shell $(PKG_CONFIG) freetype2 --libs) $(EXTRA_LDLIBS) -lGLEW -lGL  -lm -lpthread -ldl 
PKG_CONFIG?=pkg-config
all:    text.o mem_handling.o ext/sqlite/sqlite3.o handle_db.o shader_utils.o  utils.o getData.o eventHandling.o twkb.o varint.o twkb_decode.o rendering.o touch.o main.o  
	gcc -o tileLess text.o mem_handling.o ext/sqlite/sqlite3.o shader_utils.o handle_db.o utils.o getData.o  eventHandling.o twkb.o varint.o twkb_decode.o rendering.o touch.o main.o $(CPPFLAGS) $(LDLIBS)
clean:
	rm -f *.o ext/sqlite/*.o
.PHONY: all clean
