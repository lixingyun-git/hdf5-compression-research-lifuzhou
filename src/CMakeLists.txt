cmake_minimum_required(VERSION 3.10.2)
project(main)

link_directories(/usr/lib/x86_64-linux-gnu/hdf5/serial)
include_directories(/usr/include/hdf5/serial/)

#set(CMAKE_VERBOSE_MAKEFILE ON)
# 启用调试信息
#set(CMAKE_BUILD_TYPE Debug)

# 添加一个静态库目标
add_library(change_filter STATIC change_filter.cpp)
target_link_libraries(change_filter hdf5)

add_executable(main new_main.cpp)
target_link_libraries(main gflags change_filter)

add_executable(googletest change_filter_test.cpp)
target_link_libraries(googletest change_filter gtest)
