LIB_PATH := C:/Users/Recademics/Desktop/RC_BASIC/rc_basic_runtime_android/jni/libs
LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

LOCAL_MODULE := main

SDL_PATH := ../SDL

LOCAL_C_INCLUDES := $(LOCAL_PATH)/$(SDL_PATH)/include $(LOCAL_PATH)/../SDL2_image $(LOCAL_PATH)/../SDL2_ttf $(LOCAL_PATH)/../SDL2_mixer $(LOCAL_PATH)/../SDL2_net $(LOCAL_PATH)/../SDL2_gfx $(LOCAL_PATH)/../theora $(LOCAL_PATH)/../

# Add your application source files here...
LOCAL_SRC_FILES := $(SDL_PATH)/src/main/android/SDL_android_main.c \
	main.cpp \
	theoraplay.c

LOCAL_SHARED_LIBRARIES := SDL2 SDL2_image SDL2_ttf SDL2_mixer SDL2_net SDL2_gfx theora
LOCAL_LDLIBS := -lGLESv1_CM -lGLESv2 -llog

include $(BUILD_SHARED_LIBRARY)
