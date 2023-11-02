cmake_minimum_required(VERSION 3.26)
project(trabalhos)

set(CMAKE_CXX_STANDARD 23)
find_package(OpenMP REQUIRED)
if(OPENMP_FOUND)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

add_executable(trabalhos main.cpp
        src/MLP.cpp
        src/MLP.h
        src/config.h
        src/Func.cpp
        src/Func.h
)
