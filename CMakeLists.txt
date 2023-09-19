cmake_minimum_required(VERSION 3.25)
project(CREST)

set(CMAKE_CXX_STANDARD 23)
link_libraries(ws2_32)
include_directories(./)



add_library(cppRestApi STATIC http/json.cpp http/json.h http/httpserver.cpp http/httpserver.h)
add_library(perceptronApi STATIC Perceptron/perceptron.cpp Perceptron/perceptron.h)

link_libraries(cppRestApi)
link_libraries(perceptronApi)
find_package(OpenMP)
if (OPENMP_FOUND)
    message("omp finded")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_executable(CREST main.cpp http/json.h http/json.cpp http/httpserver.cpp http/httpserver.h)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY trabalho1)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY trabalho1)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY trabalho1)

add_executable(T1 trabalho1/main.cpp)
