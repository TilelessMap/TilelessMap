LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

THE_APP_ROOT = $(LOCAL_PATH)/../../../../../../src
LOCAL_MODULE := main

SDL_PATH := ../SDL
SDL_image_PATH := ../SDL_image
freetype_PATH := ../freetype
mxml_PATH := ../mxml
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include 
LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_image_PATH)/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(freetype_PATH)/include 
LOCAL_C_INCLUDES += $(LOCAL_PATH)/$(mxml_PATH)

LOCAL_CFLAGS += -DSQLITE_ENABLE_RTREE -O3  -funroll-loops
#LOCAL_LDLIBS += -lGL  -lm -lpthread -ldl 


LOCAL_LDLIBS += -lGLESv1_CM -lGLESv2 -llog
# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
$(THE_APP_ROOT)/main.c \
$(THE_APP_ROOT)/event_loop.c \
$(THE_APP_ROOT)/cleanup.c \
$(THE_APP_ROOT)/init.c \
$(THE_APP_ROOT)/symbols.c \
$(THE_APP_ROOT)/pip.c \
$(THE_APP_ROOT)/info.c \
$(THE_APP_ROOT)/mem.c \
$(THE_APP_ROOT)/buffer_handling.c \
$(THE_APP_ROOT)/reproject.c \
$(THE_APP_ROOT)/gps.c \
$(THE_APP_ROOT)/label_utils.c \
$(THE_APP_ROOT)/layer_utils.c \
$(THE_APP_ROOT)/init_data.c \
$(THE_APP_ROOT)/linewidth.c \
$(THE_APP_ROOT)/simple_geometries.c \
$(THE_APP_ROOT)/log.c \
$(THE_APP_ROOT)/mem_handling.c \
$(THE_APP_ROOT)/shader_utils.c \
$(THE_APP_ROOT)/utils.c \
$(THE_APP_ROOT)/getData.c \
$(THE_APP_ROOT)/rendering.c \
$(THE_APP_ROOT)/styling/read_sld.c \
$(THE_APP_ROOT)/text/fonts.c \
$(THE_APP_ROOT)/text/text.c \
$(THE_APP_ROOT)/read_data/twkb.c \
$(THE_APP_ROOT)/read_data/varint.c \
$(THE_APP_ROOT)/read_data/twkb_decode.c \
$(THE_APP_ROOT)/handle_input/eventHandling.c \
$(THE_APP_ROOT)/handle_input/touch.c \
$(THE_APP_ROOT)/interface/table.c \
$(THE_APP_ROOT)/interface/button.c \
$(THE_APP_ROOT)/interface/interface.c \
$(THE_APP_ROOT)/interface/ui.c \
$(THE_APP_ROOT)/interface/textbox.c \
$(THE_APP_ROOT)/interface/radiobutton.c 



#sqlite

LOCAL_SRC_FILES += \
$(THE_APP_ROOT)/ext/sqlite/sqlite3.c 

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image
LOCAL_SHARED_LIBRARIES += FreeType 
LOCAL_SHARED_LIBRARIES += mxml 

include $(BUILD_SHARED_LIBRARY)
