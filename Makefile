 

CPPFLAGS=$(shell sdl2-config --cflags) $(shell $(PKG_CONFIG) SDL2_image --cflags) $(shell $(PKG_CONFIG) freetype2 --cflags) $(EXTRA_CPPFLAGS) -DSQLITE_ENABLE_RTREE=1  -g -Wall -Wextra -pedantic
LDLIBS=-I. -Isrc $(shell sdl2-config --libs) $(shell $(PKG_CONFIG) SDL2_image --libs) $(shell $(PKG_CONFIG) freetype2 --libs) $(EXTRA_LDLIBS) -lGLEW -lGL  -lm -lpthread -ldl -lmxml
PKG_CONFIG?=pkg-config

SRC= \
src/main.o \
src/event_loop.o \
src/cleanup.o \
src/init.o \
src/symbols.o \
src/pip.o \
src/info.o \
src/mem.o \
src/buffer_handling.o \
src/reproject.o \
src/gps.o \
src/label_utils.o \
src/layer_utils.o \
src/init_data.o \
src/linewidth.o \
src/simple_geometries.o \
src/log.o \
src/mem_handling.o \
src/shader_utils.o \
src/utils.o \
src/getData.o \
src/rendering.o \
src/styling/read_sld.o \
src/text/fonts.o \
src/text/text.o \
src/read_data/twkb.o \
src/read_data/varint.o \
src/read_data/twkb_decode.o \
src/handle_input/touch.o \
src/handle_input/eventHandling.o \
src/interface/table.o \
src/interface/button.o \
src/interface/interface.o \
src/interface/ui.o \
src/interface/textbox.o \
src/interface/radiobutton.o \
src/ext/sqlite/sqlite3.o \






all:     $(SRC)
	gcc -o tileLess  $(SRC) $(CPPFLAGS) $(LDLIBS)
clean:
	rm -f src/*.o src/interface/*.o src/handle_input/*.o src/read_data/*.o src/styling/*.o src/text/*.o src/ext/sqlite/*.o
.PHONY: all clean
