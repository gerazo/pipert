cmake_minimum_required(VERSION 3.13)
project(pipert CXX)
enable_testing()


# All source files compiled into the library
set(SOURCES AdaptiveSpinLock.cpp ChannelBase.cpp ChannelImpl.cpp Scheduler.cpp SchedulerImpl.cpp Timer.cpp ThreadId.cpp)

# Put all test files here which can run using the library only through its public interface
set(BBTEST_SOURCES PacketTest.cpp PolledChannelTest.cpp SchedulerTest.cpp TimerTest.cpp ScheduledChannelTest.cpp MeasurementEventTest.cpp MeasurementProfileTest.cpp)

# Put those tests here which really, desperately need the implementation
set(WBTEST_SOURCES AdaptiveSpinLockTest.cpp ThreadIdTest.cpp)


# General settings
if(NOT CMAKE_BUILD_TYPE)
  message(STATUS "Using default build type Release as none was specified with -DCMAKE_BUILD_TYPE=<mode>")
  set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Type of build." FORCE)
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

set(USE_ADDRESS_SANITIZER ON CACHE BOOL "Use -fsanitize=address for leak checking.")

set(USE_COVERAGE_ANALYSIS ON CACHE BOOL "Use -fprofile-arcs -ftest-coverage for making a coverage analysis.")
set_property(CACHE USE_COVERAGE_ANALYSIS PROPERTY STRINGS "ON" "HTML" "OFF")


if(${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU")
  set(CMAKE_AR gcc-ar)
  set(CMAKE_RANLIB gcc-ranlib)
  # set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold)
  # set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold)
elseif(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
  set(CMAKE_AR llvm-ar)
  set(CMAKE_RANLIB llvm-ranlib)
  # set(CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} -fuse-ld=gold)
  # set(CMAKE_SHARED_LINKER_FLAGS ${CMAKE_SHARED_LINKER_FLAGS} -fuse-ld=gold)
endif()

function(SetupTarget target_name)
  set_property(TARGET ${target_name} PROPERTY CXX_STANDARD 11)
  target_compile_options(${target_name} PRIVATE -Wall -pedantic -Wextra -Werror)
  if(USE_ADDRESS_SANITIZER)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
    target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
  endif()
  if(USE_COVERAGE_ANALYSIS)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fprofile-arcs>)
    target_compile_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-ftest-coverage>)
    target_link_options(${target_name} PRIVATE $<$<CONFIG:Debug>:-fprofile-arcs>)
  endif()
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:Release>:-flto>)
  target_compile_options(${target_name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-flto>)
  target_link_options(${target_name} PRIVATE $<$<CONFIG:Release>:-flto>)
  target_link_options(${target_name} PRIVATE $<$<CONFIG:RelWithDebInfo>:-flto>)
  target_include_directories(${target_name} PRIVATE "include")
endfunction(SetupTarget)

unset(REAL_SOURCES)
foreach(CPPFILE ${SOURCES})
  set(REAL_SOURCES ${REAL_SOURCES} src/${CPPFILE})
endforeach(CPPFILE)

unset(REAL_WBTEST_SOURCES)
foreach(CPPFILE ${BBTEST_SOURCES} ${WBTEST_SOURCES})
  set(REAL_WBTEST_SOURCES ${REAL_WBTEST_SOURCES} test/${CPPFILE})
endforeach(CPPFILE)

unset(REAL_BBTEST_SOURCES)
foreach(CPPFILE ${BBTEST_SOURCES})
  set(REAL_BBTEST_SOURCES ${REAL_BBTEST_SOURCES} test/${CPPFILE})
endforeach(CPPFILE)


# Libraries
find_package(Threads)
# find_package(PkgConfig)
include(googletest.cmake)


# Target: static library
add_library(${PROJECT_NAME}_static STATIC ${REAL_SOURCES})
SetupTarget(${PROJECT_NAME}_static)

# Target: dynamic library
add_library(${PROJECT_NAME} SHARED ${REAL_SOURCES})
SetupTarget(${PROJECT_NAME})
if(Threads_FOUND)
  target_link_libraries(${PROJECT_NAME} ${CMAKE_THREAD_LIBS_INIT})
endif()

# Target: only blackbox tests link to dynamic library
add_executable(${PROJECT_NAME}_bbtest ${REAL_BBTEST_SOURCES})
SetupTarget(${PROJECT_NAME}_bbtest)
target_link_libraries(${PROJECT_NAME}_bbtest ${PROJECT_NAME} gtest_main)
if(Threads_FOUND)
  target_link_libraries(${PROJECT_NAME}_bbtest ${CMAKE_THREAD_LIBS_INIT})
endif()
add_test(NAME ${PROJECT_NAME}_bbtest COMMAND ${PROJECT_NAME}_bbtest)

# Target: all tests (whitebox+blackbox) link to static library
add_executable(${PROJECT_NAME}_wbtest ${REAL_WBTEST_SOURCES})
SetupTarget(${PROJECT_NAME}_wbtest)
target_include_directories(${PROJECT_NAME}_wbtest PRIVATE "src")
target_link_libraries(${PROJECT_NAME}_wbtest ${PROJECT_NAME}_static gtest_main)
if(Threads_FOUND)
  target_link_libraries(${PROJECT_NAME}_wbtest ${CMAKE_THREAD_LIBS_INIT})
endif()
add_test(NAME ${PROJECT_NAME}_wbtest COMMAND ${PROJECT_NAME}_wbtest)

# Target: coverage
find_program(GCOVR gcovr)
if(GCOVR)
  if(USE_COVERAGE_ANALYSIS AND ${CMAKE_BUILD_TYPE} STREQUAL "Debug")
    set(GCOV_EXE "gcov")
    if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
      set(GCOV_EXE "llvm-cov gcov")
    endif()
    if(${USE_COVERAGE_ANALYSIS} STREQUAL "ON")
      add_custom_target(
        coverage gcovr --gcov-executable "${GCOV_EXE}" --delete -su -r .. -e ../test -e googletest-src) 
    elseif(${USE_COVERAGE_ANALYSIS} STREQUAL "HTML")
      add_custom_target(
        coverage ${CMAKE_COMMAND} -E make_directory coverage
        COMMAND gcovr --gcov-executable "${GCOV_EXE}" --delete --html-details -su -r .. -e ../test -e googletest-src -o coverage/coverage.html 
        COMMENT "Generating coverage report into coverage/coverage.html...")
    endif()
  endif()
elseif(USE_COVERAGE_ANALYSIS)
  message(STATUS "Install gcovr to get coverage reports!")
endif()

#Target: docs
find_program(DOXYGEN doxygen)
if(DOXYGEN)
  add_custom_target(docs doxygen ../Doxyfile
                    COMMENT "Generating documentation into docs...")
else()
  message(STATUS "Install doxygen to get the documentation generated!")
endif()
