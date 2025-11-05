#!/bin/bash

cd ../..
git clone https://github.com/polyseqtech/hdf5-compression-bench
cd ./hdf5-compression-bench/
sh merge_hdf5.sh

cd ../hdf5-compression-research-lifuzhou/script
bash change_filter.sh 2>&1 | tee change_filter.log
less change_filter.log | grep cost | sed 's/compress_leve//g' > result.txt
