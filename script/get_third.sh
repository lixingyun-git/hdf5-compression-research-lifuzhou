#!/bin/bash


root_path=../

# 正式环境不要使用git clone, 下载指定release版本

mkdir ${root_path}/third -p

function install_depend() {
	apt update -y
	apt install -y zlib1g-dev
	apt install -y cmake
	apt install -y libhdf5-dev
	apt install -y build-essential autoconf automake libtool pkg-config
	apt install -y libgflags-dev libgtest-dev
	pip install conan==1.60
}

function install_hdf5() {
	cd ${root_path}/third
	wget https://support.hdfgroup.org/releases/hdf5/v1_14/v1_14_6/downloads/hdf5-1.14.6.tar.gz
	tar xf hdf5-1.14.6.tar.gz
}

function install_vbz_compression() {
	cd ${root_path}/third
	git clone https://github.com/nanoporetech/vbz_compression.git
	cd vbz_compression
	git submodule update --init
}

function install_lz4() {
	cd ${root_path}/third
	git clone https://github.com/HDFGroup/hdf5_plugins.git
}

function install_zstd() {
	cd ${root_path}/third
	git clone https://github.com/facebook/zstd.git
	git clone https://github.com/HDFGroup/hdf5_plugins.git
}

function install_blosc() {
	cd ${root_path}/third
	git clone https://github.com/Blosc/hdf5-blosc.git
}

install_depend
install_hdf5
install_vbz_compression
install_lz4
install_zstd
install_blosc

