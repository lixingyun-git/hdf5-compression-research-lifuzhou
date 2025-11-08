cmake_minimum_required(VERSION 3.10.2)
project(main)

link_directories(../third_lib_and_plugin/lib)
include_directories(../third_lib_and_plugin/include)

#set(CMAKE_VERBOSE_MAKEFILE ON)
# 启用调试信息
set(CMAKE_BUILD_TYPE Debug)

# 添加一个静态库目标
add_library(change_filter STATIC change_filter.cpp)
target_link_libraries(change_filter hdf5 hdf5_cpp)

add_executable(main new_main.cpp)
target_link_libraries(main gflags change_filter)

add_executable(googletest change_filter_test.cpp)
target_link_libraries(googletest change_filter gtest)

#set(CMAKE_INSTALL_PREFIX ../third_lib_and_plugin)
#set(DHDF5_BUILD_CPP_LIB ON)
#add_subdirectory(../third/hdf5-1.14.6 ../third/hdf5-1.14.6/build)
#
#
#set(CMAKE_INSTALL_PREFIX ../third_lib_and_plugin)
#add_subdirectory(../third/vbz_compression ../third/vbz_compression/build)
#
#set(CMAKE_INSTALL_PREFIX ../third_lib_and_plugin)
#set(LZ4_PACKAGE_NAME LZ4)
#set(CMAKE_PREFIX_PATH ../third_lib_and_plugin)
#set(H5PL_ALLOW_EXTERNAL_SUPPORT GIT)
#add_subdirectory(../third/hdf5_plugins/LZ4 ../third/hdf5_plugins/LZ4/build)
#
#add_subdirectory(../third/zstd ../third/zstd/build)
#set(ZSTD_PACKAGE_NAME zstd)
#set(H5ZSTD_RESOURCES_DIR /root/myhdfstuff/hdf5_plugins-hdf5-1.14.6/ZSTD/config/cmake)
#add_subdirectory(../third/hdf5_plugins/ZSTD ../third/hdf5_plugins/ZSTD/build)
#
#add_subdirectory(../third/hdf5-blosc ../third/hdf5-blosc/build)
#
