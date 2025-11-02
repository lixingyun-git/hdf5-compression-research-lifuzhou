# hdf5-compression-research-lifuzhou
hdf5 research

记录操作步骤：阿里云创建ecs操作

apt update

apt install cmake libzstd_dev install zlib1g-dev

pip install conan==1.60

git clone https://github.com/polyseqtech/hdf5-compression-bench

cd hdf5-compression-bench

sh merge_hdf5.sh

cd -

wget https://github.com/HDFGroup/hdf5/archive/refs/tags/hdf5_1.14.6.tar.gz

tar xf hdf5_1.14.6.tar.gz

./configure

make -j8

make install

git clone https://github.com/nanoporetech/vbz_compression.git

cd vbz_compression

git submodule update --init

mkdir build && cd build

cmake ..

make -j8
   
main.cpp

Makefile
