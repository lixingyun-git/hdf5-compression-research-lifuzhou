#!/bin/bash

root_path=/root/

# 正式环境不要使用git clone, 下载指定release版本

mkdir ${root_path}/plugins_source_code -p

function install_depend() {
	apt update -y
	apt install -y zlib1g-dev
	apt install -y cmake
	apt install -y libhdf5-dev
	apt install -y build-essential autoconf automake libtool pkg-config
	apt install -y libgflags-dev libgtest-dev
	pip install conan==1.60
}

function install_vbz_compression() {
	cd ${root_path}/plugins_source_code
	git clone https://github.com/nanoporetech/vbz_compression.git
	cd vbz_compression
	git submodule update --init
	mkdir build -p
	cd build
	mkdir ${root_path}/third_lib_plugin -p
	cmake .. -DCMAKE_INSTALL_PREFIX=${root_path}/third_lib_plugin
	make -j8 -k
	make install
	cp bin/*.so ${root_path}/third_lib_plugin/
}

function install_lz4() {
	cd ${root_path}/plugins_source_code
	git clone https://github.com/HDFGroup/hdf5_plugins.git
	cd hdf5_plugins
	cd LZ4
	mkdir build -p
	cd build
	cmake .. -DLZ4_PACKAGE_NAME=LZ4 -DCMAKE_PREFIX_PATH=/usr/local/ -DH5PL_ALLOW_EXTERNAL_SUPPORT=GIT
	make -j8
	make install
	cp bin/*.so ${root_path}/third_lib_plugin/
}

function install_zstd() {
	cd ${root_path}/plugins_source_code
	git clone https://github.com/facebook/zstd.git
	cd zstd/
	cd build/
	cd cmake/
	mkdir build
	cd build/
	cmake ..
	make -j8
	make install


	cd ${root_path}/plugins_source_code
	git clone https://github.com/HDFGroup/hdf5_plugins.git
	cd hdf5_plugins
	cd ZSTD
	mkdir build -p
	cd build
	cmake .. -DZSTD_PACKAGE_NAME=zstd -DH5ZSTD_RESOURCES_DIR=/root/myhdfstuff/hdf5_plugins-hdf5-1.14.6/ZSTD/config/cmake
	make -j8
	cp bin/*.so ${root_path}/third_lib_plugin/
}

function install_blosc() {
	cd ${root_path}/plugins_source_code
	git clone https://github.com/Blosc/hdf5-blosc.git
        cd hdf5-blosc/
        mkdir build
        cd build/
        cmake ..
        make -j8
        make install
	cp libblosc_filter.so  libH5Zblosc.so ${root_path}/third_lib_plugin/
}

install_depend
install_vbz_compression
install_lz4
install_zstd
install_blosc

