cmake_minimum_required(VERSION 3.21)
project(CacheLab C)

set(CMAKE_C_STANDARD 99)

add_executable(CacheLab main.c)
target_link_libraries(CacheLab m pthread)