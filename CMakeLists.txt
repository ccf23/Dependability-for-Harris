cmake_minimum_required(VERSION 2.8)
project( Harris )

MESSAGE( STATUS "------------------" ${OpenCV_INCLUDE_DIRS} )

#
find_package( OpenCV REQUIRED )

#
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
