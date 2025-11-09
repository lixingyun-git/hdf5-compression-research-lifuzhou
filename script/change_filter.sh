#!/bin/bash

export HDF5_PLUGIN_PATH=/root/third_lib_plugin/

root_src_path=`readlink -f ../src_cpp`

cmd='${root_src_path}/build/main --source_file=${root_src_path}/../../hdf5-compression-bench/PBG08621_pass_6c7986d6_167483a9_0.hdf5 --filter=NO --dest_file=NO_compression.hdf5'
echo cmd ${cmd}
${root_src_path}/build/main --source_file=${root_src_path}/../../hdf5-compression-bench/PBG08621_pass_6c7986d6_167483a9_0.hdf5 --filter=NO --dest_file=NO_compression.hdf5

exit
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=0 --dest_file=GZIP_compression_level0.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=3 --dest_file=GZIP_compression_level3.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=6 --dest_file=GZIP_compression_level6.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=9 --dest_file=GZIP_compression_level9.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=ZSTD --compress_level=0 --dest_file=ZSTD_compression_level0.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=ZSTD --compress_level=3 --dest_file=ZSTD_compression_level3.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=ZSTD --compress_level=6 --dest_file=ZSTD_compression_level6.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=ZSTD --compress_level=9 --dest_file=ZSTD_compression_level9.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=VBZ  --compress_level=0 --dest_file=VBZ_compression_level0.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=VBZ  --compress_level=3 --dest_file=VBZ_compression_level3.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=VBZ  --compress_level=6 --dest_file=VBZ_compression_level6.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=VBZ  --compress_level=9 --dest_file=VBZ_compression_level9.hdf5
${root_src_path}/build/main --source_file=NO_compression.hdf5 --filter=LZ4  --dest_file=LZ4_compression.hdf5
