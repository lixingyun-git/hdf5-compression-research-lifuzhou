# hdf5-compression-research-lifuzhou
hdf5 research

阿里云创建ecs操作完成

目前只支持GZIP，LZ4，ZSTD, VBZ算法

结果可以通过python脚本生成对比图: 目前分析结果：使用zstd即可完成目标

### 编译filter

cd scrpit

bash build_filter_plugin.sh

### 生成main

cd ../src

make

### 执行数据操作

sh get_result.sh

### 生成对比图

python get_graph.py

### 结果：
filter NO_6 cost 12045ms file_size:697662K

filter GZIP_0 cost 4244ms file_size:699617K

filter GZIP_3 cost 26028ms file_size:300570K

filter GZIP_6 cost 51025ms file_size:290173K

filter GZIP_9 cost 79622ms file_size:280199K

filter ZSTD_0 cost 9750ms file_size:309599K

filter ZSTD_3 cost 10389ms file_size:309599K

filter ZSTD_6 cost 16554ms file_size:288921K

filter ZSTD_9 cost 27044ms file_size:288882K

filter VBZ_0 cost 4165ms file_size:699617K

filter VBZ_3 cost 27017ms file_size:300570K

filter VBZ_6 cost 49391ms file_size:290173K

filter VBZ_9 cost 82107ms file_size:280199K

filter LZ4_6 cost 5365ms file_size:500151K

<img width="1990" height="1320" alt="a554c982-72a5-4a18-a06d-6207dca122b7" src="https://github.com/user-attachments/assets/75170bd8-0fd0-42fd-8793-117ad5cc94e4" />

需要有一种方式最终校验数据正确性
