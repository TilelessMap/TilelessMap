LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := mxml

LOCAL_CFLAGS += -DSQLITE_ENABLE_RTREE -O3  -funroll-loops
#LOCAL_LDLIBS += -lGL  -lm -lpthread -ldl 



#mxml

LOCAL_SRC_FILES := \
mxml-get.c \
mxml-attr.c \
mxml-private.c \
mxml-file.c \
mxml-index.c \
mmd.c \
mxml-set.c \
mxml-node.c \
mxml-search.c \
mxml-entity.c \
mxml-string.c



include $(BUILD_SHARED_LIBRARY)
