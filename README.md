# hdf5-compression-research-lifuzhou
hdf5 research

阿里云创建ecs操作完成

目前只支持GZIP，LZ4，ZSTD, LZ4, VBZ算法

结果可以通过python脚本生成对比图

编译filter
cd scrpit
bash build_filter_plugin.sh

生成main
cd ../src
make

执行数据操作
sh get_result.sh

生成对比图
python get_graph.py



vbz 暂时不可用与压缩

<img width="1000" height="664" alt="image" src="https://github.com/user-attachments/assets/a8db06ed-6aa3-4949-bc84-f0c29ff74b58" />

需要有一种方式最终校验数据正确性
