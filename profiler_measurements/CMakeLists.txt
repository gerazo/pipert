cmake_minimum_required(VERSION 3.10)
project (TestLibrary)

set(CMAKE_CXX_STANDARD 11)

#For the shared library:
set ( PROJECT_LINK_LIBS /home/user/Desktop/scinarios/pipert/build_release/libpipert.so )
link_directories(../build)

include_directories(../include; ../src)
include_directories(../include; pipert/profiler_measurements)



add_executable(measurementScenarios MeasurementsRunner.cpp)
target_link_libraries(measurementScenarios ${PROJECT_LINK_LIBS} ${OpenCV_LIBS})

target_compile_options(measurementScenarios PRIVATE $<$<CONFIG:Debug>:-fsanitize=address>)
set_target_properties(measurementScenarios PROPERTIES LINK_FLAGS_DEBUG "-fsanitize=address")