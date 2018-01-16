LOCAL_PATH := $(call my-dir)
include $(CLEAR_VARS)
LOCAL_MODULE := 3djni
LOCAL_SRC_FILES := lib3djni.so
include $(PREBUILT_SHARED_LIBRARY)

####################################################
#include $(CLEAR_VARS)
#LOCAL_MODULE := calibration
#LOCAL_SRC_FILES := libcalibration.so
#include $(PREBUILT_SHARED_LIBRARY)

#####################################################
#include $(CLEAR_VARS)
#LOCAL_MODULE := opencv_java
#LOCAL_SRC_FILES := libopencv_java.so
#include $(PREBUILT_SHARED_LIBRARY)
