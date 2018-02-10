LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := FreeType

LOCAL_C_INCLUDES := $(LOCAL_PATH)

#LOCAL_SRC_FILES := SDL_ttf.c \

    LOCAL_C_INCLUDES += $(LOCAL_PATH)/include
    LOCAL_CFLAGS += -DFT2_BUILD_LIBRARY



LOCAL_SRC_FILES := \
       $(LOCAL_PATH)/src/autofit/autofit.c \
$(LOCAL_PATH)/src/base/basepic.c \
$(LOCAL_PATH)/src/base/ftapi.c \
$(LOCAL_PATH)/src/base/ftbase.c \
$(LOCAL_PATH)/src/base/ftbbox.c \
$(LOCAL_PATH)/src/base/ftbitmap.c \
$(LOCAL_PATH)/src/base/ftdbgmem.c \
$(LOCAL_PATH)/src/base/ftdebug.c \
$(LOCAL_PATH)/src/base/ftglyph.c \
$(LOCAL_PATH)/src/base/ftinit.c \
$(LOCAL_PATH)/src/base/ftpic.c \
$(LOCAL_PATH)/src/base/ftstroke.c \
$(LOCAL_PATH)/src/base/ftsynth.c \
$(LOCAL_PATH)/src/base/ftsystem.c \
$(LOCAL_PATH)/src/cff/cff.c \
$(LOCAL_PATH)/src/pshinter/pshinter.c \
$(LOCAL_PATH)/src/pshinter/pshglob.c \
$(LOCAL_PATH)/src/pshinter/pshpic.c \
$(LOCAL_PATH)/src/pshinter/pshrec.c \
$(LOCAL_PATH)/src/psnames/psnames.c \
$(LOCAL_PATH)/src/psnames/pspic.c \
$(LOCAL_PATH)/src/raster/raster.c \
$(LOCAL_PATH)/src/raster/rastpic.c \
$(LOCAL_PATH)/src/sfnt/pngshim.c \
$(LOCAL_PATH)/src/sfnt/sfntpic.c \
$(LOCAL_PATH)/src/sfnt/ttbdf.c \
$(LOCAL_PATH)/src/sfnt/ttkern.c \
$(LOCAL_PATH)/src/sfnt/ttload.c \
$(LOCAL_PATH)/src/sfnt/ttmtx.c \
$(LOCAL_PATH)/src/sfnt/ttpost.c \
$(LOCAL_PATH)/src/sfnt/ttsbit.c \
$(LOCAL_PATH)/src/sfnt/sfobjs.c \
$(LOCAL_PATH)/src/sfnt/ttcmap.c \
$(LOCAL_PATH)/src/sfnt/sfdriver.c \
$(LOCAL_PATH)/src/smooth/smooth.c \
$(LOCAL_PATH)/src/smooth/ftspic.c \
$(LOCAL_PATH)/src/truetype/truetype.c \
$(LOCAL_PATH)/src/type1/t1driver.c \
$(LOCAL_PATH)/src/cid/cidgload.c \
$(LOCAL_PATH)/src/cid/cidload.c \
$(LOCAL_PATH)/src/cid/cidobjs.c \
$(LOCAL_PATH)/src/cid/cidparse.c \
$(LOCAL_PATH)/src/cid/cidriver.c \
$(LOCAL_PATH)/src/pfr/pfr.c \
$(LOCAL_PATH)/src/pfr/pfrgload.c \
$(LOCAL_PATH)/src/pfr/pfrload.c \
$(LOCAL_PATH)/src/pfr/pfrobjs.c \
$(LOCAL_PATH)/src/pfr/pfrsbit.c \
$(LOCAL_PATH)/src/type42/t42objs.c \
$(LOCAL_PATH)/src/type42/t42parse.c \
$(LOCAL_PATH)/src/type42/type42.c \
$(LOCAL_PATH)/src/winfonts/winfnt.c \
$(LOCAL_PATH)/src/pcf/pcfread.c \
$(LOCAL_PATH)/src/pcf/pcfutil.c \
$(LOCAL_PATH)/src/pcf/pcfdrivr.c \
$(LOCAL_PATH)/src/psaux/afmparse.c \
$(LOCAL_PATH)/src/psaux/psaux.c \
$(LOCAL_PATH)/src/psaux/psconv.c \
$(LOCAL_PATH)/src/psaux/psobjs.c \
$(LOCAL_PATH)/src/psaux/t1decode.c \
$(LOCAL_PATH)/src/tools/apinames.c \
$(LOCAL_PATH)/src/type1/t1afm.c \
$(LOCAL_PATH)/src/type1/t1gload.c \
$(LOCAL_PATH)/src/type1/t1load.c \
$(LOCAL_PATH)/src/type1/t1objs.c \
$(LOCAL_PATH)/src/type1/t1parse.c\
$(LOCAL_PATH)/src/bdf/bdfdrivr.c\
$(LOCAL_PATH)/src/bdf/bdflib.c\
$(LOCAL_PATH)/src/gzip/ftgzip.c\
$(LOCAL_PATH)/src/lzw/ftlzw.c \
 
 
LOCAL_EXPORT_C_INCLUDES += $(LOCAL_C_INCLUDES)

include $(BUILD_SHARED_LIBRARY)