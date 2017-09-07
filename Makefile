 

CPPFLAGS=$(shell sdl2-config --cflags) $(shell $(PKG_CONFIG) SDL2_image --cflags) $(shell $(PKG_CONFIG) freetype2 --cflags) $(EXTRA_CPPFLAGS) -DSQLITE_ENABLE_RTREE=1  -Wall -Wextra -pedantic -g
LDLIBS=-I. $(shell sdl2-config --libs) $(shell $(PKG_CONFIG) SDL2_image --libs) $(shell $(PKG_CONFIG) freetype2 --libs) $(EXTRA_LDLIBS) -lGLEW -lGL  -lm -lpthread -ldl -lmxml
PKG_CONFIG?=pkg-config
all:    src/symbols.o  src/read_sld.o src/fonts.o src/pip.o src/info.o src/interface/interface.o src/interface/ui.o src/interface/textbox.o src/interface/radiobutton.o src/mem.o src/buffer_handling.o src/reproject.o src/gps.o src/label_utils.o src/layer_utils.o src/init_data.o src/linewidth.o src/simple_geometries.o  src/log.o src/text.o src/mem_handling.o src/ext/sqlite/sqlite3.o src/handle_db.o src/shader_utils.o  src/utils.o src/getData.o src/eventHandling.o src/twkb.o src/varint.o src/twkb_decode.o src/rendering.o src/touch.o src/main.o  
	gcc -o tileLess src/symbols.o src/read_sld.o  src/fonts.o src/pip.o src/info.o src/interface/interface.o src/interface/ui.o src/interface/textbox.o src/interface/radiobutton.o src/mem.o src/buffer_handling.o src/reproject.o src/gps.o src/label_utils.o src/layer_utils.o src/init_data.o src/linewidth.o src/simple_geometries.o src/log.o  src/text.o src/mem_handling.o src/ext/sqlite/sqlite3.o src/shader_utils.o src/handle_db.o src/utils.o src/getData.o  src/eventHandling.o src/twkb.o src/varint.o src/twkb_decode.o src/rendering.o src/touch.o src/main.o $(CPPFLAGS) $(LDLIBS)
clean:
	rm -f src/*.o src/interface/*.o src/ext/sqlite/*.o
.PHONY: all clean
