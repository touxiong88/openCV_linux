cmake_minimum_required(VERSION 2.8 FATAL_ERROR)

project(test)

find_package(OpenCV REQUIRED)

message(STATUS "OpenCV library status:")
message(STATUS "    version: ${OpenCV_VERSION}")
message(STATUS "    libraries: ${OpenCV_LIBS}")
message(STATUS "    include path: ${OpenCV_INCLUDE_DIRS}")

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories( include )
include_directories( src )
link_directories(${OpenCV_LIBRARY_DIRS})

add_definitions(${PCL_DEFINITIONS} -std=c++11   -DTARGET_PC=1   -DUSE_LINUX=1 -g -fopenmp)

add_executable (test 
src/AngleDetect.cpp
src/Bitmap.cpp
src/Calibration.cpp
src/TimeUtils.cpp
src/Utils.cpp 
main.cpp

)



target_link_libraries (test  ${OpenCV_LIBS} )

#######################################


#add_executable (opencv_view opencv_view.cpp)
#target_link_libraries (opencv_view ${PCL_LIBRARIES} ${OpenCV_LIBS} -fopenmp )