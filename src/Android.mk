LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_image_PATH := ../SDL_image
freetype_PATH := ../freetype
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include 
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_image_PATH)/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(freetype_PATH)/include 

LOCAL_CFLAGS += -DSQLITE_ENABLE_RTREE -O3  -funroll-loops
#LOCAL_LDLIBS += -lGL  -lm -lpthread -ldl 


LOCAL_LDLIBS += -lGLESv1_CM -lGLESv2 -llog
# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
src/event_loop.c \
src/cleanup.c \
src/init.c \
src/read_sld.c \
src/symbols.c \
src/pip.c \
src/info.c \
src/interface/ui.c \
src/interface/interface.c \
src/interface/textbox.c \
src/interface/radiobutton.c \
src/interface/table.c \
src/fonts.c \
src/reproject.c \
src/mem.c \
src/buffer_handling.c \
src/gps.c \
src/label_utils.c \
src/layer_utils.c \
src/init_data.c \
src/linewidth.c \
src/simple_geometries.c \
src/log.c \
src/text.c \
src/mem_handling.c \
src/handle_db.c \
src/shader_utils.c \
src/utils.c \
src/getData.c \
src/eventHandling.c \
src/twkb.c \
src/varint.c \
src/twkb_decode.c \
src/rendering.c \
src/touch.c \
src/main.c


#mxml

LOCAL_SRC_FILES += \
src/ext/mxml/mxml-get.c \
src/ext/mxml/mxml-attr.c \
src/ext/mxml/mxml-private.c \
src/ext/mxml/mxml-file.c \
src/ext/mxml/mxml-index.c \
src/ext/mxml/mmd.c \
src/ext/mxml/mxml-set.c \
src/ext/mxml/mxml-node.c \
src/ext/mxml/mxml-search.c \
src/ext/mxml/mxml-entity.c \
src/ext/mxml/mxml-string.c

#sqlite

LOCAL_SRC_FILES += \
src/ext/sqlite/sqlite3.c 

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_SHARED_LIBRARIES += FreeType 

include $(BUILD_SHARED_LIBRARY)
