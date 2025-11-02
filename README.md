# hdf5-compression-research-lifuzhou
hdf5 research

记录操作步骤：阿里云创建ecs操作

apt update

apt install libzstd_dev zlib1g-dev cmake

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


export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/root/hdf5-hdf5_1.14.6/hdf5/lib

export HDF5_PLUGIN_PATH=/root/vbz_compression/build/bin

./main hdf5-compression-bench/PBG08621_pass_6c7986d6_167483a9_0.hdf5 
