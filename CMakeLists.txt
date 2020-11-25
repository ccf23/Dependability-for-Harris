cmake_minimum_required(VERSION 2.8)
project( Harris CXX )

if(arm)
	message("---- Compiling for ARM ----")

	set(CMAKE_SYSTEM_NAME Linux)
	set(CMAKE_SYSTEM_PROCESSOR arm)
	set(CMAKE_C_COMPILER ${tools}/usr/bin/arm-linux-gnueabihf-gcc)
	set(CMAKE_CXX_COMPILER ${tools}/usr/bin/arm-linux-gnueabihf-g++)
	set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
	set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
	set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
	
	set(CMAKE_REQUIRED_LIBRARIES "ITPP")

	# This is so I won't have to recompile openCV, it won't affect normal functionality
	if(daniel)
		find_package( OpenCV 4.5.0 REQUIRED)
	else()
		find_package( OpenCV 4.5.0 REQUIRED PATHS /usr/lib/lib/cmake/opencv4 NO_DEFAULT_PATH)
	endif()
	



	set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")
	set(BUILD_SHARED_LIBS OFF)
	set(CMAKE_EXE_LINKER_FLAGS "-static")

else()
	message("---- Compiling for local ----")

	find_package(OpenCV REQUIRED)

endif()

find_package(OpenMP REQUIRED)

message( STATUS "------------------" ${OpenCV_INCLUDE_DIRS} )

add_executable( Harris 
	./src/util.cpp
	./src/harris.cpp
	./src/main.cpp
	./src/hammingcode.cpp
	./src/injector.cpp
)

target_include_directories( Harris
	PRIVATE 	.
			${OpenCV_INCLUDE_DIRS}
)

target_include_directories( Harris
	PRIVATE		.
			/usr/local/itpp-arm/include
)

target_link_libraries( Harris 
	${OpenCV_LIBS}
	${OpenMP_CXX_FLAGS}	
	/usr/local/itpp-arm/lib/libitpp_static.a
)

target_link_libraries( Harris

)
