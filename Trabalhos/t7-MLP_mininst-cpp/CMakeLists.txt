cmake_minimum_required(VERSION 3.26)
project(t7_MLP_mininst_cpp)

set(CMAKE_CXX_STANDARD 23)
find_package(OpenMP)
if (OPENMP_FOUND)
    message("omp found")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif ()

add_library(MPLCpp STATIC mlp/MLP.cpp mlp/MLP.h mlp/Histogram.cpp mlp/Histogram.h mlp/config.h)
add_library(MPL2Cpp STATIC mlp/config.h mlp/MLP2.cpp mlp/MLP2.h)

add_executable(t7_MLP_mininst_cpp main.cpp mininst.h)
add_executable(t7_MLP2_mininst_cpp main2.cpp mininst.h)
target_link_libraries(t7_MLP_mininst_cpp MPLCpp)
target_link_libraries(t7_MLP2_mininst_cpp MPL2Cpp)
