cmake_minimum_required(VERSION 3.10)
project (TestLibrary)

set(CMAKE_CXX_STANDARD 11)
 
#For the shared library:
set ( PROJECT_LINK_LIBS libpipert.so )
link_directories(../build) 
 
include_directories(../include; ./yolo)

find_package(OpenCV REQUIRED)

add_executable(simple main2.cpp)
target_link_libraries(simple ${PROJECT_LINK_LIBS})

target_compile_options(simple PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
set_target_properties(simple PROPERTIES LINK_FLAGS_DEBUG "-fsanitize=address")

add_executable(main_yolo main_yolo.cpp)
target_link_libraries(main_yolo ${PROJECT_LINK_LIBS} ${OpenCV_LIBS})

target_compile_options(main_yolo PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
set_target_properties(main_yolo PROPERTIES LINK_FLAGS_DEBUG "-fsanitize=address")
