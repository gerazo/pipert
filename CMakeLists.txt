cmake_minimum_required(VERSION 3.10)
project(pipert CXX)
enable_testing()


# Configuration
set(SOURCES ChannelBase.cpp Scheduler.cpp SchedulerImp.cpp Timer.cpp)
set(TEST_SOURCES PacketTest.cpp)


# General settings
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Using default build type Release as none was specified with -DCMAKE_BUILD_TYPE=<mode>")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(USE_ADDRESS_SANITIZER ON CACHE BOOL "Use -fsanitize=address for leak checking.")

if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_AR gcc-ar)
  set(CMAKE_RANLIB gcc-ranlib)
  set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold)
  set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_AR llvm-ar)
  set(CMAKE_RANLIB llvm-ranlib)
  set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold)
  set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold)
endif()

function(SetupTarget target_name)
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 11)
  target_compile_options(${target_name} PRIVATE -Wall -pedantic -Wextra -Werror)
  if(USE_ADDRESS_SANITIZER)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    set_target_properties(${target_name} PROPERTIES LINK_FLAGS_DEBUG "-fsanitize=address")
  endif()
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-flto>)
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-flto>)
  set_target_properties(${target_name} PROPERTIES LINK_FLAGS_RELEASE "-flto")
  set_target_properties(${target_name} PROPERTIES LINK_FLAGS_RELWITHDEBINFO "-flto")
  target_include_directories(${target_name} PRIVATE "include")
endfunction(SetupTarget)

unset(REAL_SOURCES)
foreach(CPPFILE ${SOURCES})
  set(REAL_SOURCES ${REAL_SOURCES} src/${CPPFILE})
endforeach(CPPFILE)

unset(REAL_TEST_SOURCES)
foreach(CPPFILE ${TEST_SOURCES})
  set(REAL_TEST_SOURCES ${REAL_TEST_SOURCES} test/${CPPFILE})
endforeach(CPPFILE)


# Libraries
find_package(Threads)
# find_package(PkgConfig)
include(googletest.cmake)


# Targets
add_library(${PROJECT_NAME}_static STATIC ${REAL_SOURCES})
SetupTarget(${PROJECT_NAME}_static)

add_library(${PROJECT_NAME} SHARED ${REAL_SOURCES})
SetupTarget(${PROJECT_NAME})
if(Threads_FOUND)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

add_executable(${PROJECT_NAME}_test ${REAL_TEST_SOURCES})
SetupTarget(${PROJECT_NAME}_test)
if(Threads_FOUND)
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_static gtest_main ${CMAKE_THREAD_LIBS_INIT})
else()
  target_link_libraries(${PROJECT_NAME}_test ${PROJECT_NAME}_static gtest_main)
endif()
add_test(NAME ${PROJECT_NAME}_test COMMAND ${PROJECT_NAME}_test)
