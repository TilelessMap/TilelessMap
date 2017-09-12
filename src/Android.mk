LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_image_PATH := ../SDL_image
freetype_PATH := ../freetype
#APP_DEPRECATED_HEADERS := true
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include 
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_image_PATH)/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(freetype_PATH)/include 

LOCAL_CFLAGS += -DSQLITE_ENABLE_RTREE -O3  -funroll-loops
#LOCAL_LDLIBS += -lGL  -lm -lpthread -ldl 


LOCAL_LDLIBS += -lGLESv1_CM -lGLESv2 -llog
# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
src/ext/sqlite/sqlite3.c \
src/pip.c \
src/info.c \
src/interface/ui.c \
src/interface/interface.c \
src/interface/textbox.c \
src/interface/radiobutton.c \
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


#sqlite

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_SHARED_LIBRARIES += FreeType 

include $(BUILD_SHARED_LIBRARY)
