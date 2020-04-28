cmake_minimum_required(VERSION 3.10)
project(pipert)

set(CMAKE_BUILD_TYPE "Release")
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wno-long-long -pedantic")

#Bring the headers, such as Student.h into the project
include_directories(include)
include_directories(src)


#However, the file(GLOB...) allows for wildcard additions:
file(GLOB SOURCES "src/*.cpp")
#add_library(pipert SHARED ${SOURCES})
add_library(pipert SHARED ${SOURCES})

target_link_libraries(pipert pthread)