cmake_minimum_required(VERSION 3.26)
project(t6_MLP_cpp)

set(CMAKE_CXX_STANDARD 23)
find_package(OpenMP)
if (OPENMP_FOUND)
    message("omp found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()
find_package(Threads REQUIRED)


#include_directories(http mlp)
include_directories(.)



add_library(SHA256 STATIC http/SHA256.cpp http/SHA256.h)
add_library(MPLCpp STATIC mlp/MLP.cpp mlp/MLP.h mlp/Histogram.cpp mlp/Histogram.h mlp/config.h)
add_library(HTTPCpp STATIC http/json.cpp http/json.h http/httpserver.h http/httpserver.cpp
        http/sha256.cpp
        http/sha256.h
        http/sha256.h
)

add_executable(t6MPLtest mlp/main.cpp)
target_link_libraries(t6MPLtest MPLCpp)

link_libraries(Ws2_32)
add_executable(t6Http html/main.cpp)
target_link_libraries(t6Http MPLCpp HTTPCpp SHA256 Threads::Threads)