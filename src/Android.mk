LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)
#OPENCV_CAMERA_MODULES:=off
OPENCV_INSTALL_MODULES:=on
#OPENCV_LIB_TYPE:=SHARED

ifeq ("$(wildcard $(OPENCV_MK_PATH))","")
include G:\source\OpenCV-2.4.8-android-sdk\sdk\native\jni\OpenCV.mk
include G:\source\OpenCV-2.4.8-android-sdk\sdk\native\jni
else  
include $(OPENCV_MK_PATH)  
endif
		
LOCAL_SRC_FILES :=  \
	AngleDetect.cpp \
	Bitmap.cpp \
	Calibration.cpp \
	CalibrationJNI.cpp \
	TimeUtils.cpp \
	Utils.cpp \

		

LOCAL_C_INCLUDES +=  $(LOCAL_PATH)/include

LOCAL_SHARED_LIBRARIES += \
	libandroid \
	libnativehelper \
	libutils \
	libskia \
	libcutils

LOCAL_CPPFLAGS 	+= -DUSE_LINUX

LOCAL_LDLIBS 	+= -llog -lm -lGLESv2  -landroid

LOCAL_MODULE	:= calibration
LOCAL_LDLIBS	+= -llog -ldl 
LOCAL_CPPFLAGS 	+= -std=c++11 -fexceptions -frtti -fpermissive
include $(BUILD_SHARED_LIBRARY)
############################################
include $(CLEAR_VARS)
LOCAL_MODULE := Reconstruct
LOCAL_SRC_FILES := libReconstruct.so
#include $(PREBUILT_SHARED_LIBRARY)
