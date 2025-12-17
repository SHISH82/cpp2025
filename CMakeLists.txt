cmake_minimum_required(VERSION 3.30)
project(CPP_Lab_2)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(CPP_Lab_2 src/main.cpp src/subject.cpp)

target_include_directories(CPP_Lab_2 PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
)