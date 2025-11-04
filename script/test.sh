#!/bin/bash

../src/main --source_file=/root/hdf5-compression-bench/PBG08621_pass_6c7986d6_167483a9_0.hdf5 --filter=NO --dest_file=NO_compression.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=0 --dest_file=GZIP_compression_level0.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=1 --dest_file=GZIP_compression_level1.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=2 --dest_file=GZIP_compression_level2.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=3 --dest_file=GZIP_compression_level3.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=4 --dest_file=GZIP_compression_level4.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=5 --dest_file=GZIP_compression_level5.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=6 --dest_file=GZIP_compression_level6.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=7 --dest_file=GZIP_compression_level7.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=8 --dest_file=GZIP_compression_level8.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=GZIP --compress_level=9 --dest_file=GZIP_compression_level9.hdf5
../src/main --source_file=NO_compression.hdf5 --filter=LZ4  --dest_file=LZ4_compression.hdf5
