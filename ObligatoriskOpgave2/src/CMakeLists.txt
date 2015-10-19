cmake_minimum_required(VERSION 3.3)
project(ObligatoriskOpgave2)

set(SOURCE_FILES banker/banker.c)
set(CMAKE_C_FLAGS -g)

SET(GCC_COVERAGE_COMPILE_FLAGS "-lm -pthread")

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})

link_libraries(m)
link_libraries(pthread)

add_executable(ObligatoriskOpgave2 ${SOURCE_FILES})