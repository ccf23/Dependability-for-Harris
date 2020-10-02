cmake_minimum_required(VERSION 2.8)
project( Harris CXX )



set(CMAKE_SYSTEM_NAME Linux)
set(CMAKE_SYSTEM_PROCESSOR arm)
set(CMAKE_C_COMPILER ${tools}/usr/bin/arm-linux-gnueabihf-gcc)
set(CMAKE_CXX_COMPILER ${tools}/usr/bin/arm-linux-gnueabihf-g++)
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)


#find_package(OpenCV REQUIRED PATHS /home/daniel/openCV-arm/opencv/)
#
 find_package( OpenCV 4.5.0 )
#set(OpenCV_INCLUDE_DIRS "/home/daniel/openCV-arm/opencv/platforms/linux/lib")
#include_directories(${OpenCV_INCLUDE_DIRS})

MESSAGE( STATUS "------------------" ${OpenCV_INCLUDE_DIRS} )
#

# TODO: get this to staticly compile
#SET(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
#SET(BUILD_SHARED_LIBS OFF)
#SET(CMAKE_EXE_LINKER_FLAGS "-static")

add_executable( Harris 
	./src/util.cpp
	./src/harris.cpp
	./src/main.cpp
)


target_include_directories( Harris
	PRIVATE 	.
			${OpenCV_INCLUDE_DIRS}
)

target_link_libraries( Harris 
	${OpenCV_LIBS} 
)
