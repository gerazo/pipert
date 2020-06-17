cmake_minimum_required(VERSION 3.10)

set(BINARY run_tests)

set(SOURCES ${TEST_SOURCES})

include_directories(../include)
include_directories(../src)

add_subdirectory(lib/googletest)

file(GLOB_RECURSE TEST_SOURCES main.cpp PacketImp_test.cpp ../src/*.cpp)

add_executable(${BINARY} ${TEST_SOURCES})

add_test(NAME ${BINARY} COMMAND ${BINARY})

target_link_libraries(${BINARY} PUBLIC gtest)