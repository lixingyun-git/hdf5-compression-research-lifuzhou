#!/bin/bash

root_path=/root/

mkdir ${root_path}/plugins_source_code -p

function install_depend() {
	apt update -y
	apt install -y zlib1g-dev
	apt install -y cmake
	apt install -y libhdf5-dev
	apt install -y build-essential autoconf automake libtool pkg-config
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
	make IGNORE_TARGET=pyvbz -j8
	#make install
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

install_depend
install_vbz_compression
install_lz4


