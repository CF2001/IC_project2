cmake_minimum_required(VERSION 3.16)
project(src)

find_package( OpenCV REQUIRED )
include_directories( ${OpenCV_INCLUDE_DIRS} )

SET (CMAKE_BUILD_TYPE "Release")

SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -std=c++17")
SET (CMAKE_CXX_FLAGS_RELEASE "-O3")
SET (CMAKE_CXX_FLAGS_DEBUG "-g3 -fsanitize=address")

SET (BASE_DIR ${CMAKE_SOURCE_DIR} )
SET (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${BASE_DIR}/../src-bin)

## Exercise 1
add_executable( ex1 ex1.cpp )
target_link_libraries( ex1 ${OpenCV_LIBS} )

## Exercise 2
add_executable( ex2 ex2.cpp )
target_link_libraries( ex2 ${OpenCV_LIBS} )

## Exercise 3
add_executable( Golomb Golomb.cpp )

## Exercise 4
add_executable ( audioCodec audioCodec.cpp)
target_link_libraries (audioCodec sndfile) 
