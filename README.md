# hdf5-compression-research-lifuzhou
hdf5 research

# 问题
1、当前使用的是拷贝修改的方式，但是拷贝内容只有有dataset的目录，可能存在其他内容的路径没有拷贝

2、使用c++实现更好，当前实现方案需要处理的异常太多，可能存在的问题太多，最好好好阅读资料是使用c++实现，使用其代码封装各种细节。



阿里云创建ecs操作完成

目前只支持GZIP，LZ4，ZSTD, VBZ算法

结果可以通过python脚本生成对比图: 目前分析结果：使用zstd即可完成目标

### 编译filter

cd scrpit

bash build_filter_plugin.sh

### 生成main

cd ../src

mkdir build

cd build

cmake ..

make

### 执行数据操作

sh get_result.sh

### 生成对比图

python get_graph.py

### 结果：
filter NO_6 cost 11560ms file_size:697662K

filter GZIP_0 cost 4039ms file_size:699617K

filter GZIP_3 cost 25664ms file_size:300570K

filter GZIP_6 cost 47825ms file_size:290173K

filter GZIP_9 cost 78527ms file_size:280199K

filter ZSTD_0 cost 9642ms file_size:309599K

filter ZSTD_3 cost 9759ms file_size:309599K

filter ZSTD_6 cost 16193ms file_size:288921K

filter ZSTD_9 cost 26728ms file_size:288882K

filter VBZ_0 cost 4133ms file_size:699617K

filter VBZ_3 cost 25439ms file_size:300570K

filter VBZ_6 cost 48003ms file_size:290173K

filter VBZ_9 cost 78060ms file_size:280199K

filter LZ4_6 cost 5270ms file_size:500151K

<img width="2000" height="1328" alt="8c68e5526c9b439c08b756edc689bdf2" src="https://github.com/user-attachments/assets/333d1c65-a4b5-402c-989a-d55b9c96a737" />


需要有一种方式最终校验数据正确性
